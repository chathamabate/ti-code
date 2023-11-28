

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

        inline size_t getLength() const {
            return this->len;
        }

        void push(const T &ele) {
            // Resize if we do not have space.
            if (this->len == table->getLen()) {
                core::SafeArray<T> *newTable = 
                    core::resize(this->getChnl(), this->table, table->getLen() * 2);

                delete this->table;
                this->table = newTable;
            }

            // At this point, we know our table is large enough.

            // Let's create our key.
            core::U24 key = this->keyFunc(ele);

            size_t iter = this->len;
            size_t parent = iter / 2;

            // Do bubble up.
            // iter always points to an empty cell.
            while (iter > 0 && this->table->get(parent).key > key) {
                this->table->set(iter, this->table->get(parent));

                iter = parent;
                parent = iter / 2;
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

            core::U24 shiftKey = this->table->get(this->len - 1).key;

            // Move last element to the root.
            this->table->set(0, this->table->get(this->len - 1));
            this->len--; // Shrink heap by 1.
            
            // We will always try to go left first.
            // I don't think this will cause any problems with randomness.

            // Now we sift our new root down.
            
            size_t iter = 0;

            while (true) {
                // Try left.
                size_t leftInd = (2 * iter) + 1;

                if (leftInd < this->len && this->table->get(leftInd).key < shiftKey) {
                    this->table->set(iter, this->table->get(leftInd));
                    iter = leftInd;

                    continue;
                } 

                // Try right.
                size_t rightInd = (2 * iter) + 2;

                if (rightInd < this->len && this->table->get(rightInd).key < shiftKey) {
                    this->table->set(iter, this->table->get(rightInd));
                    iter = rightInd;

                    continue;
                }

                // Otherwise we're done.
                return rootValue; 
            }
        }
    };
}}
