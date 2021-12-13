all: xsave_test xcr0 xinuse vzero

CXXFLAGS=-O2 -g -mavx -fno-builtin -MMD
ASFLAGS=-g
LINK.o=g++

xsave_test: stdlib.o xsave_test.o asm.o
xcr0: xcr0.o
xinuse: xinuse.o
vzero: vzero.o

clean:
	rm -f *.o xsave_test *~ *.d

-include *.d
