all: spacefilling

spacefilling: src/spacefilling.cpp
	g++ -g -std=c++17 -Wall -Wfatal-errors -o spacefilling src/spacefilling.cpp -lGL -lGLU -lglut
