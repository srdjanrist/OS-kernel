#include "queue.h"


Queue::Queue() : head(0), tail(0) {}

Queue::~Queue(){
	Elem* last = 0;
	while (head){
		last = head;
		head = head->next;
		delete last;
	}
	tail = head;
}

void Queue::put(PCB* pcb){
	tail = (tail ? tail->next : head) = new Elem (pcb);
}

PCB* Queue::take(){
	if (!head) return 0;
	Elem* temp = head;
	PCB* ret = head->pcb;
	head = head->next;
	if (!head) tail = head;
	delete temp;
	return ret;
}

void Queue::remove (PCB* pcb){
	Elem* last = 0;

	Elem* tek = head;
	while (tek && tek->pcb != pcb) {
		last = tek;
		tek = tek->next;
	}
	if (!tek) return;
	if (tek == head){
		if (tail == head) tail = 0;
		head = head->next;
		delete tek;
		return;
	}
	if (tek == tail){
		last->next = 0;
		tail = last;
		delete tek;
		return;
	}
	last->next = tek->next;
	delete tek;
	return;

}
