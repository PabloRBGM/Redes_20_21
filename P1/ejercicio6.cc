// Ejercicio 6
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <thread>

class my_udp_thread{
private:
    int _sd;  
public:
    my_udp_thread(int sd) : _sd(sd){}
    ~my_udp_thread(){}
    void time_serv(){
        char buffer[80];
        int bytes;
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        sockaddr cliente;
        socklen_t clienteLen = sizeof(sockaddr);
        time_t _time;
        int tam = 0;

        while (1)
        {
            bytes = recvfrom(_sd, (void*)buffer, 80, 0, &cliente, &clienteLen);
            if(bytes < 0){
                std::cerr << strerror(errno) << '\n';
                continue;
            }

            int rc = getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);
            if(rc != 0){
                std::cerr << "[getnameinfo]: " << gai_strerror(rc) << '\n';
                continue;
            }

            switch (buffer[0])
            {
                case 't':
                {
                    time(&_time);
                    tam = strftime(buffer, 80, "%X %p", localtime(&_time));
                    buffer[tam] = '\0';
                    if(sendto(_sd, buffer, tam, 0, &cliente, clienteLen) < 0){
                        std::cerr << strerror(errno) << '\n';
                        continue;
                    }                
                    break;
                }
                case 'd':
                {
                    time(&_time);
                    tam = strftime(buffer, 80, "%Y-%m-%d",localtime(&_time));
                    buffer[tam] = '\0';
                    if(sendto(_sd, buffer, tam, 0, &cliente, clienteLen) < 0){
                        std::cerr << strerror(errno) << '\n';
                        continue;
                    }   
                    break;
                }
                case 'q':
                {
                    std::cout << "Exiting\n";
                    close(_sd);
                    exit(0);                  
                    continue;
                }
                default:
                { 
                    if(sendto(_sd, "Command not supported", 24, 0, &cliente, clienteLen) < 0){
                        std::cerr << strerror(errno) << '\n';
                        continue;
                    }   
                    std::cout << "Command not supported " << buffer[0] << "\n";        
                    break;
                }
            }
            std::cout << bytes << " bytes from " << host << ":" << serv << "\nTh_ID: " << std::this_thread::get_id() << '\n';
            sleep(3);
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

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
    if(rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << '\n';
        return -1;
    }
    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if(sd < 0){
        std::cerr << "[socket]: socket creation\n";
        return -1;
    }

    if(bind(sd, res->ai_addr, res->ai_addrlen) < 0){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }

    freeaddrinfo(res);
    
    //Threads
    for(int i = 0; i < MAX_THREAD; i++){
        my_udp_thread* udp_th = new my_udp_thread(sd);
        std::thread([udp_th](){
            udp_th->time_serv();
        }).detach();
    }

    char buffer[80];
    while (strcmp(buffer, "q\0"))
    {
        std::cin >> buffer;
    } 

    if(close(sd) == -1){
        std::cerr << strerror(errno) << '\n';
        return -1;
    }
    return 0;
}
