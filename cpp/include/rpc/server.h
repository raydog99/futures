#pragma once

/*
C++ dispatch on HOF

template<class F, F f> 
struct wrapper_impl;

template<class R, class... Args, R(*f)(Args...)>
struct wrapper_impl<R(*)(Args...), f> {
    static R wrap(Args... args) {
        return f(args...);
    }
};

*/

namespace rpc{
	class server{
	public:
		server(std::string const &addr, uint6_t port);
		~server();
		void run();
		void bind(std::string const &name, std::function func)
		void stop();
	private:
		std::unordered_map<std::string, std::any> dispatchTable;
	}
}