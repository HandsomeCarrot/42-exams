#include "../include/vbc.h"

static node	*parse_add(char **s);

static node	*parse_num(char **s)
{
	static int depth = 0;
	node	*ret;
	int		num;

	if (isdigit(**s))
	{
		num = (**s) - '0';
		if (DEBUG == 1)
			printf("got this number: %d\n", num);
		ret = new_node((node){VAL, num, NULL, NULL});
		(*s)++;
		return (ret);
	}
	else if (accept(s, '('))
	{
		if (DEBUG == 1)
			printf("went DEEPER. Now at: %d\n", ++depth);
		ret = parse_add(s);
		if (ret == NULL)
			return (NULL);
		if (accept(s, ')'))
		{
			if (DEBUG == 1)
				printf("went BACK. Now at: %d\n", --depth);
			return (ret);
		}
	}
	if (DEBUG == 1)
		printf("failed at parse_num\n");
	if (!**s)
		unexpected(0);
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
		if (DEBUG == 1)
			printf("got multiplication\n");
		tmp = parse_num(s);
		if (!tmp)
			return (destroy_tree(ret), NULL);
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
		if (DEBUG == 1)
			printf("got addition\n");
		tmp = parse_multi(s);
		if (!tmp)
			return (destroy_tree(ret), NULL);
		ret = new_node((node){ADD, 0, ret, tmp});
	}
	return (ret);
}

node	*parse(char **s)
{
	node	*ret;

	ret = parse_add(s);
	if (!expect(s, '\0'))
	{
		if (DEBUG == 1)
			printf("failed at parse\n");
		return (destroy_tree(ret), NULL);
	}
	return (ret);
}
