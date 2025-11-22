#include "algorithms.h"

// Implementación de la compresión Run-Length Encoding (RLE).
byte_vector compressRLE(const byte_vector& data) {
    if (data.empty()) {
        return {};
    }

    byte_vector compressed_data;
    for (size_t i = 0; i < data.size(); ++i) {
        // El contador se inicializa en 1 para el byte actual.
        unsigned char count = 1;
        
        // Se mira hacia adelante para contar cuántos bytes consecutivos son idénticos.
        // El bucle se detiene si se alcanza el final de los datos, si se encuentra un byte
        // diferente, o si el contador llega a 255 (el máximo valor para un unsigned char).
        while (i + 1 < data.size() && data[i] == data[i + 1] && count < 255) {
            count++;
            i++; // Se avanza el índice principal para saltar los bytes ya contados.
        }
        
        // Se añade el par [contador, valor] al resultado.
        compressed_data.push_back(count);
        compressed_data.push_back(data[i]);
    }
    return compressed_data;
}

// Implementación de la descompresión Run-Length Encoding (RLE).

byte_vector decompressRLE(const byte_vector& data) {
    // Validación de robustez: Un archivo RLE válido siempre debe tener un número par de bytes.
    // Si es impar, el archivo está corrupto. Se devuelve un vector vacío para señalar el error.
    if (data.size() % 2 != 0) {
        return {};
    }

    byte_vector decompressed_data;
    // Se itera sobre los datos de entrada, avanzando de dos en dos.
    for (size_t i = 0; i < data.size(); i += 2) {
        unsigned char count = data[i];
        unsigned char value = data[i + 1];
        
        // Se añade el byte 'value' al resultado 'count' veces.
        for (int j = 0; j < count; ++j) {
            decompressed_data.push_back(value);
        }
    }
    return decompressed_data;
}

//Implementación del Cifrado Vigenère para encriptar y desencriptar.

byte_vector processVigenere(const byte_vector& data, const std::string& key, bool encrypt) {
    if (key.empty()) {
        return data; // Si no se proporciona clave, no se realiza ninguna operación.
    }

    byte_vector result_data;
    result_data.reserve(data.size()); // Pre-reserva memoria para eficiencia.

    for (size_t i = 0; i < data.size(); ++i) {
        unsigned char original_byte = data[i];
        // Se obtiene el byte de la clave correspondiente usando la operación módulo.
        // Esto hace que la clave se repita si es más corta que los datos.
        // Se usa static_cast para asegurar que el char de la clave se trate como unsigned.
        unsigned char key_byte = static_cast<unsigned char>(key[i % key.length()]);
        unsigned char processed_byte;
        
        if (encrypt) {
            // Encriptación: (valor + clave) mod 256
            processed_byte = (original_byte + key_byte) % 256;
        } else {
            // Desencriptación: (valor - clave + 256) mod 256
            processed_byte = (original_byte - key_byte + 256) % 256;
        }
        result_data.push_back(processed_byte);
    }
    return result_data;
}