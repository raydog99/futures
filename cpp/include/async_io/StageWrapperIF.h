#ifndef STAGEWRAPPERIF_H
#define STAGEWRAPPERIF_H

namespace seda {
	#include "StageIF.h"
	#include "EventHandlerIF.h"
	#include "SourceIF.h"
	#include "ThreadManagerIF.h"
	#include "ResourceTimeControllerIF.h"

	class StageWrapperIF {
	public:
		StageIF getStage();

		EventHandlerIF getEventHandler();

		SourceIF getSource();

		ThreadManagerIF getThreadManager();

		ResourceTimeControllerIF getResponseTimeController();

		void init();
	}
}

#endif