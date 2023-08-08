#include "cxxutil/core/mem.h"
#include <cstddef>
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

size_t cxxutil::core::multiStrCatSafe(char *dest, size_t destLen, size_t destSize,
        size_t numStrs, const char **strs) {
    size_t currLen = destLen;

    // Only add strings while there are strings to add, and
    // while there is space in the destination.
    for (size_t i = 0; i < numStrs && currLen + 1 < destSize; i++) {
        currLen = strCatSafe(dest, currLen, destSize, strs[i]);
    }

    return currLen;
}

SafeArray<char> *cxxutil::core::strCopySafe(uint8_t memChnl, const char *s) {
    size_t len = strlen(s);
    SafeArray<char> *arr = new SafeArray<char>(memChnl, len + 1);
    strcpy(arr->getArrMut(), s);

    return arr;
}

