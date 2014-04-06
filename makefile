APPNAME = ReiTrei

LFLAGS = `sdl2-config --libs`
DFLAGS = -O2 -DDEBUG -DVERBOSE -Wno-unused-result
EXT = 

ifndef MINGW
LFLAGS += 
DFLAGS += 
OBJECT_FILES += zipreader.o
else
LFLAGS += -lmingw32 -mwindows -lSDL2main -static-libgcc -static-libstdc++
DFLAGS += -DMINGW -DNO_STDIO_REDIRECT
EXT = .exe
endif

ifndef NO_MAGICK
LFLAGS += `Magick++-config --libs`
DFLAGS += `Magick++-config --cppflags`
else
DFLAGS += -DNO_MAGICK
endif

APP = $(APPNAME)$(EXT)

OBJECT_FILES = \
	mat4.o \
	randomizer.o \
	triangle.o \
	mesh.o \
	grid.o \
	ray5parser.o \
	texture.o \
	pixelrenderer.o \
	renderqueue.o \
	main.o

all: $(APP)

rebuild: clean $(APP)

.cpp.o:
	$(CXX) -c $< $(DFLAGS)

$(APP): $(OBJECT_FILES)
	$(CXX) $(OBJECT_FILES) -o $(APP) $(LFLAGS) $(DFLAGS)

clean:
	rm -f *~ *.o $(APP) Thumbs.db *.stackdump

run: $(APP) tricol.ray
	./$(APP) tricol.ray --point-lights