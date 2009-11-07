#include <sumire/int-types.h>

#include <cassert>
#include <iostream>

int main()
{
	assert(sizeof(sumire::UInt8) == 1);
	assert(sizeof(sumire::UInt16) == 2);
	assert(sizeof(sumire::UInt32) == 4);
	assert(sizeof(sumire::UInt64) == 8);

	assert(sumire::UInt8Limits::min() == 0);
	assert(sumire::UInt16Limits::min() == 0);
	assert(sumire::UInt32Limits::min() == 0);
	assert(sumire::UInt64Limits::min() == 0);

	assert(sumire::UInt8Limits::max() == static_cast<sumire::UInt8>(-1));
	assert(sumire::UInt16Limits::max() == static_cast<sumire::UInt16>(-1));
	assert(sumire::UInt32Limits::max() == static_cast<sumire::UInt32>(-1));
	assert(sumire::UInt64Limits::max() == static_cast<sumire::UInt64>(-1));

	return 0;
}
