#include "list.h"

void list_append(list_node **head, list_node *n)
{
	if ((*head) && (*head)->last) {
		(*head)->last->next = n;
		(*head)->last = n;
		return;
	}

	list_node **current = head;
	while (*current) {
		current = &(*current)->next;
	}

	*current = n;
	(*head)->last = n;
}
