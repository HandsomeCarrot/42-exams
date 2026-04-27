#include "bigint.hpp"
#include <cstdio>
#include <iostream>

bigint::bigint(void) : _bigint(1)
{
	//std::cerr << "default constructor" << std::endl;

	_bigint[0] = 0;
}

bigint::~bigint(void)
{
	//std::cerr << "deconstructor" << std::endl;
}

// param constructor, with uint
bigint::bigint(unsigned int value) : _bigint(0)
{
	//std::cerr << "parameterized constructor (uint)" << value << std::endl;

	for (; value > 9; value /= 10)
		_bigint.push_back((value % 10));
	_bigint.push_back(value);
}

bigint::bigint(const bigint & other) : _bigint(other._bigint)
{
	//std::cerr << "parameterized constructor (bigint)" << other << std::endl;
}

bigint::bigint(const t_bigint & other) : _bigint(other)
{
	//std::cerr << "parameterized constructor (t_bigint)" << std::endl;
}

bigint & bigint::operator=(const bigint & other)
{
	//std::cerr << "assignment operator" << std::endl;

	if (this == &other)
		return *this;

	_bigint = other._bigint;

	return *this;
}

bigint bigint::operator+(const bigint & right) const
{
	//std::cerr << "addition operator" << std::endl;

	bigint result;
	int carry = 0;
	size_type left_size = this->_bigint.size();
	size_type right_size = right._bigint.size();

	result._bigint.erase(result._bigint.begin());

	for (size_type i = 0; i < left_size || i < right_size; ++i)
	{
		unsigned int current_digit = carry;

		if (i < left_size)
			current_digit += this->_bigint.at(i);
		if (i < right_size)
			current_digit += right._bigint.at(i);

		carry = current_digit / 10;
		current_digit %= 10;

		result._bigint.push_back(current_digit);
	}

	if (carry != 0)
		result._bigint.push_back(carry);

	return (result);
}

bigint & bigint::operator++(void)
{
	//std::cerr << "pre-increment operator" << std::endl;

	*this = *this + 1;

	return (*this);
}

bigint bigint::operator++(int)
{
	//std::cerr << "post-increment operator" << std::endl;

	bigint copy(*this);

	++*this;

	return (copy);
}

bigint & bigint::operator+=(const bigint & right)
{
	//std::cerr << "'+=' operator" << std::endl;

	*this = *this + right;

	return (*this);
}

bool bigint::operator==(const bigint & right) const
{
	//std::cerr << "'==' operator" << std::endl;

	if (this->_bigint.size() != right._bigint.size())
		return (false);

	for (size_type i = 0; i < _bigint.size(); ++i)
	{
		if (this->_bigint.at(i) != right._bigint.at(i))
			return (false);
	}

	return (true);
}

bool bigint::operator!=(const bigint & right) const
{
	//std::cerr << "'!=' operator" << std::endl;

	if (*this == right)
		return (false);
	return (true);
}

bool bigint::operator< (const bigint & right) const
{
	//std::cerr << "'<' operator" << std::endl;

	if (this->_bigint.size() < right._bigint.size())
		return (true);

	if (this->_bigint.size() > right._bigint.size())
		return (false);


	for (size_type i = _bigint.size() - 1; i != 0; --i)
	{
		if (this->_bigint.at(i) < right._bigint.at(i))
			return (true);
		else if (this->_bigint.at(i) > right._bigint.at(i))
			return (false);
	}

	if (this->_bigint.at(0) < right._bigint.at(0))
		return (true);

	return (false);
}

bool bigint::operator<=(const bigint & right) const
{
	//std::cerr << "'<=' operator" << std::endl;

	if (*this < right || *this == right)
		return (true);
	return (false);
}

bool bigint::operator>(const bigint & right) const
{
	//std::cerr << "'>' operator" << std::endl;

	if (*this <= right)
		return (false);
	return (true);
}

bool bigint::operator>=(const bigint & right) const
{
	//std::cerr << "'>=' operator" << std::endl;

	if (*this == right || *this > right)
		return (true);
	return (false);
}

bigint bigint::operator<<(const bigint & shift)
{
	//std::cerr << "'<<' operator" << std::endl;

	bigint modified(*this);

	modified <<= shift;

	return (modified);
}

bigint bigint::operator>>(const bigint & shift)
{
	//std::cerr << "'>>' operator" << std::endl;

	bigint modified(*this);

	modified >>= shift;

	return (modified);
}

bigint & bigint::operator<<=(const bigint & shift)
{
	//std::cerr << "'<<=' operator" << std::endl;

	for (bigint i = 0; i < shift; ++i)
	{
		//std::cout << "i = " << i << std::endl;
		_bigint.insert(_bigint.begin(), 0);
	}

	return (*this);
}

bigint & bigint::operator>>=(const bigint & shift)
{
	//std::cerr << "'>>=' operator" << std::endl;

	for (bigint i = 0; i < shift; ++i)
		this->_bigint.erase(this->_bigint.begin());

	return (*this);
}

std::string bigint::getAsString(void) const
{
	if (_bigint.empty())
		return ("");

	std::string str;

	for (size_type i = 0; i < _bigint.size(); ++i)
		str.insert(str.begin(), 1, '0' + _bigint[i]);

	return (str);
}

std::ostream & operator<<(std::ostream & os, const bigint & object)
{
	os << object.getAsString();
	return os;
}