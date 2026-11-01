# Configuración del compilador y banderas
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Bibliotecas necesarias para Raylib en Linux / WSL
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Archivos fuente y objeto
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = game

# Regla principal
all: $(TARGET)

# Compilación del ejecutable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Regla para compilar los archivos objeto
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para compilar y ejecutar de un solo golpe
run: all
	./$(TARGET)

# Limpieza de binarios y archivos objeto
clean:
	rm -f src/*.o $(TARGET)