#pragma once

namespace rpc{
	class server{
	public:
		server(std::string const &addr, uint6_t port);
		~server();
		void run();
		void bind(std::string const &name, std::function func)
		void stop();
	private:
		std::unordered_map<std::string, std::function> dispatchTable;
	}
}