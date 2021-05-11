// Ejercicio 2
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
    addrinfo *serv_res;
    char* client_port = "3333";
    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], client_port, &hints, &res);
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

    rc = getaddrinfo(argv[1], argv[2], &hints, &serv_res);
    if(rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << '\n';
        return -1;
    }

    char buffer[80];
    // char host[NI_MAXHOST];
    // char serv[NI_MAXSERV];

    int bytes;
    socklen_t serverLen = sizeof(sockaddr);
    
    // ret = getnameinfo(&server, serverLen, argv[1], NI_MAXHOST, argv[2], NI_MAXSERV, NI_NUMERICHOST |  NI_NUMERICSERV);
    // if(ret != 0){
    //     std::cerr << "[getnameinfo]: " << gai_strerror(rc) << '\n';
    //     return -1;
    // }

    ret = sendto(sd, argv[3], 2, 0, serv_res->ai_addr, serv_res->ai_addrlen);
    if(ret == -1){
        std::cout  << "Connection couldn't be established\n";
    }
    else{
        bytes = recvfrom(sd, (void*)buffer, 80, 0, serv_res->ai_addr, &serv_res->ai_addrlen);
        if(bytes == -1){
            return -1;
        }
        std::cout << buffer << '\n';
    }

    freeaddrinfo(serv_res);

    if(close(sd) == -1){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }  

    return 0;
}
