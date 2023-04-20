CC:=cc
TARGET:=proj2
#CFLAGS:=-std=gnu99 -Wall -Wextra -Werror -pedantic
CFLAGS:=-g -Wall -std=gnu99 -fsanitize=address -pedantic -Wall -Wextra
RFLAGS:=-std=c17 -DNDEBUG -O3

SRC:=$(wildcard src/*.c)
DOBJ:=$(patsubst src/%.c, obj/debug/%.o, $(SRC))
ROBJ:=$(patsubst src/%.c, obj/release/%.o, $(SRC))

-include dep.d

.DEFAULT_GOAL:=debug

.PHONY: debug
.PHONY: release
.PHONY: install
.PHONY: clean
.PHONY: rel
.PHONY: deb


debug:
	mkdir -p obj/debug
	clang $(CFLAGS) -MM $(SRC) | sed -r 's/^.*$$/obj\/debug\/\0/' > dep.d
	make deb

release:
	mkdir -p obj/release
	clang $(CFLAGS) -MM $(SRC) | sed -r 's/^.*$$/obj\/release\/\0/' > dep.d
	make rel

deb: $(DOBJ)
	$(CC) $(CFLAGS) $^ -o $(TARGET)

rel: $(ROBJ)
	$(CC) $(RFLAGS) $^ -o $(TARGET)

obj/release/%.o: src/%.c
	$(CC) $(RFLAGS) -c -o $@ $<

obj/debug/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

install:
	sudo cp -i $(TARGET) /bin/target

clean:
	rm obj/debug/*.o || true
	rm obj/release/*.o || true
	rm $(TARGET) || true
