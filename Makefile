TARGET := corebox
CONFIG := .config
COMMANDS := $(shell ./cmd.sh $(CONFIG))
OS_NAME := '$(shell uname -o)'

ifeq ($(.SHELLSTATUS),0)
CFLAGS := -DOS_NAME=$(OS_NAME) $(CFLAGS)
endif

OBJ := main.o
OBJ += $(addprefix util/, $(addsuffix .o,$(COMMANDS)))
CFLAGS += -I.
CFLAGS += -Wall -Wextra

# directory to install to
PREFIX ?= /usr/local/bin

all: $(TARGET)

$(TARGET): GEN.h $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(CFLAGS) $(LDFLAGS)

gen: gen.c
main.o: main.c GEN.h

GEN.h: gen $(CONFIG)
	./gen $(COMMANDS) > GEN.h

install: $(TARGET)
	./install.sh $(TARGET) '$(PREFIX)'

clean:
	$(RM) -f util/*.o *.o $(TARGET) gen

.PHONY: all clean install
