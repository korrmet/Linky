FLAGS += $(shell fltk-config --cxxflags)
FLAGS += $(shell fltk-config --ldflags)
FLAGS += $(shell fltk-config --libs)
FLAGS += -DDEBUG
FLAGS += -I./Independency

run: build
	./linky

build: clean
	g++ app.cpp editor.cpp generator.cpp simulator.cpp -o linky $(FLAGS) -g3 -ggdb

clean:
	rm -rf linky
