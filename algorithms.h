#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <string>
#include <vector>

// Usamos 'unsigned char' en todo el proyecto.
using byte_vector = std::vector<unsigned char>;

// Compresión RLE 
byte_vector compressRLE(const byte_vector& data);
byte_vector decompressRLE(const byte_vector& data);

//Encriptación Vigenère
byte_vector processVigenere(const byte_vector& data, const std::string& key, bool encrypt);

#endif // ALGORITHMS_H