#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //std::cout << "SERIALIZING: \n";
    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data;

    // Type
    //std::cout << "TYPE: " << (int)type << '\n'; 
    memcpy(tmp, &type, sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    // Nick
    //std::cout << "NICK: " << nick << '\n'; 
    memcpy(tmp, nick.c_str(), 8 * sizeof(char));
    tmp += 8 * sizeof(char);

    // Message
    //std::cout << "MESSAGE " << message << '\n'; 
    memcpy(tmp, message.c_str(), 80 * sizeof(char));    
    //tmp += sizeof(80 * sizeof(char));
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //std::cout << "DE-SERIALIZING: \n";

    //Reconstruir la clase usando el buffer _data
    char* tmp = _data;

    // Type
    memcpy(&type, tmp, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    //std::cout << "TYPE: " << (int)type << '\n'; 
    
    // Nick
    tmp[7 * sizeof(char)] = '\0';
    nick = tmp;
    tmp += 8 * sizeof(char);
    //std::cout << "NICK: " << nick << '\n'; 

    // Message
    tmp[79 * sizeof(char)] = '\0';
    message = tmp;
    //tmp += sizeof(80 * sizeof(char);
    //std::cout << "MESSAGE: " << message << '\n'; 

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    ChatMessage msg;
    Socket* client;

    //std::cout << "SERVER: INICIADO\n";

    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //std::cout << "SERVER: ESPERANDO MENSAJE\n";

        //Recibir Mensajes en y en función del tipo de mensaje
        if(socket.recv(msg, client) < 0){
            std::cout << "SERVER: Error recibiendo mensaje\n";
            continue;
        }

        client->bind();

        switch (msg.type)
        {
            // - LOGIN: Añadir al vector clients
            case ChatMessage::LOGIN:
            {
                //std::cout << "SERVER: LOGIN DE: " << msg.nick << "\n";
                clients.push_back(std::move(std::make_unique<Socket>(*client)));
                //std::cout << "NUMERO DE CLIENTES ACTUALES: " << clients.size() << "\n";
                break;
            }

            // - LOGOUT: Eliminar del vector clients
            case ChatMessage::LOGOUT:
            {
                //std::cout << "SERVER: LOGOUT DE: " << msg.nick << "\n";
                for(auto it = clients.begin(); it != clients.end(); ++it){
                    if((*it->get()) == (*client)){
                        clients.erase(it);
                        it->release();
                        break;
                    }
                }
                break;
            }

            // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
            case ChatMessage::MESSAGE:
            {
                //std::cout << "SERVER: MESSAGE DE: " << msg.nick << "\n";
                for(auto it = clients.begin(); it != clients.end(); ++it){
                    if(!( (*it->get()) == (*client) )){
                        socket.send(msg, (*it->get()));
                    }
                }
                break;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    //std::cout << "CLIENT: " + nick + " HAGO LOGIN\n";

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    //std::cout << "CLIENT: " + nick + " HAGO LOGOUT\n";

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    std::string msg;
    
    while (true)
    {   
        // Leer stdin con std::getline
        std::getline(std::cin, msg);
        
        ChatMessage em(nick, msg);
        em.type = ChatMessage::MESSAGE;

        //std::cout << "CLIENT: " + nick + " MANDO MENSAJE\n";

        // Enviar al servidor usando socket
        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    ChatMessage msg;
    while(true)
    {
        //Recibir Mensajes de red
        if(socket.recv(msg) < 0) continue;

        std::cout << msg.nick << ": " << msg.message << '\n';
    }
}