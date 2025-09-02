#ifndef VBC_H
# define VBC_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <ctype.h>
# include <stdbool.h>

//-----GIVEN-----//

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

node    *new_node(node n);
void	destroy_tree(node *n);
void	unexpected(char c);
int		accept(char **s, char c);
int		expect(char **s, char c);
int		eval_tree(node *tree);

//-----helpers.c-----//

int		add_new_value(node *node, char c, bool left);
bool	is_op(char c);
int		is_p(char c);


#endif
