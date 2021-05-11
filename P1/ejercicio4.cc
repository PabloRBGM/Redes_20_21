// Ejercicio 4
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv){
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

    int ret = bind(sd, res->ai_addr, res->ai_addrlen);
    if( ret == -1){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }
    freeaddrinfo(res);

    ret = listen(sd, backlog);
    if(ret == -1){
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

    while (1)
    {   
        // esperamos conexión
        cliente_sd = accept(sd, &cliente, &clienteLen);
        if(cliente_sd < 0){
            std::cerr << strerror(errno) << '\n';
            return -1;
        }
        ret = getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        if(ret < 0){
            std::cerr << "[getnameinfo]: " << gai_strerror(rc) << '\n';
            return -1;
        }
        std::cout << "Conexión desde " << host << " " << serv << '\n';

        // con el while puedo dejar clientes en cola
        //while(1){
        bytes = recv(cliente_sd, (void*)buffer, 79, 0); 
        if(bytes <= 0){
            std::cout << "Conection Ended\n";
            break;
        }             
        buffer[bytes] = '\0';
        send(cliente_sd, buffer, bytes, 0);
        //} 
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