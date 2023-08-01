#pragma once

#include <cxxutil/core/mem.h>

namespace cxxutil {
namespace core {

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

    template <typename T>
    class CoreList : public SafeObject {
    private:
        SafeArray<T> *safeArr;
        int16_t len;

    public:
        CoreList() : CoreList(CXX_DEF_CHNL) { }

        CoreList(uint8_t memChnl) : SafeObject(memChnl) {
            this->safeArr = new SafeArray<T>(memChnl, 1);
            this->len = 0;
        }

        ~CoreList() {
            delete this->safeArr;
        }

        inline uint16_t getCap() {
            return this->safeArr->getLen();
        }

        inline uint16_t getLen() {
            return this->len;
        }

        void add(T ele) {
            uint16_t cap = this->safeArr->getLen();

            if (this->len == cap) {
                // Create a new array with double the capacity of the old array.
                SafeArray<T> *newArr = 
                    new SafeArray<T>(this->getChnl(), cap * 2);
                
                T *newArrUnder = newArr->getArr();

                // Copy elements of old array into the new array.
                for (uint16_t i = 0; i < cap; i++) {
                    newArrUnder[i] = this->safeArr->get(i);
                }

                // finally, delete old array and store the new one.
                delete this->safeArr;
                this->safeArr = newArr;
            }

            *(this->safeArr->getPtr(this->len)) = ele;
            this->len++;
        }

        inline T get(uint16_t i) {
            return this->safeArr->get(i);
        }
    };

}
} 
