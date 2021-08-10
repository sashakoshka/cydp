all:
	gcc main.c -o cydp -Wall -Wextra -lSDL2 #-Werror

debug:
	gcc main.c -o cydp -Wall -Wextra -lSDL2 -g #-Werror

run: clean all
	./cydp

clean:
	rm -f cydp
