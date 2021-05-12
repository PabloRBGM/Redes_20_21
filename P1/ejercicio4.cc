// Ejercicio 4
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv){
    if(argc != 3){
        std::cout << "Incorrect format\n";
        return 0;
    }

    addrinfo hints;
    addrinfo *res;
    int backlog = 5;

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
    if(rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << '\n';
        return -1;
    }
    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if(sd == -1){
        std::cerr << "[socket]: creación socket\n";
        return -1;
    }

    
    if(bind(sd, res->ai_addr, res->ai_addrlen) < 0){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }

    freeaddrinfo(res);

    if(listen(sd, backlog) < 0){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }

    char buffer[80];
    int bytes = 0;
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    int cliente_sd;
    sockaddr cliente;
    socklen_t clienteLen = sizeof(sockaddr);
    cliente_sd = accept(sd, &cliente, &clienteLen);
    if(cliente_sd < 0){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }
    rc = getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    if(rc != 0){
        std::cerr << "[getnameinfo]: " << gai_strerror(rc) << '\n';
        return -1;
    }
    std::cout << "Conexión desde " << host << " " << serv << '\n';
    while (1)
    {   
        bytes = recv(cliente_sd, (void*)buffer, 80, 0); 
        if(bytes <= 0){
            std::cout << "Conection Ended\n";
            break;
        }             
        buffer[bytes] = '\0';
        send(cliente_sd, buffer, bytes, 0);
    }
    
    if(close(sd) == -1){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }
    if(close(cliente_sd) == -1){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }

    return 0;
}