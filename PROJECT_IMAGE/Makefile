MAKEFLAGS += -r

CC := gcc
CFLAGS = -Iinclude -Wall -Wextra -I./iup/include -I./im/include $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = -L./iup -Wl,-rpath=./iup -L./im -Wl,-rpath=./im -Wl,--disable-new-dtags
LDLIBS = -liupim -liup -lim -lim_process $(shell pkg-config --libs gtk+-3.0) -lX11 -lm

SRC := $(shell find . -name "*.c")
OBJS := $(patsubst %.c,%.o,$(SRC))

app: $(OBJS)
	$(CC) $(OBJS) -o $(@) $(CFLAGS) $(LDFLAGS) $(LDLIBS)

%.o:%.c
	$(CC) $(CFLAGS) -c $(<) -o $(@)

clean:
	rm -rf $(OBJS) app