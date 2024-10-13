all:
	g++ -Isrc/Include -Lsrc/lib -o main main.cpp sdl_manager.cpp render.cpp input.cpp countdown.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf	
	
run:
	./main
