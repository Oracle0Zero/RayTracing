g++ -c main.cpp Sphere.cpp Light.cpp;
g++ main.o -o sfml-app Sphere.o Light.o -lsfml-graphics -lsfml-window -lsfml-system;
./sfml-app;
