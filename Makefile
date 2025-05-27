COMPILER=g++
CSTD=c++17
CPPFLAGS=-g -Wall -Wextra -Wpedantic -Wconversion -Wnull-dereference
LINKER=-lmingw32 -lSDL2main -lSDL2
LINKERSHARE=-static-libgcc -static-libstdc++ -mwindows -lm -lkernel32 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid -ladvapi32 -lsetupapi -lshell32 -ldinput8 -Wl,--enable-auto-import
INCLUDE=-I.\SDL2\include
LIB=-L.\SDL2\lib

COMPILE=$(COMPILER) *.cpp -std=$(CSTD) $(INCLUDE) $(LIB) $(LINKER) $(CPPFLAGS) -o pong
EXPORT = $(COMPILER) pong.cpp -std=$(CSTD) $(INCLUDE) $(LIB) $(LINKER) -static $(LINKERSHARE) -o pong 

run: build
	@.\pong.exe

export:
	$(EXPORT)
	@echo Program exported successfully.

build:
	@echo Starting build...
	$(COMPILE)
	@echo Build finished successfully.

clean:
	rm .\pong.exe
	@echo Cleaned.
