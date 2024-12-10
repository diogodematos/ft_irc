/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:46:32 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/12/10 19:22:39 by dcarrilh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::string const &port, std::string const &pass) : _pass(pass)
{
    int _port = std::atoi(port.c_str());
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
        //close(_listening);
        throw std::runtime_error("Can't set socket options! Quitting");
    }
    
    //Configure the server
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET; // Set the address family to IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces
    inet_pton(AF_INET, port.c_str(), &address.sin_addr); // Convert the port to network byte order

    if (bind(_listening, (sockaddr *)&address, sizeof(address)) == -1)
    {
        std::cerr << "Can't bind to IP/port" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    if (listen(_listening, SOMAXCONN) == -1)
    {
        std::cerr << "Can't listen!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    
}

Server::~Server()
{
    close(_listening);
    std::cout << "Server shut down." << std::endl;
}