#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data;

    // Type
    memcpy(tmp, &type, sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    // Nick
    memcpy(tmp, &nick, 8 * sizeof(char));
    tmp += sizeof(8 * sizeof(char));

    // Message
    memcpy(tmp, &nick, 80 * sizeof(char));
    //tmp += sizeof(80 * sizeof(char));
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* tmp = _data;

    // Type
    memcpy(&type, tmp, sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    // Nick
    memcpy(&nick, tmp, 8 * sizeof(char));
    tmp += 8 * sizeof(char);

    // Message
    memcpy(&message, tmp, 80 * sizeof(char));
    //tmp += sizeof(80 * sizeof(char);

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        ChatMessage msg;
        Socket* client;

        if(socket.recv(msg, client) < 0) continue;

        switch (msg.type)
        {
        // - LOGIN: Añadir al vector clients
        case ChatMessage::LOGIN:
            clients.push_back(std::move(std::make_unique<Socket>(*client)));
            break;

        // - LOGOUT: Eliminar del vector clients
        case ChatMessage::LOGOUT:
            for(auto it = clients.begin(); it != clients.end(); ++it){
                if((*it).get() == client){
                    clients.erase(it);
                    (*it).release();
                    break;
                }
            }
            break;

        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        case ChatMessage::MESSAGE:
            for(auto it = clients.begin(); it != clients.end(); ++it){
                if((*it).get() != client){
                    socket.send(msg, *(*it).get());
                }
            }
            break;
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

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

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

        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::cout << msg.nick << ": " << msg.message << '\n';
    }
}

