#include "bigint.hpp"
#include <climits>
#include <iostream>

int main(void)
{
	std::cout << "====================" << std::endl;
	bigint b1(UINT_MAX);
	std::cout << UINT_MAX << " ?= " << b1 << std::endl;

	std::cout << "--------------------" << std::endl;
	bigint b2(b1 + 1);
	std::cout << b1 << " ?= " << b2 << std::endl;

	std::cout << "--------------------" << std::endl;
	bigint b3 = 0;
	bigint b3_1 = b3 + 10;
	bigint b3_2 = b3_1 + 11;
	bigint b3_3 = b3_2 + 123456789;

	std::cout << b3 << " | " << b3_1 << " | " << b3_2 << " | " << b3_3 << std::endl;

	std::cout << "====================" << std::endl;
}