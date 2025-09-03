#ifndef VBC_H
# define VBC_H

# ifndef DEBUG
#  define DEBUG 0
# endif

# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

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

//-----given.c-----//

node			*new_node(node n);
void			destroy_tree(node *n);
void			unexpected(char c);
int				accept(char **s, char c);
int				expect(char **s, char c);
int				eval_tree(node *tree);

//-----parsing.c-----//

node			*parse(char **s);

#endif
