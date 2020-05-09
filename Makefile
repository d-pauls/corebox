TARGET := corebox
CONFIG := .config
COMMANDS := $(shell ./cmd.sh $(CONFIG))
OBJ := main.o
OBJ += $(addprefix util/, $(addsuffix .o,$(COMMANDS)))
CFLAGS += -I.
CFLAGS += -Wall -Wextra

all: $(TARGET)

$(TARGET): GEN.h $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(CFLAGS) $(LDFLAGS)

gen: gen.c
main.o: main.c GEN.h

GEN.h: gen $(CONFIG)
	./gen $(COMMANDS) > GEN.h

clean:
	rm -f util/*.o *.o $(TARGET) gen

.PHONY: all clean
