all: tumls

tumls: tumls.c
	gcc -o tumls tumls.c -Wall -Werror

clean:
	rm -f tumls
