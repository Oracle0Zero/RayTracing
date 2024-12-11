g++ -fopenmp -c main.cpp Sphere.cpp Light.cpp ;
g++ main.o -o sfml-app Sphere.o Light.o -fopenmp -lsfml-graphics -lsfml-window -lsfml-system -lGL -lGLU -lSOIL -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi;
./sfml-app;
