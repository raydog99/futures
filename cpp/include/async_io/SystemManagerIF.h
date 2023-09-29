#ifndef SYSTEMMANAGERIF_H
#define SYSTEMMANAGERIF_H

namespace seda {
	#include "ThreadManagerIF.h"
	#include "StageIF.h"
	#include "StageWrapperIF.h"

	class SystemManagerIF {
	public:
		ThreadManagerIF getThreadManager();

		ThreadManagerIF getThreadManager(std::string name);

		void addThreadManager(std::string name, ThreadManagerIF threadmgr);

		StageIF createStage(StageWrapperIF wrapper, bool initialize);
	}
}

#endif