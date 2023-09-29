#ifndef MANAGERIF_H
#define MANAGERIF_H

namespace seda {
	#include "StageIF.h"
	#include "SignalMgrIF.h"
	#include "ProfilerIF.h"

	// ManagerIF coordinates stages.
	class ManagerIF{
	public:
		// Stage can have multiple event queues.
		// "main" is the default event queue for a stage,
		// and is the default sink returned on StageIF.getSink().
		const std::string MAINSINK = 'main';

		StageIF getStage(std::string stageName);

		StageIF createStage(std::string stageName, EventHandlerIF eventHandler, std::string initargs[]);

		SignalMgrIF getSignalMgr();

		ProfilerIF getProfiler();

		void destroyStage(std::string stageName);
	}
}

#endif