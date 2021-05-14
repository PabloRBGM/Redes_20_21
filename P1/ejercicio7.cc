// Ejercicio 7
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <thread>

class my_tcp_thread{
private:
    int _sd;
public:
    my_tcp_thread(int sd) : _sd(sd) {}
    void echo_message(){
        char buffer[80];
        int bytes = 0;
        while (1)
        {   
            bytes = recv(_sd, (void*)buffer, 80, 0); 
            if(bytes <= 0){
                std::cout << "Conection Ended\n";
                if(close(_sd) == -1){
                    std::cerr << strerror(errno) << '\n';
                    return;
                }
                break;
            }             
            buffer[bytes] = '\0';
            send(_sd, buffer, bytes, 0);
        }
    }

};

int main(int argc, char** argv){
    if(argc != 3){
        std::cout << "Incorrect format\n";
        return 0;
    }

    const unsigned int MAX_THREAD = 3;
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

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    int cliente_sd;
    sockaddr cliente;
    socklen_t clienteLen = sizeof(sockaddr);
    char buffer[80];
    while (1)
    {
        cliente_sd = accept(sd, &cliente, &clienteLen);
        if(cliente_sd >= 0){     
            my_tcp_thread* tcp_th = new my_tcp_thread(cliente_sd);
            
            int rc = getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
            if(rc != 0){
                std::cerr << "[getnameinfo]: " << gai_strerror(rc) << '\n';
                continue;
            }
            std::cout << "Conexión desde " << host << " " << serv << '\n';
        
            std::thread([tcp_th](){
                tcp_th->echo_message();
            }).detach();        
        }
        else{
            std::cerr << strerror(errno) << '\n';
        }
    }
    
    if(close(sd) == -1){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }

    return 0;
}