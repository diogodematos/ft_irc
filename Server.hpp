/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:35:01 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/12/06 16:24:08 by dcarrilh         ###   ########.fr       */
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
        std::string _pass;
        int _listening;
    
    public:
        Server(std::string const &ip, std::string const &pass);
        ~Server();
        Server(Server const &rhs);
        Server &operator=(Server const &rhs);
    
};

#endif