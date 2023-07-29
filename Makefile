CPP = g++

all: prep wozemu

prep:
	mkdir -p build

wozemu: src/*.cpp
	$(CPP) $^ -Ofast -o build/$@