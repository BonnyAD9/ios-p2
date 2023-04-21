CC:=cc
TARGET:=proj2
# flags for submit
#CFLAGS:=-std=gnu99 -Wall -Wextra -Werror -pedantic -O2 -DNDEBUG
# flags for debug
CFLAGS:=-g -Wall -std=gnu99 -fsanitize=address -pedantic -Wall -Wextra
RFLAGS:=-std=c17 -DNDEBUG -O3
LDFLAGS:=-pthread -lrt

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
	clang -MM $(SRC) | sed -r 's/^.*$$/obj\/debug\/\0/' > dep.d
	make deb

release:
	mkdir -p obj/release
	clang -MM $(SRC) | sed -r 's/^.*$$/obj\/release\/\0/' > dep.d
	make rel

deb: $(DOBJ)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LDFLAGS)

rel: $(ROBJ)
	$(CC) $(RFLAGS) $^ -o $(TARGET) $(LDFLAGS)

obj/release/%.o: src/%.c
	$(CC) $(RFLAGS) -c -o $@ $<

obj/debug/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

install:
	sudo cp -i $(TARGET) /bin/target

clean:
	-rm obj/debug/*.o obj/release/*.o $(TARGET) dep.d
