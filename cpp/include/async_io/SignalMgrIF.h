#ifndef SIGNALMGRIF_H
#define SIGNALMGRIF_H

namespace seda {
	#include "SignalIF.h"
	#include "SinkIF.h"

	// Delivers systemwide 'signals' to stages.
	class SignalMgrIF{
	public:
		// Signal triggers and delivers object of given type to sink
		void register(SignalIF signalType, SinkIF sink);

		void deregister(SignalIF signalType, SinkIF sink);
	}
}

#endif