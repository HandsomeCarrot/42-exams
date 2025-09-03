/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoka <vpoka@student.42vienna.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 20:44:53 by vpoka             #+#    #+#             */
/*   Updated: 2025/09/03 20:44:54 by vpoka            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/vbc.h"

static node	*parse_add(char **s);

static node	*parse_num(char **s)
{
	static int	depth = 0;
	node		*ret;
	int			num;

	if (isdigit(**s))
	{
		num = (**s) - '0';
		ret = new_node((node){VAL, num, NULL, NULL});
		(*s)++;
		return (ret);
	}
	else if (accept(s, '('))
	{
		ret = parse_add(s);
		if (ret == NULL)
			return (NULL);
		if (accept(s, ')'))
			return (ret);
	}
	if (!**s)
		unexpected(0);
	return (NULL);
}

static node	*parse_multi(char **s)
{
	node	*ret;
	node	*tmp;

	ret = parse_num(s);
	if (ret == NULL)
		return (NULL);
	while (accept(s, '*'))
	{
		tmp = parse_num(s);
		if (!tmp)
			return (destroy_tree(ret), NULL);
		ret = new_node((node){MULTI, 0, ret, tmp});
	}
	return (ret);
}

static node	*parse_add(char **s)
{
	node	*ret;
	node	*tmp;

	ret = parse_multi(s);
	if (ret == NULL)
		return (NULL);
	while (accept(s, '+'))
	{
		tmp = parse_multi(s);
		if (!tmp)
			return (destroy_tree(ret), NULL);
		ret = new_node((node){ADD, 0, ret, tmp});
	}
	return (ret);
}

node	*parse(char **s)
{
	node	*ret;

	ret = parse_add(s);
	if (!expect(s, '\0'))
		return (destroy_tree(ret), NULL);
	return (ret);
}
