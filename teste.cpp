#include <sstream>
#include <cstring>
#include <unistd.h>
# include <iostream>
# include <string>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netdb.h>

int main(int argc, char **argv)
{
    int _listening = socket(AF_INET, SOCK_STREAM, 0);
    std::string _pass = argv[2];
    std::stringstream ss(argv[1]);
    int _port;
    ss >> _port;

    if (argc != 3)
    {
        std::cerr << "Error: wrong number of arguments" << std::endl;
        return (0);
    }

    if (_listening == -1)
    {
        std::cerr << "Can't create a socket! Quitting" << std::endl;
        return -1;
    }
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(_port);
    inet_pton(AF_INET, _pass.c_str(), &hint.sin_addr);

    if (bind(_listening, (sockaddr *)&hint, sizeof(hint)) == -1)
    {
        std::cerr << "Can't bind to IP/port" << std::endl;
        return -2;
    }
    
    if (listen(_listening, SOMAXCONN) == -1)
    {
        std::cerr << "Can't listen!" << std::endl;
        return -3;
    }

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    
    int clientSocket = accept(_listening, (sockaddr *)&client, &clientSize);

    if (clientSocket == -1)
    {
        std::cerr << "Problem with client connecting!" << std::endl;
        return -4;
    }
    
    close(_listening);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr *)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

    if (result)
    {
        std::cout << host << " connected on " << svc << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }
    
    char buf[4096];
    while (true)
    {
        memset(buf, 0, 4096);
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            std::cerr << "There was a connection issue" << std::endl;
            break;
        }
        if (bytesReceived == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }
        std::cout << "Received: " << std::string(buf, 0, bytesReceived) << std::endl;
        send(clientSocket, buf, bytesReceived + 1, 0);
    }
    close(clientSocket);
    return 0;
}
