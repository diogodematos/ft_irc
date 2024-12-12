/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:46:32 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/12/12 15:58:39 by dcarrilh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::string const &port, std::string const &pass) : _pass(pass)
{
    _port = std::atoi(port.c_str());
    int _opt = 1;
    
    //Create a socket
    _listening = socket(AF_INET, SOCK_STREAM, 0);
    if (_listening == -1)
    {
        throw std::runtime_error("Can't create a socket! Quitting");        
    }
    
    //Configure the socket for reuse
    if (setsockopt(_listening, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof(_opt)) == -1)
    {
        close(_listening);
        throw std::runtime_error("Can't set socket options! Quitting");
    }
    
    //Configure the server
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET; // Set the address family to IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces
    address.sin_port = htons(_port); // Convert the port to network byte order

    //Bind the socket to IP/port
    if (bind(_listening, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        close(_listening);
        throw std::runtime_error("Can't bind to IP/port");
    }
    
    //Mark the socket for listening in
    if (listen(_listening, SOMAXCONN) == -1)
    {
        close(_listening);
        throw std::runtime_error("Can't listen!");
    }

    //Add the server socket to the poll
    struct pollfd _poll_listener;
    _poll_listener.fd = _listening;
    _poll_listener.events = POLLIN;
    _poll_listeners.push_back(_poll_listener);

    std::cout << "Server is listening on port " << _port << std::endl;
    
}

Server::~Server()
{
    close(_listening);
    std::cout << "Server shut down." << std::endl;
}