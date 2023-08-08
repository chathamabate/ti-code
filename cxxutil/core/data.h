#pragma once

#include <cstddef>
#include <cxxutil/core/mem.h>

namespace cxxutil { namespace core {
    template <typename T> class CoreList;

    // destLen is the length of the string currently held in dest.
    // (Not including /0)
    //
    // destSize is the length of the entire buffer.
    //
    // If this cat is too big, we will copy as much as we can.
    //
    // Returns the length of the string contained in dest
    // after the cat.
    size_t strCatSafe(char *dest, size_t destLen, size_t destSize, 
            const char *src);

    // Returns ths size of the string in dest after the multiple
    // cats.
    size_t multiStrCatSafe(char *dest, size_t destLen, size_t destSize,
            size_t numStrs, const char **strs);

    // Make a copy of s stored in a safe array.
    SafeArray<char> *strCopySafe(uint8_t memChnl, const char *s);

    template <typename T>
    class CoreList : public SafeObject {
    private:
        SafeArray<T> *safeArr;
        size_t len;

    public:
        CoreList() : CoreList(CXX_DEF_CHNL) { }

        CoreList(uint8_t memChnl) : SafeObject(memChnl) {
            this->safeArr = new SafeArray<T>(memChnl, 1);
            this->len = 0;
        }

        ~CoreList() {
            delete this->safeArr;
        }

        inline size_t getCap() const {
            return this->safeArr->getLen();
        }

        inline size_t getLen() const {
            return this->len;
        }

        inline T get(size_t i) const {
            return this->safeArr->get(i);
        }

        inline void clear() {
            this->len = 0;
        }

        // This copies the CoreList's data into a new SafeArray.
        SafeArray<T> *toArray() const {
            SafeArray<T> *arr = new SafeArray<T>(this->getChnl(), this->len);
            for (size_t i = 0; i < this->len; i++) {
                arr->set(i, this->safeArr->get(i));
            } 

            return arr;
        }

        void add(T ele) {
            size_t cap = this->safeArr->getLen();

            if (this->len == cap) {
                // Create a new array with double the capacity of the old array.
                SafeArray<T> *newArr = 
                    new SafeArray<T>(this->getChnl(), cap * 2);
                
                T *newArrUnder = newArr->getArr();

                // Copy elements of old array into the new array.
                for (size_t i = 0; i < cap; i++) {
                    newArrUnder[i] = this->safeArr->get(i);
                }

                // finally, delete old array and store the new one.
                delete this->safeArr;
                this->safeArr = newArr;
            }

            *(this->safeArr->getPtr(this->len)) = ele;
            this->len++;
        }
    };
}} 
