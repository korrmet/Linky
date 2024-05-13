FLAGS += $(shell fltk-config --cxxflags)
FLAGS += $(shell fltk-config --ldflags)
FLAGS += $(shell fltk-config --libs)
FLAGS += -DDEBUG
FLAGS += -I./Independency

run: build
	./linky

build: clean
	gcc -c functions.c -o functions.o -g3 -ggdb
	g++ -c harness.cpp -o harness.o -g3 -ggdb
	g++ functions.o harness.o app.cpp editor.cpp generator.cpp simulator.cpp -o linky $(FLAGS) -g3 -ggdb

clean:
	rm -rf linky
