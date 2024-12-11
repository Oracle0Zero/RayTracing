vpath %.cpp src
vpath %.h include

main.exec: main.o Light.o Sphere.o
	g++ $^ -o $@ -fopenmp -lsfml-graphics -lsfml-window -lsfml-system -lGL -lGLU -lSOIL -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi;

main.o: main.cpp Sphere.h Light.h
	g++ -c -fopenmp $<

Light.o: Light.cpp Light.h
	g++ -c -fopenmp $<

Sphere.o: Sphere.cpp Sphere.h
	g++ -c -fopenmp $<

