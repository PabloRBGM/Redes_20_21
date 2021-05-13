// Ejercicio 6
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <thread>

class my_thread{
private:
    char buffer[80];
    int bytes;
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    sockaddr cliente;
    socklen_t clienteLen = sizeof(sockaddr);
    time_t _time;
    int tam = 0;

    std::thread _thread;   
public:
    my_thread() {}
    static int receive(my_thread* th, int _sd){
        while (1)
        {
            th->bytes = recvfrom(_sd, (void*)th->buffer, 80, 0, &th->cliente, &th->clienteLen);
            if(th->bytes < 0){
                std::cerr << strerror(errno) << '\n';
                return -1;
            }

            int rc = getnameinfo(&th->cliente, th->clienteLen, th->host, NI_MAXHOST, th->serv, NI_MAXSERV, NI_NUMERICHOST);
            if(rc != 0){
                std::cerr << "[getnameinfo]: " << gai_strerror(rc) << '\n';
                return -1;
            }

            switch (th->buffer[0])
            {
                case 't':
                {
                    time(&th->_time);
                    th->tam = strftime(th->buffer, 80, "%X %p", localtime(&th->_time));
                    th->buffer[th->tam] = '\0';
                    if(sendto(_sd, th->buffer, th->tam, 0, &th->cliente, th->clienteLen) < 0){
                        std::cerr << strerror(errno) << '\n';
                        return -1;
                    }                
                    break;
                }
                case 'd':
                {
                    time(&th->_time);
                    th->tam = strftime(th->buffer, 80, "%Y-%m-%d",localtime(&th->_time));
                    th->buffer[th->tam] = '\0';
                    if(sendto(_sd, th->buffer, th->tam, 0, &th->cliente, th->clienteLen) < 0){
                        std::cerr << strerror(errno) << '\n';
                        return -1;
                    }   
                    break;
                }
                case 'q':
                {
                    std::cout << "Exiting\n";
                    exit(0);
                    close(_sd);
                    return 0;
                }
                default:
                { 
                    if(sendto(_sd, "Command not supported", 24, 0, &th->cliente, th->clienteLen) < 0){
                        std::cerr << strerror(errno) << '\n';
                        return -1;
                    }   
                    std::cout << "Command not supported " << th->buffer[0] << "\n";        
                    break;
                }
            }
            std::cout << th->bytes << " bytes from " << th->host << ":" << th->serv << "\nTh_ID: " << std::this_thread::get_id() << '\n';
            sleep(3);
        }
    }
    my_thread(int sd) : _thread(std::thread(receive, this, sd)){}
    void join(){ _thread.join(); }
};

int main(int argc, char** argv){
    if(argc != 3){
        std::cout << "Incorrect format\n";
        return 0;
    }

    const unsigned int MAX_THREAD = 3;
    my_thread pool[MAX_THREAD];
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
    
    // initialize threads
    for(int i = 0; i < MAX_THREAD; i++){
        pool[i] = my_thread(sd);
    }

    // wait for threads
    for(int i = 0; i < MAX_THREAD; i++){
        pool[i].join();
    }
    
    return 0;
}
