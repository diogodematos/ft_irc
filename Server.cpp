/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:46:32 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/12/12 18:45:52 by dcarrilh         ###   ########.fr       */
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
    /*struct pollfd _poll_listener;
    _poll_listener.fd = _server_socket;
    _poll_listener.events = POLLIN;
    _poll_fds.push_back(_poll_listener);*/

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
        _client_socket = accept(_server_socket, NULL, NULL);

    char buffer[1024] = {0};
    recv(_client_socket, buffer, sizeof(buffer), 0);
    std::cout << "Mensagem do cliente: " << buffer << std::endl;
    }
}