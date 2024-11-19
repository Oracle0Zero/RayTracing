g++ -c raytracing.cpp Sphere.cpp Light.cpp;
g++ raytracing.o -o sfml-app Sphere.o Light.o -lsfml-graphics -lsfml-window -lsfml-system;
./sfml-app;
