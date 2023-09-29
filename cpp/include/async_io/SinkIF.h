#ifndef SINKIF_H
#define SINKIF_H

namespace seda {
	#include "QueueElementIF.h"
	#include "EnqueuePredicateIF.h"

	// 'Sink' end of a finite-length event queue:
	// Enqueue operations only.
	class SinkIF{
	public:
		void enqueue(QueueElementIF element);

		// Lossy = drops element if it doesn't meet enqueue conditions.
		bool enqueueLossy(QueueElementIF element);

		// Atomic enqueue of list, aborts if any do not meet enqueue conditions.
		void enqueueMany(QueueElementIF[] elements);

		// Set predicate on enqueue operation.
		void setEnqueuePredicate(EnqueuePredicateIF pred);

		EnqueuePredicateIF getEnqueuePredicate();

		// Returns num elements in sink.
		int size();
	}
}

#endif