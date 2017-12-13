
# 
CFLAGS := ${CFLAGS} -I.

# List all object files
COMMON_OBJECTS := base/cmdline.o base/socket.o
SERVER_OBJECTS := server/main.o
CLIENT_OBJECTS := client/main.o

OBJECTS := ${COMMON_OBJECTS} ${SERVER_OBJECTS} ${CLIENT_OBJECTS}

# These are the executables we're building
PRODUCTS := bs_server bs_client

# Phony targets for all and clean
.PHONY: all clean

all: ${PRODUCTS}

clean:
	rm -f ${OBJECTS} ${PRODUCTS}

# Executable generation
bs_server: ${COMMON_OBJECTS} ${SERVER_OBJECTS}
	${CC} -o $@ $^ ${CFLAGS} ${LDFLAGS}

bs_client: ${COMMON_OBJECTS} ${CLIENT_OBJECTS}
	${CC} -o $@ $^ ${CFLAGS} ${LDFLAGS}

# How to make a .o file from a .c file
%.o: %.c
	${CC} -o $@ -c $^ ${CFLAGS}
