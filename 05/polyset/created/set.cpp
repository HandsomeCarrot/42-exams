#include "set.hpp"

void set::insert(int value)
{
	if (!has(value))
		_bag->insert(value);
}

void set::insert(int * array, int size)
{
	for(int i = 0; i < size; ++i)
		insert(array[i]);
}

void set::print(void) const
{
	_bag->print();
}

bool set::has(int value) const
{
	return _bag->has(value);
}

void set::clear(void)
{
	_bag->clear();
}

searchable_bag & set::get_bag(void)
{
	return *_bag;
}