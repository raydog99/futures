#ifndef SOURCEIF_H
#define SOURCEIF_H

namespace seda {
	#include "QueueElementIF.h"

	// Source side of event queue.
	// Dequeue operations only.
	class SourceIF {
	public:
		QueueElementIF dequeue();

		QueueElementIF[] dequeueAll();

		// Dequeues specified amount: min(num, all).
		QueueElementIF[] dequeue(int num);

		// Returns number of elements waiting in queue.
		int size();
	}
}

#endif