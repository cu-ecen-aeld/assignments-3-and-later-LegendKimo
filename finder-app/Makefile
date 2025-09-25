
ifdef CROSS_COMPILE
CC := $(CROSS_COMPILE)gcc
$(info Compiling with cross-compiler: $(CC))

else
CC := gcc
$(info Compiling with host-compiler: $(CC))
endif

all: writer

writer :
	$(CC) writer.c -o writer

clean:
	rm -rf writer