#include <iostream>      // Para std::cout, std::cerr
#include <string>        // Para std::string
#include <vector>        // Para std::vector
#include <pthread.h>     // Para la concurrencia con pthreads (hilos POSIX)
#include <sys/stat.h>    // Para stat (verificar si una ruta es archivo/directorio) y mkdir
#include <mutex>         // Para std::mutex (proteger datos compartidos entre hilos)

#include "file_manager.h" // Nuestra interfaz para leer/escribir archivos y directorios
#include "algorithms.h"   // Nuestra interfaz para los algoritmos de compresión/encriptación


std::mutex cout_mutex;

// Estructura para pasar múltiples argumentos a la función del hilo.
struct ThreadData {
    std::string operations;
    std::string comp_alg;
    std::string enc_alg;
    std::string input_path;
    std::string output_path;
    std::string key;
};

// Función para imprimir mensajes de forma segura desde múltiples hilos.
void safe_print(const std::string& message) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << message << std::endl;
}

//Imprime las instrucciones de uso del programa si los argumentos son incorrectos.
void printUsage() {
    std::cout << "Uso: ./gsea <operaciones> --comp-alg <alg> --enc-alg <alg> -i <entrada> -o <salida> [-k <clave>]\n"
              << "Operaciones: Combinación de -c (comprimir), -d (descomprimir), -e (encriptar), -u (desencriptar)\n"
              << "Algoritmos de Compresión: rle\n"
              << "Algoritmos de Encriptación: vigenere\n"
              << "Ejemplo (archivo): ./gsea -ce --comp-alg rle --enc-alg vigenere -i in.txt -o out.dat -k clave\n"
              << "Ejemplo (directorio): ./gsea -ce --comp-alg rle --enc-alg vigenere -i ./dir_in -o ./dir_out -k clave\n";
}

//Lógica principal para procesar un único archivo.

void processFile(const ThreadData& data) {
    safe_print("Procesando: " + data.input_path + " -> " + data.output_path);
    
    byte_vector file_data;
    // La firma de readFile ahora devuelve un bool, permitiendo distinguir un error de un archivo vacío.
    if (!readFile(data.input_path, file_data)) {
        // Usamos el mutex para imprimir errores también, evitando la mezcla de mensajes.
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cerr << "Error: No se pudo leer el archivo de entrada " << data.input_path << ".\n";
        return;
    }

    // El orden de las operaciones es crucial para que sean reversibles:
    // Para archivar: Comprimir -> Encriptar
    // Para restaurar: Desencriptar -> Descomprimir
    if (data.operations.find('u') != std::string::npos) {
        if (data.enc_alg == "vigenere") file_data = processVigenere(file_data, data.key, false);
    }
    if (data.operations.find('d') != std::string::npos) {
        if (data.comp_alg == "rle") file_data = decompressRLE(file_data);
    }
    if (data.operations.find('c') != std::string::npos) {
        if (data.comp_alg == "rle") file_data = compressRLE(file_data);
    }
    if (data.operations.find('e') != std::string::npos) {
        if (data.enc_alg == "vigenere") file_data = processVigenere(file_data, data.key, true);
    }

    if (!writeFile(data.output_path, file_data)) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cerr << "Error: No se pudo escribir el archivo de salida " << data.output_path << ".\n";
    } else {
        safe_print("Terminado: " + data.input_path);
    }
}

// Función que ejecuta cada hilo trabajador.
void* thread_routine(void* arg) {
    // Convertir el puntero genérico de vuelta a nuestro tipo de datos.
    ThreadData* data = static_cast<ThreadData*>(arg);
    
    // Ejecutar la lógica de procesamiento del archivo.
    processFile(*data);
    
    // Liberar la memoria asignada en 'main' para este hilo.
    delete data;
    return nullptr;
}


int main(int argc, char* argv[]) {
    // Validación inicial: se necesitan al menos 7 argumentos para una operación básica.
    // ej: ./gsea -c --comp-alg rle -i in.txt -o out.txt
    if (argc < 7) {
        printUsage();
        return 1;
    }

    // Estructura para almacenar los argumentos comunes a todas las operaciones.
    ThreadData common_data;

    // Bucle para analizar los argumentos de la línea de comandos.
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
            if (arg == "-i") common_data.input_path = argv[++i];
            else if (arg == "-o") common_data.output_path = argv[++i];
            else if (arg == "-k") common_data.key = argv[++i];
            else if (arg == "--comp-alg") common_data.comp_alg = argv[++i];
            else if (arg == "--enc-alg") common_data.enc_alg = argv[++i];
            else common_data.operations += arg.substr(1); // Acumula operaciones (ej: -c -e -> "ce")
        }
    }

    // Validaciones de argumentos post-análisis.
    if (common_data.input_path.empty() || common_data.output_path.empty()) {
        std::cerr << "Error: Rutas de entrada y salida son obligatorias.\n";
        printUsage();
        return 1;
    }
    if ((common_data.operations.find('e') != std::string::npos || common_data.operations.find('u') != std::string::npos) && common_data.key.empty()) {
        std::cerr << "Error: Operaciones de encriptación/desencriptación requieren una clave (-k).\n";
        printUsage();
        return 1;
    }

    // Usamos la llamada al sistema 'stat' para obtener información sobre la ruta de entrada.
    struct stat path_stat;
    if (stat(common_data.input_path.c_str(), &path_stat) != 0) {
        perror("Error al obtener información de la ruta de entrada");
        return 1;
    }

    // Determinamos si la ruta es un directorio o un archivo.
    if (S_ISDIR(path_stat.st_mode)) {
        // MODO DIRECTORIO (CONCURRENTE) 
        safe_print("Modo directorio detectado. Procesando de forma concurrente...");
        std::vector<std::string> files = listDirectory(common_data.input_path);
        if (files.empty()) {
            safe_print("El directorio de entrada está vacío.");
            return 0;
        }

        // Creamos el directorio de salida si no existe. 0755 son los permisos (rwx-rx-rx).
        mkdir(common_data.output_path.c_str(), 0755);

        // Vector para almacenar los identificadores de los hilos que se creen con éxito.
        std::vector<pthread_t> threads;
        for (const auto& file : files) {
            // Creamos una copia de los datos en el heap para cada hilo.
            // Cada hilo necesita su propia copia de las rutas.
            ThreadData* thread_data = new ThreadData(common_data); 
            thread_data->input_path = common_data.input_path + "/" + file;
            thread_data->output_path = common_data.output_path + "/" + file;
            
            pthread_t new_thread;
            // Creamos el hilo. Si tiene éxito, lo añadimos a nuestro vector de seguimiento.
            if (pthread_create(&new_thread, nullptr, thread_routine, thread_data) == 0) {
                threads.push_back(new_thread);
            } else {
                perror("Error al crear el hilo");
                delete thread_data; // Si la creación falla, limpiamos la memoria.
            }
        }

        // El hilo principal espera a que todos los hilos trabajadores terminen.
        safe_print("Esperando a que terminen " + std::to_string(threads.size()) + " hilos...");
        for (pthread_t& t : threads) {
            pthread_join(t, nullptr);
        }
        safe_print("Procesamiento de directorio completado.");

    } else {
        // MODO ARCHIVO ÚNICO
        processFile(common_data);
    }

    return 0;
}