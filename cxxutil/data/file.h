
#include "cxxutil/core/mem.h"
#include <stdlib.h>
#include <fileioc.h>

namespace cxxutil { namespace data {

    template <typename T>
    class FileWriter : public core::SafeObject {
    protected:
        FileWriter() : FileWriter(core::CXX_DEF_CHNL) {
        }

        FileWriter(uint8_t chnl) : core::SafeObject(chnl) {
        }
    public:
        virtual ~FileWriter() = default;

        // Should return false if write fails.
        virtual bool write(uint8_t handle, T element) = 0;
    };

    template <typename T>
    class FileReader : public core::SafeObject {
    protected:
        FileReader() : FileReader(core::CXX_DEF_CHNL) {
        }

        FileReader(uint8_t chnl) : core::SafeObject(chnl) {
        }
    public:
        virtual ~FileReader() = default;

        // Should return false on failure.
        virtual bool read(uint8_t handle, T *dest) = 0;
    };

    // NOTE:
    // Below are a few helpers for reading and writing arrays to/from files.
    // The "Shallow" version is for arrays which are self contained.
    // i.e. an array of integers or structs. These can just be copied.
    //
    // The "Deep" Version allows for recursion while reading from a file.
    // Each element can be created using its own reader/writer.

    template <typename T>
    class ShallowArrayFileWriter : public FileWriter<core::SafeArray<T> *> {
    public:
        ShallowArrayFileWriter() : ShallowArrayFileWriter(core::CXX_DEF_CHNL) {
        }

        ShallowArrayFileWriter(uint8_t chnl) : FileWriter<core::SafeArray<T> *>(chnl) {
        }

        virtual ~ShallowArrayFileWriter() {
        }

        virtual bool write(uint8_t handle, core::SafeArray<T> *element) override {
            const size_t len = element->getLen();

            size_t lenRes = ti_Write(&len, sizeof(size_t), 1, handle);

            if (lenRes != 1) {
                return false;
            }

            const T *arr = element->getArr();
            size_t arrRes = ti_Write(arr, sizeof(T), len, handle);

            if (arrRes != len) {
                return false;
            }

            return true;
        }
    };

    template <typename T>
    class ShallowArrayFileReader : public FileReader<core::SafeArray<T> *> {
    public:
        ShallowArrayFileReader() 
            : ShallowArrayFileReader(core::CXX_DEF_CHNL) {
        }

        ShallowArrayFileReader(uint8_t chnl) 
            : FileReader<core::SafeArray<T> *>(chnl) {
        }

        virtual ~ShallowArrayFileReader() {
        }

        virtual bool read(uint8_t handle, core::SafeArray<T> **dest) override {
            size_t len;
            size_t lenRes = ti_Read(&len, sizeof(size_t), 1, handle);    

            if (lenRes != 1) {
                return false;
            }

            core::SafeArray<T> *arr = new core::SafeArray<T>(this->getChnl(), len);
            size_t arrRes = ti_Read(arr->getArrMut(), sizeof(T), len, handle);

            if (arrRes != len) {
                delete arr;
                return false;
            }

            *dest = arr;

            return true;
        }
    };

    template <typename T> 
    class DeepArrayFileWriter : public FileWriter<core::SafeArray<T> *> {
    private:
        FileWriter<T> * const elementWriter;
    public:
        DeepArrayFileWriter(FileWriter<T> *ew) 
            : DeepArrayFileWriter(core::CXX_DEF_CHNL, ew) {
        }

        DeepArrayFileWriter(uint8_t chnl, FileWriter<T> *ew) 
            : FileWriter<core::SafeArray<T> *>(chnl), elementWriter(ew) {
        }

        virtual ~DeepArrayFileWriter() {
        }

        virtual bool write(uint8_t handle, core::SafeArray<T> *element) override {
            const size_t len = element->getLen();
            
            // Length will be written first.
            size_t lenRes = ti_Write(&len, sizeof(size_t), 1, handle);

            if (lenRes != 1) {
                return false;
            }

            // Now write all the elements!
            for (size_t i = 0; i < len; i++) {
                bool eleRes = 
                    this->elementWriter->write(handle, element->get(i));

                if (!eleRes) {
                    return false;
                }
            }

            return true;
        }
    };

    template <typename T>
    class DeepArrayFileReader : public FileReader<core::SafeArray<T> *> {
    private:
        FileReader<T> * const elementReader;

    public:
        DeepArrayFileReader(FileReader<T> *er) 
            : DeepArrayFileReader(core::CXX_DEF_CHNL, er) {
        }

        DeepArrayFileReader(uint8_t chnl, FileReader<T> *er) 
            : FileReader<core::SafeArray<T> *>(chnl), elementReader(er) {
        }

        virtual ~DeepArrayFileReader() {
        }

        virtual bool read(uint8_t handle, core::SafeArray<T> **dest) override {
            size_t len;
            size_t lenRes = ti_Read(&len, sizeof(size_t), 1, handle);    

            if (lenRes != 1) {
                return false;
            }

            core::SafeArray<T> *arr = 
                new core::SafeArray<T>(this->getChnl(), len);

            for (size_t i = 0; i < len; i++) {
                bool eleRes = 
                    this->elementReader->read(handle, arr->getPtrMut(i));

                if (!eleRes) {
                    delete arr;
                    return false;
                }
            }

            *dest = arr;
            return true;
        }
    };
}};
