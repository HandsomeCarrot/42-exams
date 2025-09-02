#ifndef VBC_H
# define VBC_H

# include <ctype.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

//-----GIVEN-----//

typedef struct node
{
	enum
	{
		ADD,
		MULTI,
		VAL
	} type;
	int			val;
	struct node	*l;
	struct node	*r;
}				node;

node			*new_node(node n);
void			destroy_tree(node *n);
void			unexpected(char c);
int				eval_tree(node *tree);

//-----helpers.c-----//

node			*new_value(char c);
bool			is_op(char c);
int				is_p(char c);

#endif
