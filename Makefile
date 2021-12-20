CC=g++
CFLAGS=-g -pedantic # -Wall -Werror -Wextra
SOURCEDIR = src
BUILDDIR = build
OBJECTS = utils lproto ldump lytbl linstr lobject

LUAS = aequalsix hello localx minimal subfunc tf binop\
 assign subfunc2 move move2 tailcall jmp vararg tfor lgcinstr\
 test1 test2 test3 tab
LUAC = luac5.3
LUASDIR = luas

.PHONY: all
all: luacformatter

luas: $(addsuffix .luac, $(LUAS))

%.luac: $(LUASDIR)/%.lua
	$(LUAC) -o $(LUASDIR)/$@ $^

luacformatter: luacformatter.o $(addsuffix .o, $(OBJECTS))
	$(CC) -o $@ $(addprefix $(BUILDDIR)/,$^) $(CFLAGS)

luacformatter.o: luacformatter.cpp
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)

%.o: $(SOURCEDIR)/%.cpp $(SOURCEDIR)/%.hpp
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(BUILDDIR)/*.o
# 	rm -f $(LUASDIR)/*.luac
	rm -f luacformatter

package:
	tar cJf luacFormatter.tar.xz Makefile build/DONOTREMOVEBUILDDIR src luacformatter.cpp