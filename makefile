all:
	g++ -Iinclude/sdl2 -Iinclude/headers -Llib -o build/8964.exe src/*.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image