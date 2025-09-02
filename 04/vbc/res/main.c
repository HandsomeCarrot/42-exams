#include "vbc.h"

node    *parse_expr(char *s)
{
	node	*ret = NULL;
	node	*cur = ret;
	bool	left = true;

	ret = calloc(1, sizeof(node));
	if (!ret)
		return (NULL);
	while (*s)
	{
		if (!valid_next_char(s))
			break;
		if (isdigit(*s) && add_new_value(cur, *s, left))
			break;
		else if (is_calc(*s) && set_calc(cur, *s, *left))
			break;
		else if (is_p(*s) && new_depth(ret, cur, left, *s))
			break;
		else
			break;
		s++;
	}
    if (*s) 
    {
        destroy_tree(ret);
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
