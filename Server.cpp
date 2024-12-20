/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:46:32 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/12/20 12:31:41 by dcarrilh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//Start the server
Server::Server(std::string const &port, std::string const &pass) : _pass(pass)
{
    _port = std::atoi(port.c_str());
    int _opt = 1;
    
    //Create a socket
    _server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket == -1)
    {
        throw std::runtime_error("Can't create a socket! Quitting");        
    }
    
    //Configure the socket for reuse
    if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof(_opt)) == -1)
    {
        close(_server_socket);
        throw std::runtime_error("Can't set socket options! Quitting");
    }
    
    //Configure the server
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET; // Set the address family to IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces
    address.sin_port = htons(_port); // Convert the port to network byte order

    //Bind the socket to IP/port
    if (bind(_server_socket, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        close(_server_socket);
        throw std::runtime_error("Can't bind to IP/port");
    }
    
    //Mark the socket for listening in
    if (listen(_server_socket, SOMAXCONN) == -1)
    {
        close(_server_socket);
        throw std::runtime_error("Can't listen!");
    }

    //Add the server socket to the poll
    struct pollfd _poll_listener;
    _poll_listener.fd = _server_socket;
    _poll_listener.events = POLLIN;
    _poll_fds.push_back(_poll_listener);

    std::cout << "Server is listening on port " << _port << std::endl;
    
}

Server::~Server()
{
    close(_server_socket);
    std::cout << "Server shut down." << std::endl;
}

//Run the server
void Server::run()
{
    while (1)
    {
        int poll_c = poll(_poll_fds.data(), _poll_fds.size(), -1);
        if (poll_c == -1)
        {
            std::cerr << "Poll failed" << std::endl;
            break;
        }

        for (unsigned int i = 0; i < _poll_fds.size(); i++)
        {
            if(_poll_fds[i].revents & POLLIN)
            {
                if (_poll_fds[i].fd == _server_socket)
                {
                    //int new_socket;
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
                    _poll_fds.push_back(_poll_client);

					//add a client to the map
					_clients[_client_socket] = Client();
                }

                else
                {
                    char buffer[1024] = {0};
                    int valread = read(_poll_fds[i].fd, buffer, 1024);
                    if (valread <= 0)
                    {
                        std::cout << "Client disconnected" << std::endl;
                        close(_poll_fds[i].fd);
						_clients.erase(_poll_fds[i].fd);
                        _poll_fds.erase(_poll_fds.begin() + i);
						i--;
                    }
					//message is received
                    else {
						
                        std::string msg(buffer);
                        //std::cout << msg + " ------teste------" << std::endl;
                        command(_poll_fds[i].fd, msg);
                        //_clients[_poll_fds[i].fd].bufferAppend(msg);
                        //send(_poll_fds[i].fd, buffer, valread, 0);

						//The IRC protocol specifies that commands are terminated by \r\n

					
					}
                }
            }
        }
    }
}

void Server::command(int fd, std::string &msg)
{
    _clients[fd].bufferAppend(msg);

    size_t pos;
    while((pos = _clients[fd].buffer().find("\r\n")) != std::string::npos)
    {
        std::string command = _clients[fd].buffer().substr(0, pos);
        _clients[fd].clearBuffer();
        handleClientMsg(fd, command);
    }
}

void Server::handleClientMsg(int fd, std::string &msg)
{
    
    
    if (!_clients[fd].getAutheticated())
    {
        if (msg.rfind("PASS ", 0) == 0)
        {
            std::string pass = msg.substr(5);
            if (pass != _pass )
            {
                std::string error = "Error: Wrong Password!\r\n";
                send(fd, error.c_str(), error.size(), 0);
            }
            else
                _clients[fd].setAuthenticated(true);
        }
        else
        {
            std::string error = "Error: First you need to put Password to authenticate!\r\n";
            send(fd, error.c_str(), error.size(), 0);
        }
    }
    else
    {
        if (msg.rfind("PASS ", 0) == 0)
        {
            std::string error = "Error: You are already authenticated!\r\n";
            send(fd, error.c_str(), error.size(), 0);
        }
        else if (msg.rfind("NICK ", 0) == 0)
        {
            std::string nickname = msg.substr(5);
            _clients[fd].setNickname(nickname);
            std::cout << "Client: " << fd << " set nickname to: " << _clients[fd].getNickname() << std::endl;
        }
        else if (msg.rfind("USER ", 0) == 0)
        {
            if (_clients[fd].getNickname().empty())
            {
                std::string error = "First you need to set nickname!\r\n";
                send(fd, error.c_str(), error.size(), 0);
            }
            else
            {
                std::istringstream iss(msg);
                std::string command;
                iss >> command;
                std::string username, hostname, serverName, realname;
                iss >> username >> hostname >> serverName;
                std::getline(iss, realname);
                _clients[fd].setUsername(username);
                _clients[fd].setHostname(hostname);
                _clients[fd].setRealName(realname);
                std::cout << "Client " << fd << " set USER info." << std::endl;
                std::string welcome = "Welcome to the IRC server, " + _clients[fd].getNickname() + "!\r\n";
                send(fd, welcome.c_str(), welcome.size(), 0);
            }
        }
        else if (msg.rfind("PING", 0) == 0)
        {
            std::string token = msg.substr(5);
            std::string response = "PONG " + token + "\r\n";
            send(fd, response.c_str(), response.length(), 0);
            std::cout << "Responded to PING with PONG." << std::endl;
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