#ifndef PROFILERIF_H
#define PROFILERIF_H

namespace seda {
	#include "ProfilableIF.h"

	class ProfilerIF{
	public:
		bool enabled();

		// Profiler tracks object's size over time.
		void add(String name, ProfilableIF pr);
	}
}

#endif