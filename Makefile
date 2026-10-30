# Configuración del compilador y banderas
CXX = g++
CXXFLAGS = -Wall -std=c++17 -O2

# Librerías de Raylib para Linux / WSL
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Nombre del archivo ejecutable final
TARGET = game

# Archivos fuente del proyecto (aquí es donde agregas tus módulos nuevos)
SRCS = main.cpp enemyAI.cpp
OBJS = $(SRCS:.cpp=.o)

# Regla principal por defecto
all: $(TARGET)

# Enlazar los objetos para crear el ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# Regla genérica para compilar cualquier archivo .cpp en su respectivo .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Atajo para compilar y ejecutar el juego de inmediato
run: all
	./$(TARGET)

# Limpiar archivos compilados (objetos y ejecutable)
clean:
	rm -f $(OBJS) $(TARGET)