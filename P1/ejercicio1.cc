// Ejercicio 1
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv){
    if(argc < 2 || argc > 3){
        std::cout << "Incorrect format\n";
        return 0;
    }
    addrinfo hints;
    addrinfo *res;

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_UNSPEC;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
    if(rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << '\n';
        return -1;
    }

    char host[NI_MAXHOST];
    for(auto i = res; i != nullptr; i = i->ai_next){
        rc = getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, nullptr, NI_MAXSERV, NI_NUMERICHOST);
        if(rc != 0){
            std::cerr << "[getnameinfo]: " << gai_strerror(rc) << '\n';
            return -1;
        }
        std::cout << host << "\t" << i->ai_family <<  "\t" << i->ai_socktype << '\n';
    }

    return 0;
}