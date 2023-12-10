
#include "cxxutil/core/mem.h"
#include <stdlib.h>
#include <cxxutil/core/serial.h>
#include <fileioc.h>

namespace cxxutil { namespace core {

    template <typename T>
    class FileWriter : public core::SafeObject {
    protected:
        FileWriter() : FileWriter(core::CXX_DEF_CHNL) {
        }

        FileWriter(uint8_t chnl) : core::SafeObject(chnl) {
        }
    public:
        virtual ~FileWriter() = default;
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
        virtual T read(uint8_t handle) = 0;
    };

    // Here are some standard ways for reading and writing 
    // arrays!

    // Safe Array Helpers.
    template <typename T> 
    class ArrayFileWriter : public FileWriter<core::SafeArray<T> *> {
    private:
        FileWriter<T> * const elementWriter;
    public:
        ArrayFileWriter(FileWriter<T> *ew) 
            : ArrayFileWriter(core::CXX_DEF_CHNL, ew) {
        }

        ArrayFileWriter(uint8_t chnl, FileWriter<T> *ew) 
            : FileWriter<core::SafeArray<T> *>(chnl), elementWriter(ew) {
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
}};
