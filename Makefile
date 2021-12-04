CC=g++
CFLAGS=-g -pedantic # -Wall -Werror -Wextra
SOURCEDIR = src
BUILDDIR = build
OBJECTS = utils

.PHONY: all
all: luaparser

luaparser: luaparser.o $(addsuffix .o, $(OBJECTS))
	$(CC) -o $@ $(addprefix $(BUILDDIR)/,$^) $(CFLAGS)

luaparser.o: luaparser.cpp
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)

%.o: $(SOURCEDIR)/%.cpp $(SOURCEDIR)/%.hpp
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(BUILDDIR)/*.o