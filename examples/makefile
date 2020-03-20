CPP=clang++
CPPFLAGS=-std=c++11
LIBS=-lsdl2 -lportaudio -lsndfile -framework OpenGl -lrtmidi

# Uncomment this if you want to debug your app.
CPPFLAGS+=-g

# Uncomment this if you want to optimize your app.
# CPPFLAGS+=-O3

# Edit these variables to suit your application.
SOURCES=$(wildcard ../pal-fds/*.cpp)
OBJECTS=$(patsubst %.cpp, %.o, $(SOURCES))
DEPS := $(OBJECTS:.o=.d)

# pal specific variables.
PAL_SOURCES=$(wildcard pal/pal/*.cpp) $(wildcard pal/pal/**/*.cpp)
PAL_HEADERS=$(wildcard pal/pal/*.h) $(wildcard pal/pal/**/*.h)
PAL_OBJECTS=$(patsubst %.cpp, %.o, $(PAL_SOURCES))
PAL_DEPS := $(PAL_OBJECTS:.o=.d)

ZITA_SOURCES=$(wildcard zita-rev1/*.cc)

all: Model1dExample ReedExample PitchExample Fingers Fingers2

PitchExample: PitchExample.cpp $(OBJECTS) $(PAL_OBJECTS) ../Munola.h
	$(CPP) $(CPPFLAGS) $(PAL_OBJECTS) $(OBJECTS) $(LIBS) $< -o $@

Model1dExample: Model1dExample.cpp $(OBJECTS) $(PAL_OBJECTS) ../Munola.h
	$(CPP) $(CPPFLAGS) $(PAL_OBJECTS) $(OBJECTS) $(LIBS) $< -o $@

ReedExample: ReedExample.cpp $(OBJECTS) $(PAL_OBJECTS)
	$(CPP) $(CPPFLAGS) $(PAL_OBJECTS) $(OBJECTS) $(LIBS) $< -o $@

selector_test: selector_test.cpp $(OBJECTS) $(PAL_OBJECTS)
	$(CPP) $(CPPFLAGS) $(PAL_OBJECTS) $(OBJECTS) $(LIBS) $< -o $@

Fingers: Fingers.cpp $(OBJECTS) $(PAL_OBJECTS)
	$(CPP) $(CPPFLAGS) $(PAL_OBJECTS) $(OBJECTS) $(LIBS) $< -o $@

Fingers2: Fingers2.cpp $(OBJECTS) $(PAL_OBJECTS) $(ZITA_SOURCES)
	$(CPP) $(CPPFLAGS) $(ZITA_SOURCES) $(PAL_OBJECTS) $(OBJECTS) $(LIBS) $< -o $@

%.o : %.cpp
	$(CPP) $(CPPFLAGS) -c -MMD -MP $< -o $@

.phony: clean run

run: all
	./$(BIN)

clean:
	rm -f $(PAL_OBJECTS)
	rm -f $(PAL_DEPS)
	rm -f $(OBJECTS)
	rm -f $(DEPS)
	rm -f $(BIN)

-include $(PAL_DEPS)
-include $(DEPS)