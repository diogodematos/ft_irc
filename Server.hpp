/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:35:01 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/12/12 15:10:53 by dcarrilh         ###   ########.fr       */
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
        int _listening;
        std::vector<struct pollfd> _poll_listeners;
        struct sockaddr_in address;
        

        //void setupServer(int port);
    
    public:
        Server(std::string const &port, std::string const &pass);
        ~Server();
        //void run();
    
};

#endif