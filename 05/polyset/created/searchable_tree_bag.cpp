#include "searchable_tree_bag.hpp"
#include "tree_bag.hpp"

searchable_tree_bag & searchable_tree_bag::operator=(const searchable_tree_bag & other)
{
	if (this != &other)
		tree_bag::operator=(other);
	return *this;
}

bool searchable_tree_bag::has(int value) const
{
	node * n = this->tree;
	while (n)
	{
		if (value < n->value)
			n = n->l;
		else if (value > n->value)
			n = n->r;
		else
			return true;
	}
	return false;
}