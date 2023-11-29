

#include "ti/screen.h"
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

    // This is a min heap by keys.
    template <typename T>
    class Heap : public core::SafeObject {
    public:
        core::U24 (* const keyFunc)(const T &);

        core::SafeArray<HeapEntry<T>> *table;
        size_t len;

    public:
        Heap(core::U24 (*kf)(const T &), size_t initCap) 
            : Heap(core::CXX_DEF_CHNL, kf, initCap) {
        }

        Heap(uint8_t chnl, core::U24 (*kf)(const T &), size_t initCap) 
            : keyFunc(kf) {
            // Init cap must be larger than 0.
            size_t actualInitCap = initCap == 0 ? 1 : initCap;

            this->table = new core::SafeArray<HeapEntry<T>>(chnl, actualInitCap);
            this->len = 0;                
        }

        ~Heap() {
            delete this->table; 
        }

        inline size_t getLength() const {
            return this->len;
        }

        void push(T ele) {
            // Resize if we do not have space.
            if (this->len == table->getLen()) {
                core::SafeArray<HeapEntry<T>> *newTable = 
                    core::resize(this->getChnl(), this->table, table->getLen() * 2);

                delete this->table;
                this->table = newTable;
            }

            // At this point, we know our table is large enough.

            // Let's create our key.
            core::U24 key = this->keyFunc(ele);

            size_t iter = this->len;
            size_t parent = (iter-1) / 2;

            // Do bubble up.
            // iter always points to an empty cell.
            while (iter > 0 && this->table->get(parent).key > key) {
                this->table->set(iter, this->table->get(parent));

                iter = parent;
                parent = (iter-1) / 2;
            }

            // At this point, iter points to a cell which can
            // and will be overwritten with the given value.

            HeapEntry<T> *entry = this->table->getPtrMut(iter);

            entry->key = key;
            entry->value = ele;

            // Finally increase our length.
            this->len++;
        }

        // NOTE: UB if empty.
        T pop() {
            // NOTE: this should still work, even if there is only 
            // one element.

            // Save our return value.
            T rootValue = this->table->get(0).value;

            // The entry to be sifted down.
            HeapEntry<T> siftEntry = this->table->get(this->len - 1);

            this->len--; // Shrink heap by 1.
            
            // Skip needless work if there are no more elements in the heap.
            if (this->len == 0) {
                return rootValue;
            }
            
            size_t iter = 0;

            // This loop will reorganize the heap and find the correct
            // position for the sift Entry to be placed.

            while (true) {
                // Try left.
                size_t leftInd = (2 * iter) + 1;

                // Check if iter has any children at all.
                if (leftInd >= this->len) {
                    break;
                }

                // If it does, find which child has the smallest
                // key.

                size_t minInd = leftInd;

                size_t rightInd = (2 * iter) + 2;
                if (rightInd < this->len && 
                        this->table->get(rightInd).key < this->table->get(leftInd).key) {
                    minInd = rightInd;
                }

                if (siftEntry.key < this->table->get(minInd).key) {
                    // The sifted key is less than both of its
                    // childrens' keys. we are done!
                   
                    break;
                }

                this->table->set(iter, this->table->get(minInd));
                iter = minInd;
            }

            this->table->set(iter, siftEntry);

            return rootValue;
        }
    };
}}
