#ifndef ENQUEUEPREDICATEIF_H
#define ENQUEUEPREDICATEIF_H

namespace seda {
	#include "QueueElementIF.h"

	// Enqueue predicates specify a condition on the enqueue operation.
	// Mechanism allows for load-conditioning policies, i.e. simple
	// thresholding, rate control, credit-based flow control, etc.
	class EnqueuePredicateIF{
	public:
		bool accept(QueueElementIF element);
	}
}

#endif