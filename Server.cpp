/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:46:32 by dcarrilh          #+#    #+#             */
/*   Updated: 2025/01/30 14:00:21 by dcarrilh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int Server::_active = 1;

// Start the server
Server::Server(std::string const &port, std::string const &pass) : _pass(pass)
{
	_active = 1;
	_port = std::atoi(port.c_str());
	int _opt = 1;

	// Create a socket
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1)
	{
		throw std::runtime_error("Can't create a socket! Quitting");
	}

	// Configure the socket for reuse
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof(_opt)) == -1)
	{
		close(_server_socket);
		throw std::runtime_error("Can't set socket options! Quitting");
	}

	// Configure the server
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;		  // Set the address family to IPv4
	address.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces
	address.sin_port = htons(_port);	  // Convert the port to network byte order

	// Bind the socket to IP/port
	if (bind(_server_socket, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		close(_server_socket);
		throw std::runtime_error("Can't bind to IP/port");
	}

	// Mark the socket for listening in
	if (listen(_server_socket, SOMAXCONN) == -1)
	{
		close(_server_socket);
		throw std::runtime_error("Can't listen!");
	}

	// Add the server socket to the poll
	struct pollfd _poll_listener;
	_poll_listener.fd = _server_socket;
	_poll_listener.revents = 0;
	_poll_listener.events = POLLIN;
	_poll_fds.push_back(_poll_listener);

	std::cout << "Server is listening on port " << _port << std::endl;
}

Server::~Server()
{
	close(_server_socket);
	std::cout << "Server shut down." << std::endl;
}

// Run the server
void Server::run()
{
	while (_active == 1)
	{
		std::signal(SIGINT, Server::signalHandler);
		std::signal(SIGTSTP, Server::signalHandler);

		// ignore date sent to a closed socket
		struct sigaction sa;
		sa.sa_handler = SIG_IGN;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sigaction(SIGPIPE, &sa, NULL);

		int poll_c = poll(_poll_fds.data(), _poll_fds.size(), -1);

		if (poll_c == 0)
		{
			// Poll timed out
			continue;
		}

		if (poll_c == -1 && !_active)
		{
			std::cerr << "Server shutting down gracefully." << std::endl;
			break;
		}
		if (poll_c == -1)
		{
			std::cerr << "Poll failed" << std::endl;
			break;
		}

		for (unsigned int i = 0; i < _poll_fds.size(); i++)
		{
			if (_poll_fds[i].revents & POLLIN)
			{
				if (_poll_fds[i].fd == _server_socket)
				{
					// int new_socket;
					struct sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);
					if ((_client_socket = accept(_server_socket, (struct sockaddr *)&client_addr, &client_len)) == -1)
					{
						std::cerr << "Accept failed" << std::endl;
						continue;
					}
					std::cout << "New connection from " << _client_socket << std::endl;

					struct pollfd _poll_client;
					_poll_client.fd = _client_socket;
					_poll_client.events = POLLIN;
					_poll_client.revents = 0;
					_poll_fds.push_back(_poll_client);

					// add a client to the map
					_clients[_client_socket] = Client(_client_socket);
				}
				else
				{
					char buffer[1024] = {0};
					int valread = read(_poll_fds[i].fd, buffer, 1024);
					if (valread <= 0)
					{
						std::cout << "Client disconnected" << std::endl;
						// Mandar a todos os channels que o user foi desconectado
						
						for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) 
						{
							if (it->second.hasClient(_poll_fds[i].fd)) 
								it->second.removeClient(_poll_fds[i].fd);
						}
							
						close(_poll_fds[i].fd);
						
						_clients.erase(_poll_fds[i].fd);
						_poll_fds.erase(_poll_fds.begin() + i);
						i--;
					}
					// message is received
					else
					{
						std::string msg(buffer);
						command(_poll_fds[i].fd, msg);
					}
				}
			}
		}
	}
}

void Server::command(int fd, std::string &msg)
{
	_clients[fd].bufferAppend(msg);

	std::vector<std::string> command;
	if (_clients[fd].buffer().find("\r\n") != std::string::npos)
	{
		std::string cmd;
		std::istringstream iss(_clients[fd].buffer());
		while(std::getline(iss, cmd))
		{
			size_t pos = cmd.find_first_of("\r\n");
			if (pos != std::string::npos)
				cmd = cmd.substr(0,pos);
			command.push_back(cmd);
		}
		for (unsigned int i = 0; i < command.size(); i++)
		{
			handleClientMsg(fd, command[i]);
		}
		_clients[fd].clearBuffer();
	}
}

void Server::handleClientMsg(int fd, std::string &msg)
{
	std::istringstream iss(msg);
	std::string command, arg1, arg2, arg3, arg4;
	iss >> command >> arg1 >> arg2 >> arg3 >> arg4;
	
	if (command == "CAP")
	{
		std::string hex = "Hello Hexchat Client!\r\n";
		send(fd, hex.c_str(), hex.size(), 0);
		//send(fd, msg.c_str(), msg.size(), 0);
		
	}
	else if (!_clients[fd].getAutheticated())
	{
		if (command == "PASS")
		{
			if (arg1 != _pass)
			{
				std::string error = "Error: Wrong Password!\r\n";
				send(fd, error.c_str(), error.size(), 0);
			}
			else
			{
				_clients[fd].setAuthenticated(true);
				std::string info = "Access granted!\r\n";
				Channel::sendMsg(fd, info);
			}
		}
		else
		{
			std::string error = "Error: You need to enter the password first!\r\n";
			send(fd, error.c_str(), error.size(), 0);
		}
	}
	else
	{
		if (command == "PASS")
		{
			std::string error = "Error: You are already authenticated!\r\n";
			send(fd, error.c_str(), error.size(), 0);
		}
		else if (command == "NICK")
		{
			_clients[fd].setNickname(arg1);
			std::cout << "Client: " << fd << " set nickname to: " << _clients[fd].getNickname() << std::endl;
		}
		else if (command == "USER")
		{
			if (_clients[fd].getNickname().empty())
			{
				std::string error = "You need to set a nickname first!\r\n";
				send(fd, error.c_str(), error.size(), 0);
			}
			else
			{
				_clients[fd].setUsername(arg1);
				_clients[fd].setHostname(arg2);
				_clients[fd].setRealName(arg4);
				std::cout << "Client " << fd << " set USER info." << std::endl;
				std::string welcome = "Welcome to the IRC server, " + _clients[fd].getNickname() + "!\r\n";
				send(fd, welcome.c_str(), welcome.size(), 0);
			}
		}
		else if (command == "PING")
		{
			std::string token = msg.substr(5);
			std::string response = "PONG " + token + "\r\n";
			send(fd, response.c_str(), response.length(), 0);
			std::cout << "Responded to PING with PONG." << std::endl;
		}
		else if (command == "JOIN")
		{
			// size_t space = msg.find(' ', 5);
			// std::string channelName = msg.substr(6);

			if (_clients[fd].getUsername().empty())
			{
				std::string error = "You need to set a username first!\r\n";
				send(fd, error.c_str(), error.size(), 0);
			}
			else if (arg1.find('#')!= 0)
			{
				std::string error = "Wrong Channel!\r\n";
				send(fd, error.c_str(), error.size(), 0);
			}	
			else
			{
				// if channel does not exist, creates
				if (_channels.find(arg1) == _channels.end())
				{
					std::string creation = "Channel " + arg1 + " created!\r\n";
					send(fd, creation.c_str(), creation.size(), 0);
					_channels[arg1] = Channel(arg1);
					_channels[arg1].addClient(fd, &_clients[fd]); // makes the first client the operator

					std::string topic = "TOPIC " + arg1 + " :Welcome to " + arg1 + "!\r\n";
					_channels[arg1].setTopic("Welcome to " + arg1 + "!\r\n");
					send(fd, topic.c_str(), topic.size(), 0);
				}
				// if channel exist
				else if (_channels[arg1].isInvOnly())
				{
					std::string error = "Channel " + arg1 + " only accepts invites!\r\n";
					send(fd, error.c_str(), error.size(), 0);
				}
				// if channel full
				else if (!_channels[arg1].canAddUsr())
				{
					std::string error = "Channel full!\r\n";
					send(fd, error.c_str(), error.size(), 0);
				}
				else if(_channels[arg1].isKeyProtected())
				{
					
					// std::string key = msg.substr(space + 1) + "\r\n";
					if (!_channels[arg1].compareKey(arg2))
					{
						std::string error = "Wrong Channel key!\r\n";
						send(fd, error.c_str(), error.size(), 0);
					}
					else
					{
						_channels[arg1].addClient(fd, &_clients[fd]);
						std::string response = "Joined Channel " + arg1 + "\r\n";
						send(fd, response.c_str(), response.size(), 0);
						std::cout << "Client " << _clients[fd].getNickname() << " joined channel " << arg1 << "." << std::endl;
					}
				}
				else
				{
					_channels[arg1].addClient(fd, &_clients[fd]);
					std::string response = "Joined Channel " + arg1 + "\r\n";
					send(fd, response.c_str(), response.size(), 0);
					std::cout << "Client " << _clients[fd].getNickname() << " joined channel " << arg1 << "." << std::endl;
				}
			}
		}
		else if (command == "PART")
		{
			// std::string channelName = msg.substr(5);
			if (_channels.find(arg1) != _channels.end() && _channels[arg1].hasClient(fd))
			{
				_channels[arg1].removeClient(fd);

				if (_channels[arg1].getClients().empty())
				{
					_channels.erase(arg1);
				}
				std::string response = "Left channel " + arg1 + "\r\n";
				send(fd, response.c_str(), response.size(), 0);
				std::cout << "Client " << fd << " left channel " << arg1 << "." << std::endl;
			}
			else
			{
				std::string error = "Error: You are not in channel " + arg1 + "\r\n";
				send(fd, error.c_str(), error.size(), 0);
			}
		}
		else if (command == "PRIVMSG")
		{
			size_t spacePos = msg.find(' ', 8);
			if (arg1.empty())
			{
				std::string error = "Error: Invalid PRIVMSG format!\r\n";
				send(fd, error.c_str(), error.size(), 0);
			}
			else
			{
				// std::string target = msg.substr(8, spacePos - 8);
				std::string message = msg.substr(spacePos + 1) + "\r\n";
				int check = 0;
				// send msg to specific user
				for (unsigned int i = 0; i < _clients.size(); i++)
				{
					if (_clients[i].getNickname() == arg1)
					{
						std::string pMessage = ":" + _clients[fd].getNickname() + " PRIVMSG " + arg1 + " :" + message + "\r\n";
						send(i, pMessage.c_str(), pMessage.size(), 0);
						check = 1;
						break;
					}
				}
				if (check == 0)
				{
					if (!_channels[arg1].hasClient(fd))
					{
						std::string error = "Error: You are not in channel " + arg1 + "\r\n";
						send(fd, error.c_str(), error.size(), 0);
					}
					// send msg to a channel
					else if (_channels.find(arg1) != _channels.end())
					{
						
        				std::string chanMessage = ":" + _clients[fd].getNickname() + " PRIVMSG " + arg1 + " " + message + "\r\n";
						_channels[arg1].broadcastMsg(chanMessage, fd);
					}
					else
					{
						std::string error = "Error: Channel not found\r\n";
						send(fd, error.c_str(), error.size(), 0);
					}
				}
			}
		}

		else if (msg.rfind("KICK ", 0) == 0 || msg.rfind("TOPIC ", 0) == 0 || msg.rfind("MODE ", 0) == 0 || msg.rfind("INVITE ", 0) == 0)
{
			if (command == "MODE" && arg1 == "#42" && arg2.size() == 0)
			{
				int i;
				i = 0;
			}
			else if (command == "INVITE")
			{
				if (_channels.find(arg2) != _channels.end())
				{
					if (_channels[arg2].isOperator(fd) || _channels[arg2].isOwner(fd))
					{
						for (unsigned int i = 0; i < _clients.size(); i++)
						{
							if (_clients[i].getNickname() == arg1)
							{
								if (_clients[i].getUsername().empty())
								{
									std::string error = arg1 + " needs to set a username first!\r\n";
									send(fd, error.c_str(), error.size(), 0);
									break;
								}
								else
								{
									_channels[arg2].inviteClient(fd, &_clients[i]);
									if (!_channels[arg2].parseMessage(msg, fd))
									Channel::sendMsg(fd, "Error: Command not found!\r\n");
									break;
								}
							}
							else if (i == _clients.size()-1)
							{
								std::string error = "Error: Client Invited doesn't exist!\r\n";
								send(fd, error.c_str(), error.size(), 0);
							}
						}
							
					}
					else
						Channel::sendMsg(fd, "Error: you don't have permission to invite someone.\r\n");
				}
				else
					Channel::sendMsg(fd, "Error: channel doesn't exist!\r\n");
			}
			else if (_channels.find(arg1) != _channels.end())
			{
				if (_channels[arg1].isOperator(fd) || _channels[arg1].isOwner(fd))
				{
					if (!_channels[arg1].parseMessage(msg, fd))
						Channel::sendMsg(fd, "Error: Command not found!\r\n");
				}
				else
				{
					std::string error = "Error: Client isn't Channel Operator\r\n";
					send(fd, error.c_str(), error.size(), 0);
				}
			}
			else
			{
				std::string error = command + "\n" + arg1 + "\n" + arg2 + "\n" + arg3 + "\nError: Channel not found\r\n";
				send(fd, error.c_str(), error.size(), 0);
			}
		}
		else if (command == "WHO")
		{
			int i;
			i = 0;
		}
		else	
		{
			std::ostringstream oss;
			oss << "Unknown command from client " << fd << ": " << msg << "\r\n";
			std::string error = oss.str();
			send(fd, error.c_str(), error.size(), 0);
		}
	}
}

void Server::signalHandler(int sig)
{
	if (sig == SIGINT)
	{
		_active = 0;
		std::cout << "\nCrtl + C called\r" << std::endl;
	}
}