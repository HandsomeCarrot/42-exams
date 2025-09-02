#include "vbc.h"

static node	*get_expression(char **s);

static node	*get_factor(char **s)
{
	node	*n;

	n = NULL;
	if (isdigit(**s))
	{
		n = new_value(**s);
		(*s)++;
		return (n);
	}
	else if (**s == '(')
	{
		(*s)++;
		n = get_expression(s);
		if (**s == ')')
		{
			(*s)++;
			return (n);
		}
	}
	destroy_tree(n);
	if (!**s)
		unexpected(**s);
	return (NULL);
}

static node	*get_term(char **s)
{
	node	*left;
	node	*tmp;
	node	*right;

	left = get_factor(s);
	if (!left)
		return (NULL);
	while (**s == '*')
	{
		(*s)++;
		right = get_factor(s);
		if (!right)
			return (destroy_tree(left), NULL);
		tmp = new_node((node){MULTI, 0, left, right});
		if (!tmp)
			return (destroy_tree(left), destroy_tree(right), NULL);
		left = tmp;
	}
	return (left);
}

static node	*get_expression(char **s)
{
	node	*left;
	node	*tmp;
	node	*right;

	left = get_term(s);
	if (!left)
		return (NULL);
	while (**s == '+')
	{
		(*s)++;
		right = get_term(s);
		if (!right)
			return (destroy_tree(left), NULL);
		tmp = new_node((node){ADD, 0, left, right});
		if (!tmp)
			return (destroy_tree(left), destroy_tree(right), NULL);
		left = tmp;
	}
	return (left);
}

node	*parse_expr(char *s)
{
	node	*ret;

	if (!*s)
		return (NULL);
	ret = get_expression(&s);
	if (*s)
	{
		destroy_tree(ret);
		unexpected(*s);
		return (NULL);
	}
	return (ret);
}

int	main(int argc, char **argv)
{
	node	*tree;

	if (argc != 2)
		return (1);
	tree = parse_expr(argv[1]);
	if (!tree)
		return (1);
	printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
}
