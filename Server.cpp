/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:46:32 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/12/10 15:05:24 by dcarrilh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::string const &port, std::string const &pass) : _port(port), _pass(pass)
{
    _listening = socket(AF_INET, SOCK_STREAM, 0);
    if (_listening == -1)
    {
        std::cerr << "Can't create a socket! Quitting" << std::endl;
        exit(EXIT_FAILURE);
    }
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(std::stoi(port));
    inet_pton(AF_INET, port.c_str(), &hint.sin_addr);

    if (bind(_listening, (sockaddr *)&hint, sizeof(hint)) == -1)
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