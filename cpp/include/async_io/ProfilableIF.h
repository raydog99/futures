#ifndef PROFILABLEIF_H
#define PROFILABLEIF_H

namespace seda {
	// An object that implements ProfilableIF can be profiled by
	// a ProfilerIF. Object has measure of size (queue length, list
	// length, amount of memory). Primarily for profiling event queue lists.
	class ProfilableIF{
	public:
		// Returns size of object being profiled.
		int profileSize();
	}
}

#endif