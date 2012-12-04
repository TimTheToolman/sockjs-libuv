#include "queue.h"

// TODO: Use more efficient memory layout
// TODO: use ngx-queue.h?

void queue_init(queue_t* queue) {
	memset(queue, 0, sizeof(queue_t));
}

void queue_add(queue_t* queue, void* value) {
	node_t* node = (node_t*)malloc(sizeof(node));
	node->value = value;
	node->next = NULL;

	if (queue->tail == NULL) {
		queue->head = queue->tail = node;
	} else {
		queue->tail->next = node;
		queue->tail = node;
	}
}

void* queue_pop(queue_t* queue) {
	node_t* node = queue->head;

	if (node == NULL)
		return NULL;

	queue->head = node->next;

	if (queue->tail == node)
		queue->tail = NULL;

	void* data = node->value;
	free(node);
	return data;
}

int queue_is_empty(queue_t* queue) {
	return queue->head == NULL;
}
