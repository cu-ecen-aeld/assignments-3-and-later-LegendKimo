
ifdef CROSS_COMPILE
CC := $(CROSS_COMPILE)gcc
$(info Compiling with cross-compiler: $(CC))

else
CC := gcc
$(info Compiling with host-compiler: $(CC))
endif

all: aesdsocket

aesdsocket :
	$(CC) aesdsocket.c -o aesdsocket

clean:
	rm -rf aesdsocket