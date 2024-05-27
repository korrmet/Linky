platform = linux
ifeq ($(platform),linux)
CXX = g++
COMPILE_FLAGS += $(shell fltk-config --cxxflags)
COMPILE_FLAGS += -DLINUX
LINK_FLAGS    += $(shell fltk-config --ldflags)
LINK_FLAGS    += $(shell fltk-config --libs)
LINK_FLAGS    += -ldl
else ifeq ($(platform),windows)
CXX = mingw-w64-gcc
COMPILE_FLAGS += $(shell mingw-w64-fltk-config --cxxflags)
COMPILE_FLAGS += -DWINDOWS
LINK_FLAGS    += $(shell mingw-w64-fltk-config --ldflags)
LINK_FLAGS    += $(shell mingw-w64-fltk-config --libs)
endif

build_type = release
ifeq ($(build_type),debug)
COMPILE_FLAGS += -DDEBUG
endif
COMPILE_FLAGS += -DVERSION="\"v0.1 alpha\""
COMPILE_FLAGS += -I./Independency

linky: app.o editor.o generator.o simulator.o
	$(CXX) app.o editor.o generator.o simulator.o -o linky $(LINK_FLAGS)

run: linky
	./linky

app.o: app.cpp app.hpp
	$(CXX) -c app.cpp -o app.o $(COMPILE_FLAGS)

editor.o: editor.cpp editor.hpp
	$(CXX) -c editor.cpp -o editor.o $(COMPILE_FLAGS)

generator.o: generator.cpp generator.hpp
	$(CXX) -c generator.cpp -o generator.o $(COMPILE_FLAGS)

simulator.o: simulator.cpp simulator.hpp
	$(CXX) -c simulator.cpp -o simulator.o $(COMPILE_FLAGS)

app.cpp:
app.hpp:
editor.cpp:
editor.hpp:
generator.cpp:
generator.hpp:
simulator.cpp:
simulator.hpp:

clean:
	rm -rf linky *.o examples/*.h examples/*.c examples/*.so
