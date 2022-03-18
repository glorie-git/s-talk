
all:
	gcc -g -D _POSIX_C_SOURCE=200809L -Wall -Werror output.c transmitter.c input.c receiver.c general.c list.c main.c -o s-talk -lpthread

clean:
	rm -f s-talk