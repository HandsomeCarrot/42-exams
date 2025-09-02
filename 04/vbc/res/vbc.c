#include "vbc.h"

static node *parse_expression(char **s)
{
	node	*left;
	node	*tmp;
	node	*right;

	left = parse_term(s);
	if (!left)
		return (NULL);
	while(**s == '+')
	{
		(*s)++;
		right = parse_term(s);
		if (!right)
			return (destroy_tree(left), NULL);
		tmp = new_node((node){ADD, 0, left, right});
		if (!tmp)
			return (destroy_tree(left), destroy_tree(right), NULL);
		left = tmp;
	}
	return (left);
}

static node *parse_term(char **s)
{
	node	*left;
	node	*tmp;
	node	*right;

	left = parse_factor(s);
	if (!left)
		return (NULL);
	while(**s == '*')
	{
		(*s)++;
		right = parse_factor(s);
		if (!right)
			return (destroy_tree(left), NULL);
		tmp = new_node((node){MULTI, 0, left, right});
		if (!tmp)
			return (destroy_tree(left), destroy_tree(right), NULL);
	}
	return (left);
}

static node *parse_factor(char **s)
{
	node	*left = NULL;
	node	*right;

	if (is_p(**s))
	{}
	else if (isdigit(**s))
	{}
	else
		return (left);

}

node	*parse_expr(char *s)
{
	char	*ptr;
	node	*ret;

	if (!*s)
		return (NULL);
	ptr = s;
	ret = parse_expression(&ptr);
	if (*s)
	{
		destroy_tree(ret);
		unexpected(*s);
		return (NULL);
	}
	return (ret);
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return (1);
	node *tree = parse_expr(argv[1]);
	if (!tree)
		return (1);
	printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
}
