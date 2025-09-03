#include "vbc.h"

static node	*parse_num(char **s)
{
	node	*ret;
	int		num;

	if (isdigit(**s))
	{
		num = (**s) - '0';
		ret = new_node((node){VAL, num, NULL, NULL});
		(*s)++;
		return (ret);
	}
	else if (expect(s, '('))
	{
		ret = parse_add(s);
		if (ret == NULL)
			return (NULL);
		if (expect(s, ')'))
			return (ret);
	}
	return (NULL);
}

static node	*parse_multi(char **s)
{
	node	*ret;
	node	*tmp;

	ret = parse_num(s);
	if (ret == NULL)
		return (NULL);
	while (accept(s, '*'))
	{
		tmp = parse_num(s);
		if (tmp == NULL)
		{
			destroy_tree(ret);
			return (NULL);
		}
		ret = new_node((node){MULTI, 0, ret, tmp});
	}
	return (ret);
}

node	*parse_add(char **s)
{
	node	*ret;
	node	*tmp;

	ret = parse_multi(s);
	if (ret == NULL)
		return (NULL);
	while (accept(s, '+'))
	{
		tmp = parse_multi(s);
		if (tmp == NULL)
		{
			destroy_tree(ret);
			return (NULL);
		}
		ret = new_node((node){ADD, 0, ret, tmp});
	}
	if (**s)
	{
		destroy_tree(ret);
		return (NULL);
	}
	return (ret);
}
