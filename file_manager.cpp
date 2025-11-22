#include "file_manager.h"
#include <fcntl.h>      // Para open() y las banderas O_RDONLY, O_WRONLY, O_CREAT
#include <unistd.h>     // Para read(), write(), close()
#include <sys/stat.h>   // Para stat() y los modos de archivo
#include <dirent.h>     // Para opendir(), readdir(), closedir()
#include <iostream>     // Para perror()
#include <cstring>      // Para strcmp()
#include <errno.h>      // Para la variable global 'errno'

//¿Implementación de la lectura de archivos con llamadas al sistema.

bool readFile(const std::string& path, byte_vector& out_data) {
    out_data.clear(); // Se limpia el vector de salida para asegurar que no contenga datos antiguos.
    
    // 1. Abrir el archivo: open() pide al kernel acceso al archivo.
    // Devuelve un 'descriptor de archivo' (un entero) si tiene éxito, o -1 si falla.
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        perror(("Error al abrir archivo de entrada " + path).c_str());
        return false;
    }

    unsigned char read_buffer[4096]; // Un buffer temporal para leer en trozos.
    ssize_t bytes_read;

    // 2. Leer el archivo: read() intenta leer hasta 4096 bytes del archivo en el buffer.
    // Devuelve el número de bytes leídos, 0 si se llega al final del archivo, o -1 si hay un error.
    while ((bytes_read = read(fd, read_buffer, sizeof(read_buffer))) > 0) {
        // Se insertan los bytes leídos del buffer temporal en nuestro vector final.
        out_data.insert(out_data.end(), read_buffer, read_buffer + bytes_read);
    }

    // Se comprueba el resultado final de 'read'. Si es 0, la lectura fue exitosa (llegó al final).
    // Si es -1, hubo un error durante la última operación de lectura.
    bool success = (bytes_read == 0);
    if (!success) {
        perror(("Error al leer archivo " + path).c_str());
    }

    // 3. Cerrar el archivo: close() le dice al kernel que hemos terminado con el archivo, liberando recursos.
    close(fd);
    return success;
}

//Implementación de la escritura de archivos con llamadas al sistema.
 
bool writeFile(const std::string& path, const byte_vector& data) {
    // 1. Abrir/Crear el archivo: O_WRONLY (escritura), O_CREAT (crear si no existe), O_TRUNC (vaciar si existe).
    // 0644 son los permisos: lectura/escritura para el dueño, lectura para otros.
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror(("Error al abrir/crear archivo de salida " + path).c_str());
        return false;
    }

    // 2. Escribir en el archivo: La llamada 'write' no garantiza escribir todos los datos de una vez.
    // Por eso, se implementa un bucle para asegurar una escritura completa y robusta.
    const unsigned char* p_data = data.data();
    size_t bytes_to_write = data.size();
    ssize_t bytes_written_total = 0;

    while (bytes_to_write > 0) {
        ssize_t bytes_written_now = write(fd, p_data, bytes_to_write);
        if (bytes_written_now == -1) {
            // Si la escritura fue interrumpida por una señal del sistema (EINTR), simplemente reintentamos.
            if (errno == EINTR) continue;
            
            perror(("Error al escribir en archivo " + path).c_str());
            close(fd);
            return false;
        }
        bytes_to_write -= bytes_written_now;
        p_data += bytes_written_now;
        bytes_written_total += bytes_written_now;
    }

    // 3. Cerrar el archivo.
    close(fd);
    return (size_t)bytes_written_total == data.size();
}

/**
 * @brief Implementación del listado de directorios con llamadas al sistema.
 */
std::vector<std::string> listDirectory(const std::string& path) {
    std::vector<std::string> files;
    // 1. Abrir el directorio.
    DIR* dir = opendir(path.c_str());
    if (dir == nullptr) {
        perror(("Error al abrir directorio " + path).c_str());
        return files;
    }

    struct dirent* entry;
    // 2. Leer cada entrada del directorio.
    while ((entry = readdir(dir)) != nullptr) {
        // Se ignoran las entradas especiales "." (directorio actual) y ".." (directorio padre).
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            std::string full_path = path + "/" + entry->d_name;
            struct stat st;
            // Se usa 'stat' para obtener información de la entrada y asegurarse de que es un archivo regular.
            if (stat(full_path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                 files.push_back(entry->d_name);
            }
        }
    }

    // 3. Cerrar el directorio.
    closedir(dir);
    return files;
}