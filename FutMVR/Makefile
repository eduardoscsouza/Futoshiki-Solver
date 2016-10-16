all:
	g++ futoshiki.cpp -std=c++11 -O3 -o futoshiki.out

run:
	./futoshiki.out 

clean:
	rm *.out *.o

test:
	g++ futoshiki.cpp -Wall -Wextra -Wno-ignored-qualifiers -std=c++11 -O3 -g -o futoshiki.out
	valgrind --leak-check=full --track-origins=yes ./futoshiki.out 2 < in

time:
	g++ futoshiki.cpp --std=c++11 -O3 -o futoshiki.out
	time ./futoshiki.out 1 < in
	@echo "\n"