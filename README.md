# GSEA: Utilidad de Gestión Segura y Eficiente de Archivos

### Funcionalidades Implementadas (Avance)

-   **Análisis de Argumentos:** El programa interpreta correctamente las operaciones (`-c`, `-d`, `-e`, `-u`), los algoritmos (`--comp-alg`, `--enc-alg`), las rutas de entrada/salida (`-i`, `-o`) y la clave de encriptación (`-k`).
-   **Gestión de Archivos con Llamadas al Sistema:** Toda la manipulación de archivos (lectura y escritura) se realiza a través de llamadas al sistema POSIX (`open`, `read`, `write`, `close`).
-   **Algoritmo de Compresión Propio:** Se ha implementado el algoritmo **Run-Length Encoding (RLE)**
-   **Algoritmo de Encriptación Propio:** Se ha implementado el **Cifrado Vigenère** 
-   **Operaciones Combinadas:** El programa puede encadenar operaciones, como comprimir y luego encriptar (`-ce`), o desencriptar y luego descomprimir (`-ud`).


Para compilar el proyecto

```bash
make
```
Esto generará un ejecutable llamado `gsea`.


### Uso

La sintaxis general del programa es:
`./gsea <operaciones> --comp-alg <alg> --enc-alg <alg> -i <entrada> -o <salida> -k <clave>`

**Ejemplo 1: Comprimir y encriptar un archivo**
```bash
./gsea -ce --comp-alg rle --enc-alg vigenere -i documento.txt -o documento.dat -k clave
```

**Ejemplo 2: Desencriptar y descomprimir un archivo**
```bash
./gsea -ud --comp-alg rle --enc-alg vigenere -i documento.dat -o documento_restaurado.txt -k clave
```


### Para la Entrega Final

Las funcionalidades que se añadirán son:

-   **Manejo de Directorios** 
-   **Modelo de Concurrencia** 
-   **Algoritmos Avanzados**