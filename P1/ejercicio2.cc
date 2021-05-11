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

    int ret = bind(sd, res->ai_addr, res->ai_addrlen);
    if( ret == -1){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }

    freeaddrinfo(res);
    
    // solo necesitamos 1 caracter, el buffer un poco mas grande porsi
    char buffer[80];
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    sockaddr cliente;
    socklen_t clienteLen = sizeof(sockaddr);
    while (true)
    {
        int bytes = recvfrom(sd, (void*)buffer, 80, 0, &cliente, &clienteLen);
        if(bytes == -1){
            return -1;
        }

        ret = getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);
        if(ret != 0){
            std::cerr << "[getnameinfo]: " << gai_strerror(rc) << '\n';
            return -1;
        }

        time_t _time;
        int tam = 0;
        switch (buffer[0])
        {
            case 't':
            {
                time(&_time);
                tam = strftime(buffer, 80, "%X %p", localtime(&_time));
                ret = sendto(sd, buffer, tam, 0, &cliente, clienteLen);
                if(ret == -1)
                    return -1;
                break;
            }
            case 'd':
            {
                time(&_time);
                tam = strftime(buffer, 80, "%Y-%m-%d",localtime(&_time));
                ret = sendto(sd, buffer, tam, 0, &cliente, clienteLen);
                if(ret == -1)
                    return -1;
                break;
            }
            case 'q':
            {
                std::cout << "Saliendo\n";
                close(sd);
                return 0;
            }
            default:
            {
                std::cout << "Command not supported " << buffer[0] << "\n";        
                break;
            }
        }
        std::cout << bytes << " bytes de " << host << ":" << serv << '\n';

    }
    
    return 0;
}
