all:
	g++ futoshiki.cpp -O3 -o futoshiki.out

run:
	./futoshiki.out 

clean:
	rm *.out *.o

test:
	g++ futoshiki.cpp -Wall -Wextra -O3 -g -o futoshiki.out
	valgrind --leak-check=full --track-origins=yes ./futoshiki.out  < in