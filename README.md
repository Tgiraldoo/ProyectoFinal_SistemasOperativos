# GSEA: Utilidad de Gestión Segura y Eficiente de Archivos
### Proyecto Final - Sistemas Operativos


---

### Funcionalidades Implementadas

-   **Procesamiento Concurrente:** Al procesar un directorio, GSEA lanza un hilo (`pthread`) por cada archivo, permitiendo un procesamiento en paralelo que aprovecha al máximo los sistemas multinúcleo.

-   **Uso Exclusivo de Llamadas al Sistema:** Toda la E/S de archivos (`open`, `read`, `write`) y directorios (`opendir`, `readdir`, `stat`) se realiza a través de la API de bajo nivel de POSIX, interactuando directamente con el kernel del sistema operativo.

-   **Manejo de Archivos y Directorios:** El programa detecta automáticamente si la ruta de entrada es un archivo o un directorio y adapta su comportamiento, procesando un único archivo o todos los archivos dentro de un directorio.

-   **Algoritmo de Compresión Propio:** Se ha implementado el algoritmo **Run-Length Encoding (RLE)** desde cero, optimizado para datos con secuencias de bytes repetitivas.

-   **Algoritmo de Encriptación Propio:** Se ha implementado el **Cifrado Vigenère** desde cero, un cifrado polialfabético clásico que opera a nivel de byte, haciéndolo compatible con cualquier tipo de archivo.

-   **Código Robusto y Seguro para Hilos:** Se han implementado mejoras críticas como el uso de `unsigned char` para aritmética de bytes, validaciones de entrada, manejo de escrituras parciales y un `mutex` para proteger la salida de la consola, garantizando un funcionamiento estable en entornos concurrentes.

---


### Cómo Compilar

El proyecto incluye un `Makefile` que gestiona todo el proceso de compilación. Simplemente ejecuta:

```bash
# Compilar el proyecto 
make

# Para limpiar los archivos compilados
make clean
```

---

### Guía de Uso

La sintaxis general del programa es:
`./gsea <operaciones> --comp-alg <alg> --enc-alg <alg> -i <entrada> -o <salida> -k <clave>`

#### Ejemplo 1: Procesar un solo archivo

1.  **Comprimir y encriptar un archivo:**
    ```bash
    ./gsea -ce --comp-alg rle --enc-alg vigenere -i mi_documento.txt -o mi_documento.dat -k "claveSecreta"
    ```

2.  **Desencriptar y descomprimir el archivo:**
    ```bash
    ./gsea -ud --comp-alg rle --enc-alg vigenere -i mi_documento.dat -o documento_restaurado.txt -k "claveSecreta"
    ```

#### Ejemplo 2: Procesar un directorio completo (Modo Concurrente)

1.  **Crear un directorio de prueba:**
    ```bash
    mkdir ./mis_archivos
    echo "AAAAA" > ./mis_archivos/log1.txt
    echo "BBBBB" > ./mis_archivos/log2.txt
    ```

2.  **Comprimir y encriptar todo el directorio:**
    ```bash
    ./gsea -ce --comp-alg rle --enc-alg vigenere -i ./mis_archivos -o ./archivos_procesados -k "clave_directorio"
    ```

3.  **Desencriptar y descomprimir todo el directorio:**
    ```bash
    ./gsea -ud --comp-alg rle --enc-alg vigenere -i ./archivos_procesados -o ./archivos_restaurados -k "clave_directorio"
    ```