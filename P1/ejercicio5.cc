// Ejercicio 5
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
    addrinfo *serv_res;

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int sd = socket(hints.ai_family, hints.ai_socktype, 0);
    if(sd < 0){
        std::cerr << "[socket]: creación socket\n";
        return -1;
    }

    hints.ai_flags = AI_PASSIVE;
    int rc = getaddrinfo(argv[1], argv[2], &hints, &serv_res);
    if(rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << '\n';
        return -1;
    }

    char buffer[80];
    int bytes;
    
    int ret = connect(sd, serv_res->ai_addr, serv_res->ai_addrlen);
    if(ret < 0){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }
    
    freeaddrinfo(serv_res);

    while(1){
        std::cin >> buffer;

        if(!strcmp(buffer, "q"))
            break;
        else{  
            if(send(sd, buffer, strlen(buffer), 0) < 0){
                std::cerr << strerror(errno) << '\n';
                return -1;
            }
            if(recv(sd, buffer, 80, 0) < 0){
                std::cerr << strerror(errno) << '\n';
                return -1;
            }
            std::cout << buffer << '\n';
        }
    }

    if(close(sd) == -1){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }  

    return 0;
}
