#include "bigint.hpp"
#include <cstdio>
#include <iostream>

bigint::bigint(void) : _bigint(0)
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

	for (; value > 9; value /= 10)
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
	size_type left_size = this->_bigint.size();
	size_type right_size = right._bigint.size();

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
	return (result);
}

bigint bigint::operator++(int)
{
	bigint copy(*this);

	*this = *this + 1;

	return (copy);
}

bigint & bigint::operator++(void)
{
	*this = *this + 1;

	return (*this);
}

bigint & bigint::operator+=(const bigint & right)
{
	*this = *this + right;

	return (*this);
}

bool bigint::operator==(const bigint & right) const
{
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
	if (this->_bigint.size() != right._bigint.size())
		return (true);

	for (size_type i = 0; i < _bigint.size(); ++i)
	{
		if (this->_bigint.at(i) != right._bigint.at(i))
			return (true);
	}

	return (false);
}

bool bigint::operator< (const bigint & right) const
{
	if (this->_bigint.size() < right._bigint.size())
		return (true);

	if (this->_bigint.size() > right._bigint.size())
		return (false);

	for (size_type i = _bigint.size(); i != 0; --i)
	{
		if (this->_bigint.at(i) < right._bigint.at(i))
			return (true);
		else if (this->_bigint.at(i) < right._bigint.at(i))
			return (false);
	}

	return (false);
}

bool bigint::operator<=(const bigint & right) const
{
	if (this->_bigint.size() < right._bigint.size())
		return (true);

	if (this->_bigint.size() > right._bigint.size())
		return (false);

	for (size_type i = _bigint.size(); i != 0; --i)
	{
		if (this->_bigint.at(i) < right._bigint.at(i))
			return (true);
		else if (this->_bigint.at(i) < right._bigint.at(i))
			return (false);
	}

	return (true);
}

bool bigint::operator>(const bigint & right) const
{
	if (this->_bigint.size() < right._bigint.size())
		return (false);

	if (this->_bigint.size() > right._bigint.size())
		return (true);

	for (size_type i = _bigint.size(); i != 0; --i)
	{
		if (this->_bigint.at(i) < right._bigint.at(i))
			return (false);
		else if (this->_bigint.at(i) < right._bigint.at(i))
			return (true);
	}

	return (false);
}

bool bigint::operator>=(const bigint & right) const
{
	if (this->_bigint.size() < right._bigint.size())
		return (false);

	if (this->_bigint.size() > right._bigint.size())
		return (true);

	for (size_type i = _bigint.size(); i != 0; --i)
	{
		if (this->_bigint.at(i) < right._bigint.at(i))
			return (false);
		else if (this->_bigint.at(i) < right._bigint.at(i))
			return (true);
	}

	return (true);
}

bigint bigint::operator<<(const bigint & shift) const
{
	for (bigint i = 0; i < shift; ++i)
		this->_bigint.push_front(0);
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