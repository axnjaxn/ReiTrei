APPNAME = ReiTrei5
ifndef CYGWIN
EXT = 
else
EXT = .exe
endif
APP = $(APPNAME)$(EXT)
DFLAGS = -O2 -DDEBUG -DVERBOSE -g
LFLAGS = `sdl2-config --libs`

OBJECT_FILES = \
	mat4.o \
	randomizer.o \
	ray5grid.o \
	ray5parser.o \
	ray5screen.o \
	pixelrenderer.o \
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

run: $(APP) tricol.ray
	./$(APP) tricol.ray
