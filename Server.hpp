/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:35:01 by dcarrilh          #+#    #+#             */
/*   Updated: 2025/01/03 16:27:48 by dcarrilh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netdb.h>
# include <cstdlib>
# include <cstring>
# include <fstream>
# include <sstream>
# include <unistd.h>
# include <vector>
# include <poll.h>
# include <map>
# include "client/client.hpp"
# include "channel/channel.hpp"
# include <algorithm>
# include <csignal>

class Client;
class Channel;

class Server
{
private:
		std::string _pass;
		static int _active;
		int _port;
		int _client_socket;
		int _server_socket;
		std::vector<struct pollfd> _poll_fds;
		struct sockaddr_in address;
		std::map<int, Client> _clients;
		std::map<std::string, Channel> _channels; //map of channel name to channel object 

		//void setupServer(int port);

	public:
		Server(std::string const &port, std::string const &pass);
		~Server();
		void run();
		void handleClientMsg(int fd, std::string &msg);
		void command(int fd, std::string &msg);
		static void signalHandler(int sig);
		void SignalDown();
};

#endif