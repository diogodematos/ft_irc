/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:35:01 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/12/12 18:43:02 by dcarrilh         ###   ########.fr       */
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

class Server
{
    private:
        
        int _port;
        std::string _pass;
        int _server_socket;
        int _client_socket;
        std::vector<struct pollfd> _poll_fds;
        struct sockaddr_in address;
        

        //void setupServer(int port);
    
    public:
        Server(std::string const &port, std::string const &pass);
        ~Server();
        void run();
    
};

#endif