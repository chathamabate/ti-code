
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
        ProbedEntryState s; 

        // NOTE: we will save hash to speed up resizing.
        uint24_t hash;

        T value;
    };

    template <typename T>
    class ProbedHashSet : public core::SafeObject, public Set<T> {
    private:
        uint24_t (* const hashFunc)(const T &ele);

    public:
        
    };

}}
