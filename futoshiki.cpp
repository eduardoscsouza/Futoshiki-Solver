#include <iostream>
#include <map>
#include <cstdlib>

#define LESSER -1
#define GREATER 1

using namespace std;



class Pos
{
public:
	int i, j;


	Pos()
	{
		Pos(0, 0);
	}

	Pos(int i, int j)
	{
		this->i = i;
		this->j = j;
	}

	const bool operator < (const Pos b) const
	{
		if (this->i != b.i) return this->i > b.i;
		else return this->j > b.j;
	}

	const bool operator == (const Pos b) const
	{
		return (this->i == b.i) && (this->j == b.j);
	}

	const bool operator > (const Pos b) const
	{
		return !((*this < b) || (*this == b));
	}

	const bool operator != (const Pos b) const
	{
		return !(*this==b);
	}
};

class Restriction
{
public:
	Pos p1, p2;
	int type;

	Restriction()
	{
		Restriction(Pos(), Pos(), 0);
	}

	Restriction(int i1, int j1, int i2, int j2, int type)
	{
		Restriction(Pos(i1, j1), Pos(i2, j2), type);
	}

	Restriction(Pos p1, Pos p2, int type)
	{
		this->p1 = p1;
		this->p2 = p2;
		this->type = type;
	}

	const bool operator < (const Restriction b) const
	{
		if (this->p1 != b.p1) return this->p1 > b.p1;
		else return this->p2 > b.p2;
	}
};


class Board
{
public:
	int size;
	char ** table;
	map<pair<Pos, Pos>, int> restricts; 

	Board()
	{
		this->size = 0;
		this->table = NULL;
		this->restricts = map<pair<Pos, Pos>, int>();
	}

	Board(int size)
	{
		this->size = size;
		this->restricts = map<pair<Pos, Pos>, int>();

		this->table = new char*[size];
		for(int i=0; i<size; i++) {
			this->table[i] = new char[size];
			for (int j=0; j<size; j++) this->table[i][j] = 0;
		}
	}

	~Board()
	{
		for (int i=0; i<this->size; i++) delete[] this->table[i];
		delete[] this->table;
	}
};



int main(int argc, char * argv[])
{
	
	int testCases;
	cin>>testCases;
	for (int k=0; k<testCases; k++){
		int size, nRestricts;
		cin>>size>>nRestricts;

		Board board(size);
		for(int i=0; i<size; i++) for (int j=0; j<size; j++) cin>>board.table[i][j];
			
		for(int i=0; i<nRestricts; i++){
			int i1, j1, i2, j2;
			cin>>i1>>j1>>i2>>j2;
			i1--;i2--;j1--;j2--;

			board.restricts[pair<Pos, Pos>(Pos(i1, j1), Pos(i2, j2))] = LESSER;
			board.restricts[pair<Pos, Pos>(Pos(i2, j2), Pos(i1, j1))] = GREATER;
		}
	}


	return 0;
}