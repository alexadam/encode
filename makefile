debug:
	gcc -g -pedantic -Wall -Wextra -std=c99 -o e encode.c -lm
	gcc -g -pedantic -Wall -Wextra -std=c99 -o g gol-encode.c -lm
build:
	gcc -O3 -pedantic -Wall -Wextra -std=c99 -o e encode.c -lm
	gcc -O3 -pedantic -Wall -Wextra -std=c99 -o g gol-encode.c -lm