/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcarrilh <dcarrilh@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 14:01:03 by dcarrilh          #+#    #+#             */
/*   Updated: 2024/12/20 15:07:24 by dcarrilh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool isValidPort(std::string port)
{
    const char *iport = port.c_str();
    char *res;
    double num = std::strtod(iport, &res);
    if ((unsigned)strlen(res) != 0 || port.find(".") != std::string::npos)
    {
        std::cerr << "Error: Port must be a number!" << std::endl;
        return 0;
    }
    if(num < 1024 || num > 65535)
    {
        std::cerr << "Error: Port needs to be between 1024 and 65535!" << std::endl;
        return 0;
    }
    return (1);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Error: Wrong number of arguments!" << std::endl;
        return 1;
    }
    else if (!isValidPort(argv[1]))
        return 1;
    else
    {
        try
        {
            Server myirc(argv[1], argv[2]);
            myirc.run();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
         
    }
    return 0;
}
