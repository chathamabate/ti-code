#include <cxxutil/core/data.h>

#include <string.h>

using namespace cxxutil::core;

size_t cxxutil::core::strCatSafe(char *dest, size_t destLen, 
        size_t destSize, const char *src) {

    size_t srcLen = strlen(src);

    // Copying will not include null terminator
    // This will be added at the end.
    size_t copyAmt = srcLen;
    if (destLen + copyAmt + 1 > destSize) {
        copyAmt = destSize - destLen - 1;
    }
    
    size_t finalLen = destLen + copyAmt;

    memcpy(&(dest[destLen]), src, copyAmt);

    // Make sure final character is the null term.
    dest[finalLen] = '\0';

    return finalLen;
}
