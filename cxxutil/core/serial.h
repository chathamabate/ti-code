
#include "cxxutil/core/mem.h"
#include <stdlib.h>
#include <cxxutil/core/serial.h>

namespace cxxutil { namespace core {
    class Serializable {
    public:
        virtual ~Serializable() = default;

        // This should be seen as a copy constructor.
        // The given objects resources should be replaced with
        // whatever is read from the buffer.
        //
        // Returns the number of bytes read from the buffer.
        virtual size_t constructFromBuffer(void *src) = 0;

        // This copies the given object into the given buffer.
        // 
        // Returns the number of bytes written.
        virtual size_t serializeToBuffer(void *dest) = 0;
    };

    // Lots of things to think about!
}};
