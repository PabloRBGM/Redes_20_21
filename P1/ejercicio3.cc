// Ejercicio 3
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv){
    if(argc != 4){
        std::cout << "Incorrect format\n";
        return 0;
    }
    addrinfo hints;
    addrinfo *serv_res;
    
    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int sd = socket(hints.ai_family, hints.ai_socktype, 0);
    if(sd < 0){
        std::cerr << "[socket]: creación socket\n";
        return -1;
    }

    int rc = getaddrinfo(argv[1], argv[2], &hints, &serv_res);
    if(rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << '\n';
        return -1;
    }

    char buffer[80];
    int bytes;

    if(sendto(sd, argv[3], 2, 0, serv_res->ai_addr, serv_res->ai_addrlen) < 0){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }
    else{
        bytes = recvfrom(sd, (void*)buffer, 80, 0, serv_res->ai_addr, &serv_res->ai_addrlen);
        if(bytes < 0){
            std::cerr << strerror(errno) << '\n';
            return -1;
        }
        buffer[bytes] = '\0';
        std::cout << buffer << '\n';
    }

    freeaddrinfo(serv_res);

    if(close(sd) == -1){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }  

    return 0;
}
