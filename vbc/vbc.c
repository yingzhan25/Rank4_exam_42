#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct node
{
	enum {ADD, MULTI, VAL} type;
	int			val;
	struct node		*l;
	struct node		*r;
} node;

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

node    *parse_add(char **s);

node	*parse_val(char **s)
{
	node	*n;
	
	if (isdigit(**s))
	{
		node new = {.type=VAL, .val=**s - '0', .l=NULL, .r=NULL};
		(*s)++;
		n = new_node(new);
		if (!n)
			return (NULL);
		return (n);
	}
	else if (accept(s, '('))
	{
		n = parse_add(s);
		if (!n)
			return (NULL);
		if (!expect(s, ')'))
		{
			destroy_tree(n);
			return (NULL);
		}
		return (n);
	}
	unexpected(**s);
	return (NULL);
}

node	*parse_multi(char **s)
{
	node	*left;
	node	*right;
	node	*n;

	left = parse_val(s);
	if (!left)
		return (NULL);
	while (accept(s, '*'))
	{
		right = parse_val(s);
		if (!right)
		{
			destroy_tree(left);
			return (NULL);
		}
		node new = {.type=MULTI, .l=left, .r=right};
		n = new_node(new);
		if (!n)
		{
			destroy_tree(left);
			destroy_tree(right);
			return (NULL);
		}
		left = n;
	}
	return (left);
}

node	*parse_add(char **s)
{
	node	*left;
	node	*right;
	node	*n;

	left = parse_multi(s);
	if (!left)
		return (NULL);
	while (accept(s, '+'))
	{
		right = parse_multi(s);
		if (!right)
		{
			destroy_tree(left);
			return (NULL);
		}
		node new = {.type=ADD, .l=left, .r=right};
		n = new_node(new);
		if (!n)
		{
			destroy_tree(left);
			destroy_tree(right);
			return (NULL);
		}
		left = n;
	}
	return (left);
}

node	*parse_expr(char *s)
{
	node	*ret;

	ret = parse_add(&s);
	if (!ret)
		return (NULL);
	if (*s)
	{
		unexpected(*s);
		destroy_tree(ret);
		return (NULL);
	}
	return (ret);
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
	return (0);
}

int	main(int ac, char *av[])
{
	node *tree;

	if (ac != 2)
		return (1);
	tree = parse_expr(av[1]);
	if (!tree)
		return (1);
	printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
}

