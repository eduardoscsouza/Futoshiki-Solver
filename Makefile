all:
	g++ futoshiki.cpp -O3 -o futoshiki.out

run:
	./futoshiki.out 2

clean:
	rm *.out *.o

test:
	g++ futoshiki.cpp -Wall -Wextra -Wno-ignored-qualifiers -Wno-char-subscripts -O3 -g -o futoshiki.out
	valgrind --leak-check=full --track-origins=yes ./futoshiki.out 2 < in

time:
	./futoshiki.out 0 <in