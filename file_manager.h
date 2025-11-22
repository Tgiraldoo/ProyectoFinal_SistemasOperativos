#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>

// Usamos 'byte_vector' para consistencia.
using byte_vector = std::vector<unsigned char>;

// Lee un archivo y devuelve su contenido.
// Ahora devuelve un 'bool' para indicar éxito/fracaso y pasa el vector por referencia.
// Esto permite distinguir entre un archivo vacío y un error de lectura.
bool readFile(const std::string& path, byte_vector& out_data);

// Escribe datos en un archivo.
bool writeFile(const std::string& path, const byte_vector& data);

// Lista los archivos en un directorio.
std::vector<std::string> listDirectory(const std::string& path);

#endif // FILE_MANAGER_H