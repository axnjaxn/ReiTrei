APPNAME = ReiTrei5
ifndef CYGWIN
EXT = 
else
EXT = .exe
endif
APP = $(APPNAME)$(EXT)
DFLAGS = -g -DDEBUG -DVERBOSE
LFLAGS = `sdl-config --libs`

OBJECT_FILES = \
	mat4.o \
	ray4grid.o \
	ray4parser.o \
	ray5screen.o \
	main.o

all: $(APP)

rebuild: clean $(APP)

.cpp.o:
	g++ -c $< $(DFLAGS)

$(APP): $(OBJECT_FILES)
	g++ $(OBJECT_FILES) -o $(APP) $(LFLAGS) $(DFLAGS)

clean:
	rm -f *~ *.o $(APP) Thumbs.db *.stackdump

debug: $(APP)
	gdb $(APP)

run: $(APP) test.ray
	./$(APP) test.ray

tricol.bmp: $(APP) tricol.ray
	./$(APP) -b 0.1 -e 0.25 -n 1e6 tricol.ray
	mv out.bmp tricol.bmp

test.bmp: $(APP) test.ray
	./$(APP) -b 0.1 -e 0.25 -n 1e6 test.ray
	mv out.bmp test.bmp
