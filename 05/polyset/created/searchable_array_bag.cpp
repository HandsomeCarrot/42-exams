#include "searchable_array_bag.hpp"
#include "array_bag.hpp"

searchable_array_bag & searchable_array_bag::operator=(const searchable_array_bag & other)
{
	if (this != &other)
		array_bag::operator=(other);
	return *this;
}

bool searchable_array_bag::has(int value) const
{
	for (int i = 0; i < this->size; ++i)
	{
		if (value == this->data[i])
			return true;
	}
	return false;
}