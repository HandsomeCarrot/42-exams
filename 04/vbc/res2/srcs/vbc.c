#include "../include/vbc.h"

int	main(int argc, char **argv)
{
	node	*tree;

	if (argc != 2)
		return (1);
	tree = parse_add(argv + 1);
	if (!tree)
		return (1);
	printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
}
