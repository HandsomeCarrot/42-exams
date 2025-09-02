#include "vbc.h"

static node	*new_value(char c)
{
	node	*ret;

	ret  = calloc(1, sizeof(node));
	if (!ret)
		return (NULL);
	ret->type = VAL;
	ret->val = ((int)c - (int)'0');
	return (ret);
}

bool	is_op(char c)
{
	if (c == '+' || c == '*')
		return (true);
	return (false);
}

int	is_p(char c)
{
	if (c == '(')
		return (1);
	if (c == ')')
		return (2);
	return (0);
}

