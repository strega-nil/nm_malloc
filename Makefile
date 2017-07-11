CC = clang
# Definitely requires GCC or Clang
CFLAGS = -Wall \
	-Wextra \
	-Wunreachable-code \
	-Wno-deprecated-declarations \
	-Wno-gnu-flexible-array-initializer \
	-pedantic \
	-std=c99 \
	-g

obs = obj/malloc.o \
	obj/test.o

all: malloc test
	@$(CC) $(CFLAGS) -o malloc_test $(obs)

malloc: obj
	@$(CC) $(CFLAGS) -c -o obj/malloc.o src/malloc.c

test: obj
	@$(CC) $(CFLAGS) -c -o obj/test.o src/test.c

obj:
	mkdir -p obj
