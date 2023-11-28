

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <cxxutil/core/mem.h>

namespace cxxutil { namespace data {

    template <typename T>
    struct HeapEntry {
        core::U24 key;
        T value;
    };

    // This is a max heap.
    template <typename T>
    class Heap : public core::SafeObject {
    private:
        core::U24 (* const keyFunc)(const T &);

        core::SafeArray<T> *table;
        size_t len;

    public:
        Heap(core::U24 (*kf)(const T &), size_t initCap) 
            : Heap(core::CXX_DEF_CHNL, kf, initCap) {
        }

        Heap(uint8_t chnl, core::U24 (*kf)(const T &), size_t initCap) 
            : keyFunc(kf) {
            // Init cap must be larger than 0.
            size_t actualInitCap = initCap == 0 ? 1 : initCap;

            this->table = new core::SafeArray<T>(chnl, actualInitCap);
            this->len = 0;                
        }

        ~Heap() {
            delete this->table; 
        }

        void push(const T &ele) {
            if (this->len == table->getLen()) {
                core::SafeArray<T> *newTable = 
                    core::resize(this->getChnl(), this->table, table->getLen() * 2);

                delete this->table;
                this->table = newTable;
            }

            // At this point, we know our table is large enough.

            // Let's create our random key.
            // uint24_t will be large enough to hold the random number.
            core::U24 rkey = rand(); 

            // Now we need to bubble up.

            this->len++;
        }

        T pop() {

        }
    };
}}
