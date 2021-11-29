PROG = sample
CFLAGS = -pthread -g -O2 -Wall
LDFLAGS += -pthread
# CFLAGS = -std=c99 -g -O2 -Wall
# LDLIBS += -L/usr/local/lib -lmylib

all: $(PROG)
OBJS += $(PROG).o
OBJS += set_cpu.o
OBJS += get_num.o
$(PROG): $(OBJS)

clean:
	rm -f *.o $(PROG)
