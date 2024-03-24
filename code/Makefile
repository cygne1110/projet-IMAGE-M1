CC = g++
CPPFLAGS = -Wall -g -Ofast -Wno-write-strings

LIBFLAGS =
LINKFLAGS = 

INCLUDE = -Iinclude

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

ODIR=obj
IDIR=include
SDIR=src

EXEC = ./main.out
RM = rm -f

SOURCES := $(call rwildcard,$(SDIR),*.cpp)
OBJ := $(SOURCES:$(SDIR)/%.cpp=$(ODIR)/%.o)
OBJ += $(ODIR)/main.o

default: $(EXEC)

run: 
	$(EXEC)

install: $(EXEC)

reinstall: clean install

$(EXEC): $(OBJ)
	@$(CC) $(OBJ) -o $@ $(LIBFLAGS) $(LINKFLAGS)

obj/main.o: main.cpp
	@$(CC) -c $(CPPFLAGS) $(LIBFLAGS) $(INCLUDE) $< -o $@

obj/%.o: src/%.cpp
	@$(CC) -c $(CPPFLAGS) $(LIBFLAGS) $(INCLUDE) $< -o $@ 

clean: 
	@$(RM) $(EXEC) obj/*.o
