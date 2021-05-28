#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
private:
    
    static const size_t MAX_NAME = 80;
    char name[MAX_NAME];

    int16_t x;
    int16_t y;

public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t data_size = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(data_size);

        // puntero auxiliar para modificar la _data
        char* tmp = _data;

        // copiamos el nombre en _data
        memcpy(tmp, name, MAX_NAME * sizeof(char));
        tmp += MAX_NAME * sizeof(char);

        // copiamos 'x' e 'y' en _data
        memcpy(tmp, &x, sizeof(int16_t));
        tmp += sizeof(int16_t);
        memcpy(tmp, &y, sizeof(int16_t));
        //tmp += sizeof(int16_t);
    }

    int from_bin(char * data)
    {
        // actualizamos el size por si acaso
        alloc_data(MAX_NAME * sizeof(char) + 2 * sizeof(int16_t));

        char* tmp = data;

        // copiamos el nombre que nos llega en data
        memcpy(name, tmp, MAX_NAME * sizeof(char));
        tmp += MAX_NAME * sizeof(char);

        // copiamos los valores que nos llegan de 'x' e 'y'
        memcpy(&x, tmp, sizeof(int16_t));
        tmp += sizeof(int16_t);
        memcpy(&y, tmp, sizeof(int16_t));
        //tmp += sizeof(int16_t);
        
        return 0;
    }

    char* getName(){
        return name;
    }

    int16_t getX(){
        return x;
    }
        
    int16_t getY(){
        return y;
    }
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 0. Crear el fichero
    int fd = open("./player_data", O_CREAT | O_TRUNC | O_RDWR, 0666);
    if(fd < 0){
        std::cerr <<  strerror(errno) << '\n';
        return -1;
    }

    // 1. Serializar el objeto one_w
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    int ret = write(fd, one_w.data(), one_w.size());
    if(ret < 0){
        std::cerr <<  strerror(errno) << '\n';
        return -1;
    }

    // 3. Cerrar el fichero
    ret = close(fd);
    if(ret < 0){
        std::cerr <<  strerror(errno) << '\n';
        return -1;
    }

    // 3. Leer el fichero
    fd = open("./player_data", O_RDONLY, 0444);
    if(fd < 0){
        std::cerr <<  strerror(errno) << '\n';
        return -1;
    }

    int32_t dataSize = 80 + 2 * sizeof(int16_t);
    char data[dataSize];

    ret = read(fd, data, dataSize);
    if(ret < 0){
        std::cerr <<  strerror(errno) << '\n';
        return -1;
    }

    // 4. "Deserializar" en one_r
    one_r.from_bin(data);

    // 5. Mostrar el contenido de one_r
    std::cout   << "Name: " << one_r.getName() 
                << "\nX: " << one_r.getX() 
                << "\nY: " << one_r.getY() << "\n";

    ret = close(fd);
    if(ret < 0){
        std::cerr <<  strerror(errno) << '\n';
        return -1;
    }

    return 0;
}

