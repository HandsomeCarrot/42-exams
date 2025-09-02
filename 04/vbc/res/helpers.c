#include "vbc.h"

static node	*new_value(char c)
{
	node	*ret = calloc(1, sizeof(node));
	if (!ret)
		return (NULL);

	ret->type = VAL;
	ret->val = ((int)c - (int)'0');
	
	return (ret);
}

int	add_new_value(node *node, char c, bool left)
{
	node *tmp = new_value(c);
	if (!tmp)
		return (1);

	node->parent = node;
	if (left)
		node->l = tmp;
	else
		node->r = tmp;

	return (0);
}

bool	is_calc(char c)
{
	if (c == '+' || c == '*')
		return (true);
	return (false);
}

int	is_p(char c)
{
	if (c == '(')
		return (1);
	if (c == ')')
		return (2);
	return (0);
}

bool	valid_next_char(char *s)
{
	char next = s[1];

	if (!s)
		return (true);
	if (isdigit(*s))
		if (is_calc(next) || next == '\0')
			return (true);
	if (is_calc(*s))
		if (isdigit(next) || is_p(next))
			return (true);
	if (is_p(*s))
		if (is_calc(next) || isdigit(next)
			|| is_p(next) || next == '\0')
			return (true);
	unexpected(*s);
	return (false);
}

void	set_calc(node *n, char c, bool *l)
{
	if (c == '+')
		n->type = ADD;
	else
		n->type = MULTI;
	*l = !*l;
}

int	new_depth(node *ret, node *cur, bool left, char c)
{
	node	*new = NULL;
	if (c == '(')
	{
		new = calloc(1, sizeof(node));
		new->parent = cur;
		if (!new)
			return (1);
		if (left)
			cur->l = new;
		else
			cur->r = new;
	}
	else
	{
	}
	return (0);	
}

