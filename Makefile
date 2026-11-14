# Configuración del compilador y banderas
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -Isrc -Isrc/DESIGNAR/include -Isrc/DESIGNAR/src -Isrc/DESIGNAR

# Banderas del sistema necesarias para Raylib en WSL/Linux
LDFLAGS = lib/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

# Ahora que DESIGNAR está dentro de src/, el find detectará todos sus .cpp automáticamente
SRC = $(shell find src -name "*.cpp" -not -path "*/samples/*" -not -path "*/tests/*")
OBJ = $(SRC:.cpp=.o)
TARGET = game

# Regla principal
all: $(TARGET)

# Compilación del ejecutable enlazando todo
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(TARGET)

# Regla genérica para compilar todos los .cpp (incluyendo los de la librería integrada)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para compilar y ejecutar de un solo golpe
run: all
	./$(TARGET)

# Limpieza total de binarios y archivos objeto
clean:
	find src -name "*.o" -delete
	rm -f $(TARGET)