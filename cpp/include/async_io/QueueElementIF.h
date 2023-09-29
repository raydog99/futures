#ifndef QUEUEELEMENTIF_H
#define QUEUEELEMENTIF_H

namespace seda {
	#include "QueueElementIF.h"
	#include "EnqueuePredicateIF.h"

	// QueueElementIF: event that populates an event queue
	// Empty interface, applications implement instances to
	// represent different events 
	class QueueElementIF{
		// Empty interface
	}
}

#endif