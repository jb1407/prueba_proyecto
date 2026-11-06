# Configuración del compilador y banderas
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -Isrc -Isrc/DESIGNAR/src -Isrc/DESIGNAR/include -Isrc/DESIGNAR

# Banderas del sistema necesarias para Raylib en WSL/Linux, 
# apuntando explícitamente a la biblioteca estática local en lib/
LDFLAGS = lib/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

# Archivos fuente y objeto (excluyendo automáticamente samples y tests de DESIGNAR)
SRC = $(shell find src -name "*.cpp" -not -path "*/samples/*" -not -path "*/tests/*")
OBJ = $(SRC:.cpp=.o)
TARGET = game

# Regla principal
all: $(TARGET)

# Compilación del ejecutable enlazando la librería estática
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(TARGET)

# Regla para compilar los archivos objeto
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para compilar y ejecutar de un solo golpe
run: all
	./$(TARGET)

# Limpieza total de binarios y archivos objeto
clean:
	find src -name "*.o" -delete
	rm -f $(TARGET)