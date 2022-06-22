default:
	g++ main.cpp -o game -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows