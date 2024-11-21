/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 14:01:03 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/11/21 17:23:32 by dcarrilh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Server.hpp"

// int main(int argc, char **argv)
// {
//     if (argc != 3)
//     {
//         std::cerr << "Error: wrong number of arguments" << std::endl;
//         return (0);
//     }
//     else
//     {
//         server = new Server(argv[1], argv[2]);
//     }      
// }

#include "Server.hpp"
#include <sstream>

int main(int argc, char **argv)
{
    int _listening = socket(AF_INET, SOCK_STREAM, 0);
    std::string _ip = argv[1];
    std::stringstream ss(argv[2]);
    int _port;
    ss >> _port;

    if (_listening == -1)
    {
        std::cerr << "Can't create a socket! Quitting" << std::endl;
        return 1;
    }
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(_port);
    inet_pton(AF_INET, _ip.c_str(), &hint.sin_addr);

    if (bind(_listening, (sockaddr *)&hint, sizeof(hint)) == -1)
    {
        std::cerr << "Can't bind to IP/port" << std::endl;
        return 2;
    }
    
    if (listen(_listening, SOMAXCONN) == -1)
    {
        std::cerr << "Can't listen!" << std::endl;
        return 3;
    }

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    
    int clientSocket = accept(_listening, (sockaddr *)&client, &clientSize);
    
}