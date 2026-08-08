# --- Variables
CC = gcc
CFLAGS = -Iinclude -pthread
LDFLAGS = -pthread

# --- Rutas
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include

# Obterner los códgios fuente (ve agregandolos aquí conforme los vayas creando)
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/monitors.c

# de .c a .o
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# --- Ejecutables
# Ubicación del ejecutable principal
TARGET = $(BIN_DIR)/program

# sintaxis para agregar más ejecutables si es necesario:
# TARGETS = $(BIN_DIR)/program $(BIN_DIR)/another_program

# Ejecutar todos los ejecutables (regla por defecto)
all: directories $(TARGET)

# Si la función clear es usada borrará los obj y binarios, con está los crea
directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# de .c a .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# .o a ejecutbles
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)


# --- PHONY 

# >>> Clean borra las carpetas bin y obj con los ejecutables y .o respectivamente
.PHONY: clean
clean:
	@echo "Acabas de borrar los archivos de bin y obj"
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/*

# >>> Run ejecuta main, serverAuth, serverFire, clientAuth, clientFire 
.PHONY: run
run: $(TARGET)
	./$(TARGET)