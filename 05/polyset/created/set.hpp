#ifndef SET_HPP
#define SET_HPP

#include "searchable_bag.hpp"

class set : public searchable_bag
{
	protected:
		searchable_bag * _bag;

	public:
		set(searchable_bag & bag) : _bag(&bag) {};

		void insert (int value);
		void insert (int * array, int size);
		void print() const;
		void clear();

		bool has(int value) const;

		searchable_bag & get_bag(void);
};

#endif