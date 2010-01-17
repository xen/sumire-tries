#ifndef SUMIRE_INT_TYPES_H
#define SUMIRE_INT_TYPES_H

#include <cstddef>
#include <limits>

namespace sumire {

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64;

typedef std::numeric_limits<UInt8> UInt8Limits;
typedef std::numeric_limits<UInt16> UInt16Limits;
typedef std::numeric_limits<UInt32> UInt32Limits;
typedef std::numeric_limits<UInt64> UInt64Limits;

}  // namespace sumire

#endif  // SUMIRE_INT_TYPES_H
