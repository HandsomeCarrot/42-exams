/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   given.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoka <vpoka@student.42vienna.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 20:45:00 by vpoka             #+#    #+#             */
/*   Updated: 2025/09/05 13:10:47 by vpoka            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/vbc.h"

node	*new_node(node n)
{
	node	*ret;

	ret = calloc(1, sizeof(n));
	if (!ret)
		return (NULL);
	*ret = n;
	return (ret);
}

void	destroy_tree(node *n)
{
	if (!n)
		return ;
	if (n->type != VAL)
	{
		destroy_tree(n->l);
		destroy_tree(n->r);
	}
	free(n);
}

void	unexpected(char c)
{
	if (c)
		printf("Unexpected token '%c'\n", c);
	else
		printf("Unexpected end of input\n");
}

int	accept(char **s, char c)
{
	if (**s == c)
	{
		(*s)++;
		return (1);
	}
	return (0);
}

int	expect(char **s, char c)
{
	if (accept(s, c))
		return (1);
	unexpected(**s);
	return (0);
}

int	eval_tree(node *tree)
{
	switch (tree->type)
	{
	case ADD:
		return (eval_tree(tree->l) + eval_tree(tree->r));
	case MULTI:
		return (eval_tree(tree->l) * eval_tree(tree->r));
	case VAL:
		return (tree->val);
	}
}
