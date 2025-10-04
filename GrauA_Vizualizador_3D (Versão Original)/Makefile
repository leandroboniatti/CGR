# Makefile para Visualizador 3D - CGR
# Compilação sem CMake conforme solicitado

# Configurações do compilador
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -Isrc/ -I.

# Bibliotecas necessárias
LIBS = -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32

# Diretórios
SRCDIR = src
OBJDIR = obj
CPP_SOURCES = $(wildcard $(SRCDIR)/*.cpp) main.cpp
C_SOURCES = $(SRCDIR)/glad.c
CPP_OBJECTS = $(CPP_SOURCES:%.cpp=$(OBJDIR)/%.o)
C_OBJECTS = $(C_SOURCES:%.c=$(OBJDIR)/%.o)
OBJECTS = $(CPP_OBJECTS) $(C_OBJECTS)
TARGET = visualizador3d.exe

# Regra principal
all: $(TARGET)

# Criar diretório de objetos se não existir
$(OBJDIR):
	mkdir $(OBJDIR)
	mkdir $(OBJDIR)\$(SRCDIR)

# Compilar executável
$(TARGET): $(OBJDIR) $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

# Compilar arquivos .cpp para .o
$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar arquivos .c para .o
$(OBJDIR)/%.o: %.c
	gcc $(INCLUDES) -c $< -o $@

# Limpar arquivos compilados
clean:
	rmdir /s /q $(OBJDIR)
	del $(TARGET)

# Executar o programa
run: $(TARGET)
	./$(TARGET)

# Regras auxiliares
.PHONY: all clean run

# Informações sobre dependências
info:
	@echo "=== Dependências necessárias ==="
	@echo "- GLFW 3.x (para gerenciamento de janelas)"
	@echo "- GLAD (incluído no projeto - carregamento de extensões OpenGL)"
	@echo "- GLM (biblioteca header-only para matemática)"
	@echo "- OpenGL 4.0+ drivers"
	@echo ""
	@echo "=== Como instalar no Windows ==="
	@echo "1. Baixe GLFW de https://www.glfw.org/"
	@echo "2. Baixe GLM de https://github.com/g-truc/glm"
	@echo "3. Configure as variáveis de ambiente para incluir as bibliotecas"
	@echo "4. Execute 'make' para compilar o projeto"
	@echo "5. GLAD já está incluído no projeto em src/glad.h e src/glad.c"