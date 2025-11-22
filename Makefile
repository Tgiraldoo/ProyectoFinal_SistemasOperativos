# Compilador de C++
CXX = g++

# Banderas de compilación: -std=c++11 como en el avance, -Wall para advertencias, -g para depuración
CXXFLAGS = -std=c++11 -Wall -g

# Banderas del enlazador: -lpthread es CRUCIAL para enlazar la librería POSIX Threads
LDFLAGS = -lpthread

# Nombre del ejecutable final
TARGET = gsea

# Lista de todos los archivos fuente (.cpp)
SOURCES = main.cpp file_manager.cpp algorithms.cpp

# Genera automáticamente la lista de archivos objeto (.o) a partir de los fuentes
OBJECTS = $(SOURCES:.cpp=.o)

# Regla principal: compila el ejecutable final
all: $(TARGET)

# Regla para enlazar los archivos objeto y crear el ejecutable
# Nota la adición de $(LDFLAGS) al final
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Regla para compilar cada archivo .cpp en un archivo .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets no son nombres de archivos
.PHONY: all clean