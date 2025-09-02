#include "vbc.h"

node *parse_expression(char **s)
{
	
}

node *parse_term(char **s)
{}

node *parse_factor(char **s)
{}

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
