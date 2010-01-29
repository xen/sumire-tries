#ifndef SUMIRE_SELECT_TABLE_H
#define SUMIRE_SELECT_TABLE_H

#include "int-types.h"

namespace sumire {

class SelectTable
{
public:
	static UInt32 lookup(bool bit, UInt32 count, UInt8 unit);

private:
	// Disallows instantiation.
	SelectTable() {}
	~SelectTable() {}

	// Disallows copies.
	SelectTable(const SelectTable &);
	SelectTable &operator=(const SelectTable &);
};

}  // namespace sumire

#include "select-table-in.h"

#endif  // SUMIRE_SELECT_TABLE_H
