#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../Server.hpp"

class Client {
	private:
		std::string _username;
		std::string _nickname;
		std::string _hostname;
		std::string _realName;
		int			_fd;
		bool		_authState;
		int			_socketClient;
		//(send)buffer
	public:


};

#endif
