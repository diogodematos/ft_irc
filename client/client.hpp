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
		Client();
		Client(int fd);
		~Client();

		//getters
		int					getFd() const;
		const std::string	getUsername() const;
		const std::string	getNickname() const;
		const std::string	getHostname() const;
		const std::string	getRealName() const;
		const std::string	buffer() const;
		bool				getAutheticated() const;

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
		bool isReadyToRegister() const;
};

#endif
