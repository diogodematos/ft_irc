/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:35:01 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/11/21 16:11:26 by dcarrilh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netdb.h>

class Server
{
    private:
        Server();
        Server(Server const &rhs);
        Server &operator=(Server const &rhs);
        std::string _ip;
        std::string _port;
        int _listening;
    
    public:
        Server(std::string const &ip, std::string const &port);
        ~Server();
        Server(Server const &rhs);
        Server &operator=(Server const &rhs);
    
};

#endif