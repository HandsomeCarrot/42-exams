#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

class bigint
{
	public:
		typedef std::vector<unsigned int> t_bigint;

	protected:
		t_bigint _bigint;

	//---------- de/constructors ----------//

	public:
		bigint(void);
		~bigint(void);
		bigint(unsigned int value);
		bigint(const bigint & other);
		bigint & operator=(const bigint & other);

	protected:
		bigint(const t_bigint & other);

	//---------- operators ----------//

	public:
		bigint operator+(const bigint & right) const;

		bigint operator++(int); //post increment i.e. x++
		bigint & operator++(void); //pre increment i.e. ++x

		bigint & operator+=(const bigint & right);

		bool operator==(const bigint & right) const;
		bool operator!=(const bigint & right) const;
		bool operator< (const bigint & right) const;
		bool operator<=(const bigint & right) const;
		bool operator>(const bigint & right) const;
		bool operator>=(const bigint & right) const;

		bigint operator<<(const bigint & shift) const;
		bigint operator>>(const bigint & shift) const;

		bigint & operator<<=(const bigint & shift);
		bigint & operator>>=(const bigint & shift);

	//---------- accessors ----------//

	public:
		std::string getAsString(void) const;
};

// ---------- stream operator ----------//

std::ostream & operator<<(std::ostream & os, const bigint & ubint);

#endif /* BIGINT_HPP */