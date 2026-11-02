# Configuración del compilador y banderas
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Banderas del sistema necesarias para Raylib en WSL/Linux, 
# apuntando explícitamente a la biblioteca estática local en lib/
LDFLAGS = lib/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

# Archivos fuente y objeto
SRC = $(wildcard src/*.cpp)
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

# Limpieza de binarios y archivos objeto
clean:
	rm -f src/*.o $(TARGET)