#ifndef flcl_list
#define flcl_list


typedef struct list_node {
	void *data;
	struct list_node *next;
	struct list_node *last;
} list_node;

void list_append(list_node **head, list_node *n);

#endif
