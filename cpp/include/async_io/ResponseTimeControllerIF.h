#ifndef RESPONSETIMECONTROLLERIF_H
#define RESPONSETIMECONTROLLERIF_H

namespace seda {
	#include "QueueElementIF.h"

	class ResponseTimeControllerIF {
	public:
		// Set response time target in milliseconds.
		void setTarget(double RTtarget);

		double getTarget();

		void adjustThreshold(QueueElementIF fetched[], long serviceTime);

		void enable();

		void disable();
	}
}

#endif