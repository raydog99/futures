#ifndef STAGEIF_H
#define STAGEIF_H

namespace seda {
	#include "SinkIF.h"
	#include "StageWrapperIF.h"

	class StageIF{
	public:
		std::string getName();

		SinkIF getSink();

		StageWrapperIF getWrapper();

		void destroy();
	}
}

#endif