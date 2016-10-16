#include <iostream>
#include <map>
#include <set>
#include <cstdlib>
#include <cstring>

#define LESSER -1
#define GREATER 1

#define GAME_COMPLETED -1
#define KEEP_GOING 0

using namespace std;



class Pos
{
public:
	int i=0, j=0;


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

	const bool operator > (const Pos b) const
	{
		return !((*this < b) || (*this == b));
	}

	const bool operator == (const Pos b) const
	{
		return ((this->i == b.i) && (this->j == b.j));
	}

	const bool operator != (const Pos b) const
	{
		return !(*this==b);
	}
};


class Restriction
{
public:
	Pos p1 = Pos(), p2 = Pos();
	int type = 0;


	Restriction()
	{
		Restriction(Pos(), Pos(), 0);
	}
	
	Restriction(int i1, int j1, int i2, int j2)
	{
		Restriction(i1, j1, i2, j2, 0);
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

	const bool operator > (const Restriction b) const
	{
		return !((*this < b) || (*this == b));
	}

	const bool operator == (const Restriction b) const
	{
		return ((this->p1 == b.p1) && (this->p2 == b.p2));
	}

	const bool operator != (const Restriction b) const
	{
		return !(*this==b);
	}
};


class Remaining
{
public:
	long long * vect = NULL;
	char count = 0;
	
	Remaining()
	{
		this->vect = NULL;
		this-> count = 0;
	}
	
	Remaining(int size)
	{
		this->vect = new long long[size];
		for (int i=0; i<size; i++) vect[i] = 1;
		
		this->count = size;
	}
	
	~Remaining()
	{
		delete[] this->vect;
	}
};


class Futoshiki
{
public:
	int size = 0, setted = 0;
	char heur = 0;
	char ** board = NULL;
	Remaining ** remain = NULL;
	set<Restriction> restricts = set<Restriction>();


	Futoshiki()
	{
		this->size = 0;
		this->setted = 0;
		this->heur = 0;
		this->board = NULL;
		this->remain = NULL;
		this->restricts = set<Restriction>();
	}

	Futoshiki(int size)
	{
		this->size = size;
		this->setted = 0;
		this->heur = 0;
		this->restricts = set<Restriction>();

		this->board = new char*[size];
		for(int i=0; i<size; i++) {
			this->board[i] = new char[size];
			memset(this->board[i], -1, size * sizeof(char));
		}
		
		this->remain = new Remaining*[size];
		for (int i=0; i<size; i++){
			this->remain[i] = new Remaining[size];
			for (int j=0; j<size; j++) this->remain[i][j] = Remaining(size);
		}
	}

	~Futoshiki()
	{
		for (int i=0; i<this->size; i++) delete this->board[i];
		delete this->board;
		
		for (int i=0; i<this->size; i++) {
			for (int j=0; j<this->size; j++) this->remain[i][j].~Remaining();
			delete this->remain[i];
		}
		delete[] this->remain;
	}


	bool valid()
	{
		set<Restriction>::iterator it;
		for (it=this->restricts.begin(); it!=this->restricts.end(); it++){
			if (it->type == LESSER && this->board[it->p1.i][it->p1.j] >= this->board[it->p2.i][it->p2.j]) return false;
			else if (it->type == GREATER && this->board[it->p1.i][it->p1.j] <= this->board[it->p2.i][it->p2.j]) return false;
		}

		bool found[this->size];
		for (int i=0; i<this->size; i++){
			memset(found, false, sizeof(bool)*this->size);
			for (int j=0; j<this->size; j++){
				if (this->board[i][j]!=-1){
					if (found[this->board[i][j]]) return false;
					else found[this->board[i][j]] = true;
				}
			}
		}

		for (int i=0; i<this->size; i++){
			memset(found, false, sizeof(bool)*this->size);
			for (int j=0; j<this->size; j++){
				if (this->board[j][i]!=-1){
					if (found[this->board[j][i]]) return false;
					else found[this->board[j][i]] = true;
				}
			}
		}

		return true;
	}

	bool ended()
	{
		return (this->setted >= this->size * this->size);
	}
	

	void print_board()
	{
		for (int i=0; i<this->size; i++){
			for (int j=0; j<this->size; j++){
				cout<<this->board[i][j]+1;
				if (j != this->size-1) cout<<' ';
			}
			cout<<endl;
		}
	}
};



int no_heur(Futoshiki *, int, int);
int set_possibilities(Futoshiki *, int, int, int, bool);
int foward_checking(Futoshiki *, int, int);
void backtracking(Futoshiki *);



int no_heur(Futoshiki * game, int line, int col)
{
	for (char i=0; i<game->size; i++){
		game->board[line][col] = i;
		if (game->valid()){
			game->setted++;
			if (game->ended()) return GAME_COMPLETED; 

			backtracking(game);
			if (game->ended()) return GAME_COMPLETED;

			game->setted--;
		}
	}
	
	game->board[line][col] = -1;
	return KEEP_GOING;
}

int set_possibilities(Futoshiki * game, int line, int col, int value, bool plus_minus)
{
	bool dead_end = false;
	
	for (int i=0; i<game->size; i++){
		game->remain[line][i].vect[value] += plus_minus ? 1 : -1;
		if (game->remain[line][i].vect[value] < 1) {
			game->remain[line][i].count--;
			if (game->remain[line][i].count <= 0 && game->board[line][i]==-1) dead_end = true;
		}
	}
	
	for (int i=0; i<game->size; i++){
		game->remain[i][col].vect[value] += plus_minus ? 1 : -1;
		if (game->remain[i][col].vect[value] < 1) {
			game->remain[i][col].count--;
			if (game->remain[i][col].count <= 0 && game->board[i][col]==-1) dead_end = true;
		}
	}
	
	set<Restriction>::iterator it;
	Restriction res(line, col, line, col+1);
	
	it = game->restricts.find(res);
	if (it != game->restricts.end()){}
	
	return (!dead_end);
}

int foward_checking(Futoshiki * game, int line, int col)
{
	for (int i=0; i<game->size; i++){
		if (game->remain[line][col].vect[i] >= 1){
			game->board[line][col] = i;
			game->setted++;
			if (game->ended()) return GAME_COMPLETED;
			
			if (set_possibilities(game, line, col, i, false)){
				backtracking(game);
				if (game->ended()) return GAME_COMPLETED;
			}
			set_possibilities(game, line, col, i, true);
			game->setted--;
		}
	}
	
	game->board[line][col] = -1;
	return KEEP_GOING;
}

void backtracking(Futoshiki * game)
{
	for (int i=0; i<game->size; i++){
		for (int j=0; j<game->size; j++){
			if (game->board[i][j] == -1){
				if (game->heur==0){
					if (no_heur(game, i, j) == GAME_COMPLETED) return;
				}
				
				else if(game->heur==1){
					if (foward_checking(game, i, j) == GAME_COMPLETED) return;
				}
			}
		}
	}
}



int main(int argc, char * argv[])
{
	int testCases;
	cin>>testCases;
	for (int k=0; k<testCases; k++){
		int size, nRestricts;
		cin>>size>>nRestricts;

		Futoshiki * game = new Futoshiki(size);
		for(int i=0; i<size; i++) for (int j=0; j<size; j++) {
			int aux;
			cin>>aux;
			
			
			game->board[i][j] = aux - 1;
			if (game->board[i][j] != -1){
				game->setted++;
				//set_possibilities(game, i, j, game->board[i][j], false);
			}
		}
			
		for(int i=0; i<nRestricts; i++){
			int i1=0, j1=0, i2=0, j2=0;
			cin>>i1>>j1>>i2>>j2;
			i1--;i2--;j1--;j2--;

			game->restricts.insert(Restriction(i1, j1, i2, j2, LESSER));
			game->restricts.insert(Restriction(i2, j2, i1, j1, GREATER));
		}
		
		game->heur = 0;
		backtracking(game);
		game->print_board();
		//delete game;
	}


	return 0;
}
