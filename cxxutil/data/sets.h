
#include <stdint.h>
#include <cxxutil/core/mem.h>

namespace cxxutil { namespace data {

    // NOTE: Set doesn't extend SafeObject as it is just
    // a interface.
    //
    // NOTE: Type T should have a safe use == and = operators.
    
    template <typename T>
    class Set {
    public:
        virtual ~Set() = default;

        // How many elements are in the set currently.
        virtual size_t size() const = 0;

        virtual bool contains(const T &ele) const  = 0;

        virtual void add(const T &ele) = 0;

        // Returns true if an element was removed.
        // If the element exists in this set,
        // it will always be removed.
        virtual bool remove(const T &ele) = 0;

        // Pops an element from the Set.
        // No restrictions on which element is popped.
        // NOTE: Unknown behavoir if the set is empty.
        virtual T pop() = 0;
    };

    enum class ProbedEntryState {
        EMPTY,
        FULL,
        DELETED,
    };

    template <typename T>
    struct ProbedEntry {
        // NOTE: As of now entries use up a good amount of memory.
        
        ProbedEntryState s; 

        // NOTE: the below fields are only used if the Entry
        // is full.
        
        // Hash of value. (Stored for quick resizing)
        // Honestly just using 16-bit here since 24-bit
        // doesn't work with the language server.
        uint16_t hash;

        // Indeces of previous and next full cell.
        // Used for fast random search.
        // They should = capcity if NULL.
        size_t prevFull;
        size_t nextFull;

        T value;
    };

    template <typename T>
    class ProbedHashSet : public core::SafeObject, public Set<T> {
    private:
        uint16_t (* const hashFunc)(const T &ele);

        // Hash table.
        core::SafeArray<ProbedEntry<T>> *tbl;
        size_t fill;    // Number of elements in the table.
        
        size_t headFull; // = cap when set is empty.

        // returns capacity if element is not in the set.
        size_t getInd(const T &ele) const {
            const uint16_t hash = this->hashFunc(ele);
            const size_t cap = this->tbl->getLen();
            const size_t expInd = hash % cap;

            size_t i = expInd;

            do {
                const ProbedEntry<T> *entry = this->tbl->getPtr(i);

                if (entry->s == ProbedEntryState::EMPTY) {
                    return cap;
                }

                if (entry->s == ProbedEntryState::FULL 
                        && entry->hash == hash      // Check hash first for speed.
                        && entry->value == ele) {
                    return i;
                }

                i = (i + 1) % cap;

            // Exit when we make it back to where we started.
            } while (i != expInd);

            return cap;
        }

        // Removes Cell at ind from the set.
        // NOTE: UB if tbl[ind] isn't full.
        void removeInd(size_t ind) {
            ProbedEntry<T> *entry = this->tbl->getPtrMut(ind);
            const size_t cap = this->tbl->getLen();

            // First, remove our entry from the full list.

            // If there is a next, point it back to prev.
            if (entry->nextFull < cap) {
                ProbedEntry<T> *next = this->tbl->getPtrMut(entry->nextFull);
                next->prevFull = entry->prevFull;
            }

            if (entry->prevFull < cap) {
                // If there is a prev, point it forward to next.
                ProbedEntry<T> *prev = this->tbl->getPtrMut(entry->prevFull);
                prev->nextFull = entry->nextFull;

            } else {
                // If there is no prev. Set next as the new set head.
                this->headFull = entry->nextFull;
            }

            // Next mark as deleted.

            entry->s = ProbedEntryState::DELETED;
            this->fill--;
        }
                        
    public:
        ProbedHashSet(uint16_t (*hf)(const T &ele)) 
            : ProbedHashSet(core::CXX_DEF_CHNL, hf) {
        }

        ProbedHashSet(uint8_t chnl, uint16_t (*hf)(const T &ele)) 
            : core::SafeObject(chnl), hashFunc(hf) {
        }

        virtual ~ProbedHashSet() {
            // Fill this in.
        }

        virtual inline size_t size() const override {
            return this->fill; 
        }


        virtual inline bool contains(const T &ele) const override {
            return (this->getInd(ele) < this->tbl->getLen());
        }

        virtual bool remove(const T &ele) override {
            size_t ind = this->getInd(ele);

            if (ind >= this->tbl->getLen()) {
                return false;
            }

            this->removeInd(ind);

            return true;
        }
        
    };

}}
