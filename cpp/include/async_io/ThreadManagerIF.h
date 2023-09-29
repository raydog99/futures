#ifndef THREADMANAGERIF_H
#define THREADMANAGERIF_H

namespace seda {
	#include "StageWrapperIF.h"

	class ThreadManagerIF {
	public:
		void register(StageWrapperIF stage);

		void deregister(StageWrapperIF stage);

		void deregisterAll();

		// Wake any thread waiting for work.
		void wake();
	}
}

#endif