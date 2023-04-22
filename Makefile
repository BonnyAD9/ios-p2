CC:=cc
TARGET:=proj2
# flags for submit
CFLAGS:=-std=gnu99 -Wall -Wextra -Werror -pedantic -O2 -DNDEBUG
# flags for debug
#CFLAGS:=-g -Wall -std=gnu99 -fsanitize=address -pedantic -Wall -Wextra
LDFLAGS:=-pthread -lrt
SUBNAME:=xstigl00

SRC:=$(wildcard src/*.c)
DOBJ:=$(patsubst src/%.c, obj/debug/%.o, $(SRC))

-include dep.d

.DEFAULT_GOAL:=debug

.PHONY: debug clean deb


debug:
	mkdir -p obj/debug
	clang -MM $(SRC) | sed -r 's/^.*$$/obj\/debug\/\0/' > dep.d
	make deb

deb: $(DOBJ)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LDFLAGS)

obj/debug/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm obj/debug/*.o $(TARGET) dep.d

submit:
	zip $(SUBNAME).zip src/*
