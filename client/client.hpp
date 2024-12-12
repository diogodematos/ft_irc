#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../Server.hpp"

class Client {
	private:
		std::string _username;
		std::string _nickname;
		std::string _hostname;
		std::string _realName;
		std::string	_buffer;
		int			_fd;
		bool		_authState;
		int			_socketClient;

	public:
		Client(int fd);
		~Client();

		//getters
		int					getFd() const;
		const std::string	getUsername();
		const std::string	getNickname();
		const std::string	getHostname();
		const std::string	getRealName();
		const std::string	buffer();

		//setters
		void setUsername(const std::string &username);
		void setNickname(const std::string &username);
		void setHostname(const std::string &username);
		void setRealName(const std::string &username);
		void setAuthenticated(bool state);

		//buffer management
		void bufferAppend(const std::string &data);
		void clearBuffer();

		//UTILS
		bool readyToRegist() const;
};

#endif
