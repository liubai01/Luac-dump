CC=g++
CFLAGS=-g -pedantic # -Wall -Werror -Wextra
SOURCEDIR = src
BUILDDIR = build
OBJECTS = utils lproto ldump

LUAS = aequalsix hello localx minimal subfunc
LUAC = luac5.3
LUASDIR = luas

.PHONY: all
all: luaparser

luas: $(addsuffix .luac, $(LUAS))

%.luac: $(LUASDIR)/%.lua
	$(LUAC) -o $(LUASDIR)/$@ $^

luaparser: luaparser.o $(addsuffix .o, $(OBJECTS))
	$(CC) -o $@ $(addprefix $(BUILDDIR)/,$^) $(CFLAGS)

luaparser.o: luaparser.cpp
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)

%.o: $(SOURCEDIR)/%.cpp $(SOURCEDIR)/%.hpp
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(BUILDDIR)/*.o
# 	rm -f $(LUASDIR)/*.luac
	rm -f luaparser