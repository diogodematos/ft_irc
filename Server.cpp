/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:46:32 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/11/21 16:58:09 by dcarrilh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::string const &ip, std::string const &port) : _ip(ip), _port(port)
{
    _listening = socket(AF_INET, SOCK_STREAM, 0);
    if (_listening == -1)
    {
        std::cerr << "Can't create a socket! Quitting" << std::endl;
        exit(1);
    }
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(std::stoi(port));
    inet_pton(AF_INET, ip.c_str(), &hint.sin_addr);

    if (bind(_listening, (sockaddr *)&hint, sizeof(hint)) == -1)
    {
        std::cerr << "Can't bind to IP/port" << std::endl;
        exit(2);
    }
    
    if (listen(_listening, SOMAXCONN) == -1)
    {
        std::cerr << "Can't listen!" << std::endl;
        exit(3);
    }
    
    
}