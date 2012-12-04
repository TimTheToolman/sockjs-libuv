#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct node_s {
	void* data;
	struct node_s* next;
} node_t;

typedef struct {
	node_t* start;
	node_t* end;
} queue_t;

void queue_init(queue_t* queue);
void queue_add(queue_t* queue, void* value);
void* queue_pop(queue_t* queue);
int queue_is_empty(queue_t* queue);

#endif