vpath %.cpp src
vpath %.h include

CC = g++
CPPFLAGS = -fopenmp
OUTPUT_OPTION = -o $@
LOADLIBES = -lsfml-graphics -lsfml-window -lsfml-system -lGL -lGLU -lSOIL -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi

main.exec: main.o Light.o Sphere.o
	$(CC) $^ $(CPPFLAGS) $(LOADLIBES) -o $@
	rm -r *.o

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< $(OUTPUT_OPTION)