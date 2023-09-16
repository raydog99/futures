#pragma once

namespace rpc{
	class client{
	public:
		client(std::string const &addr, uint16_t port);
		~client();

		template <typename... Args>
		void call(const std::string& funcName, Args... args);
	};
}