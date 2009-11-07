#include <sumire/object-clipper.h>
#include <sumire/object-reader.h>
#include <sumire/object-writer.h>

#include <cassert>
#include <cstring>
#include <sstream>

#include <iostream>

int main()
{
	std::stringstream stream;

	sumire::ObjectReader reader(&stream);
	sumire::ObjectWriter writer(&stream);

	assert(writer.write(static_cast<int>(100)));
	assert(writer.write(static_cast<double>(1.23)));
	assert(writer.write("apple", 1));
	assert(writer.write("orange", 6));

	int int_value;
	assert(reader.read(&int_value));
	assert(int_value == 100);

	double double_value;
	assert(reader.read(&double_value));
	assert(double_value == 1.23);

	char buf[16] = "";
	reader.read(buf);
	assert(buf[0] == 'a');
	reader.read(buf, 6);
	assert(std::strcmp(buf, "orange") == 0);

	std::string data = stream.str();
	sumire::ObjectClipper clipper(&data[0]);

	int_value = *clipper.clip<int>();
	assert(int_value == 100);

	double_value = *clipper.clip<double>();
	assert(double_value == 1.23);

	buf[0] = *clipper.clip<char>();
	assert(buf[0] == 'a');

	const char *str = clipper.clip<char>(6);
	assert(std::strcmp(str, "orange") == 0);

	return 0;
}
