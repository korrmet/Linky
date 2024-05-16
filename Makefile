FLAGS += $(shell fltk-config --cxxflags)
FLAGS += $(shell fltk-config --ldflags)
FLAGS += $(shell fltk-config --libs)
FLAGS += -DDEBUG
FLAGS += -DVERSION="\"v0.1 alpha\""
FLAGS += -I./Independency
FLAGS += -ldl

run: build
	./linky

build: clean
	g++ app.cpp editor.cpp generator.cpp simulator.cpp -o linky $(FLAGS)

clean:
	rm -rf linky *.o *.so
