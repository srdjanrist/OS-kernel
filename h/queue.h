#ifndef _QUEUE_H_
#define _QUEUE_H_


class PCB;

class Queue{

protected:
	struct Elem {
		PCB* pcb;
		Elem* next;

		Elem (PCB* pcb, Elem* next = 0){
			this->pcb = pcb;
			this->next = next;
		}
	};

	Elem *head, *tail;

public:
	Queue ();
	virtual ~Queue();

	virtual void put(PCB*);
	PCB* take();
	void remove (PCB*);

};

#endif
