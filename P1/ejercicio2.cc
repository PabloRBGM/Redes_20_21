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

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

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

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);
    
    // solo necesitamos 1 caracter, el buffer un poco mas grande porsi
    char buffer[80];
    char host[NI_MAXHOST];

    sockaddr cliente;
    socklen_t clienteLen = sizeof(sockaddr);
    while (true)
    {
        int bytes = recvfrom(sd, (void*)buffer, 80, 0, &cliente, &clienteLen);
        if(bytes = -1){
            return -1;
        }

        int ret = getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, nullptr, NI_MAXSERV, NI_NUMERICHOST);
        if(ret != 0){
            std::cerr << "[getnameinfo]: " << gai_strerror(rc) << '\n';
            return -1;
        }
        //char msg[] = ;
        time_t _time;
        switch (buffer[0])
        {
        case 't':
        {
            time(&_time);
            bytes = strftime(buffer, 80, "%X %p", localtime(&_time));
            ret = sendto(sd, buffer, bytes, 0, &cliente, clienteLen);
            if(ret == -1){
                return -1;
            break;
        }
        case 'd':
            time(&_time);
            bytes = strftime(buffer, 80, "%Y-%m-%d",localtime(&_time));
            ret = sendto(sd, buffer, bytes, 0, &cliente, clienteLen);
            if(ret == -1){
                return -1;
            }
            break;
        case 'q':
            close(sd);
            return 0;
            break;
        default:
            ret = sendto(sd, "Command not supported", 22, 0, &cliente, clienteLen);
            if(ret == -1){
                return -1;
            }
            break;
        }
        //std::cout << host << "\t" << i->ai_family <<  "\t" << i->ai_socktype << '\n';

    }
    
    return 0;
}
