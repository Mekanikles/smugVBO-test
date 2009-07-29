
GLFWHOME=../../../SDK/glfw
CFLAGS=-I$(GLFWHOME)/include -Wall -ffast-math -O3
LFLAGS=-lglfw -L$(GLFWHOME)/lib/win32 -lopengl32


all:
	gcc main.c glee.c $(CFLAGS) $(LFLAGS) -o vbotest.exe -DWIN32 -mno-cygwin
    
clean:
	rm -f vbotest.exe main.o
