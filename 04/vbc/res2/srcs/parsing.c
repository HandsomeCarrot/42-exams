#include "../include/vbc.h"

static node	*parse_num(char **s)
{
	static int depth = 0;
	node	*ret;
	int		num;

	if (isdigit(**s))
	{
		num = (**s) - '0';
		printf("got this number: %d\n", num);
		ret = new_node((node){VAL, num, NULL, NULL});
		(*s)++;
		return (ret);
	}
	else if (expect(s, '('))
	{
		printf("went DEEPER. Now at: %d\n", ++depth);
		ret = parse_add(s);
		if (ret == NULL)
			return (NULL);
		if (expect(s, ')'))
		{
			printf("went BACK. Now at: %d\n", --depth);
			return (ret);
		}
	}
	printf("failed at parse_num\n");
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
		printf("got multiplication\n");
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

static node	*parse_add(char **s)
{
	node	*ret;
	node	*tmp;

	ret = parse_multi(s);
	if (ret == NULL)
		return (NULL);
	while (accept(s, '+'))
	{
		printf("got addition\n");
		tmp = parse_multi(s);
		if (tmp == NULL)
		{
			destroy_tree(ret);
			return (NULL);
		}
		ret = new_node((node){ADD, 0, ret, tmp});
	}
	return (ret);
}

node	*parse(char **s)
{
	node	*ret;

	ret = parse_add(s);
	if (**s)
	{
		printf("failed at parse\n");
		unexpected(**s);
		destroy_tree(ret);
		return (NULL);
	}
	return (ret);
}
