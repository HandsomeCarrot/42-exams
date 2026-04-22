#include "bigint.hpp"
#include <cstddef>
#include <iostream>
#include <iterator>

bigint::bigint(void) : _bigint(1)
{
	std::cerr << "default constructor" << std::endl;
}

bigint::~bigint(void)
{
	std::cerr << "deconstructor" << std::endl;
}

// param constructor, with uint
bigint::bigint(unsigned int value) : _bigint(0)
{
	std::cerr << "parameterized constructor (uint)" << std::endl;

	for (; value > 10; value /= 10)
		_bigint.push_back((value % 10));
	_bigint.push_back(value);
}

bigint::bigint(const bigint & other) : _bigint(other._bigint)
{
	std::cerr << "parameterized constructor (bigint)" << std::endl;
}

bigint::bigint(const t_bigint & other) : _bigint(other)
{
	std::cerr << "parameterized constructor (t_bigint)" << std::endl;
}

bigint & bigint::operator=(const bigint & other)
{
	std::cerr << "assignement operator" << std::endl;

	if (this == &other)
		return *this;

	_bigint.clear();
	_bigint = other._bigint;

	return *this;
}

bigint bigint::operator+(const bigint & right) const
{
	bigint result;

	int carry = 0;
	std::size_t this_size = this->_bigint.size();
	std::size_t right_size = right._bigint.size();

	for (std::size_t i = 0; i < this_size || i < right_size; ++i)
	{
		unsigned int current_digit = carry;

		if (i < this_size)
			current_digit += this->_bigint.at(i);
		if (i < right_size)
			current_digit += right._bigint.at(i);

		carry = current_digit / 10;
		current_digit %= 10;

		if (i == 0)
			result._bigint[0] = current_digit;
		else
			result._bigint.push_back(current_digit);
	}
	return (result);
}

bigint bigint::operator++(int)
{
	return (bigint());
}

bigint & bigint::operator++(void)
{
	return (*this);
}

bigint & bigint::operator+=(const bigint & right)
{
	(void)right;
	return (*this);
}

bool bigint::operator==(const bigint & right) const
{
	(void)right;
	return (false);
}

bool bigint::operator!=(const bigint & right) const
{
	(void)right;
	return (false);
}

bool bigint::operator< (const bigint & right) const
{
	(void)right;
	return (false);
}

bool bigint::operator<=(const bigint & right) const
{
	(void)right;
	return (false);
}

bool bigint::operator>(const bigint & right) const
{
	(void)right;
	return (false);
}

bool bigint::operator>=(const bigint & right) const
{
	(void)right;
	return (false);
}

bigint bigint::operator<<(const bigint & shift) const
{
	(void)shift;
	return (bigint());
}

bigint bigint::operator>>(const bigint & shift) const
{
	(void)shift;
	return (bigint());
}

bigint & bigint::operator<<=(const bigint & shift)
{
	(void)shift;
	return (*this);
}

bigint & bigint::operator>>=(const bigint & shift)
{
	(void)shift;
	return (*this);
}

std::string bigint::getAsString(void) const
{
	std::string str;

	for (std::size_t i = 0; i < _bigint.size(); ++i)
		str.insert(str.begin(), 1, '0' + _bigint[i]);

	return (str);
}

std::ostream & operator<<(std::ostream & os, const bigint & ubint)
{
	os << ubint.getAsString();
	return os;
}