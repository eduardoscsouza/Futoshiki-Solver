#include <iostream>
#include <map>
#include <sstream>
#include <climits>
#include <cstring>

#define LESSER -1
#define GREATER 1

#define GAME_COMPLETED 0
#define KEEP_GOING 1

#define STD_HUERISTIC_LEVEL 1

using namespace std;



class Pos
{
public:
	pair<char, char> pos;


	Pos()
	{
		Pos(0, 0);
	}

	Pos(char i, char j)
	{
		this->pos = pair<char, char>(i, j);
	}
	
	inline const char i() const{
		return this->pos.first;
	}
	
	inline const char j() const{
		return this->pos.second;
	}


	const bool operator < (const Pos b) const
	{
		return this->pos < b.pos;
	}
};


class Restriction
{
public:
	pair<Pos, Pos> rest;


	Restriction()
	{
		Restriction(Pos(0, 0), Pos(0 ,0));
	}

	Restriction(Pos p1, Pos p2)
	{
		this->rest = pair<Pos, Pos>(p1, p2);
	}
	
	inline const Pos p1() const
	{
		return this->rest.first;
	}
	
	inline const Pos p2() const
	{
		return this->rest.second;
	}


	const bool operator < (const Restriction b) const
	{
		return this->rest < b.rest;
	}
};


class Remaining
{
public:
	char * vect = NULL;
	char count = 0;
	
	Remaining()
	{
		this->vect = NULL;
		this-> count = 0;
	}
	
	Remaining(char size)
	{
		this->vect = new char[size];
		for (char i=0; i<size; i++) vect[i] = 1;
		
		this->count = size;
	}
	
	/*
	~Remaining()
	{
		delete this->vect;
	}*/
};


class Futoshiki
{
public:
	char size = 0, setted = 0;
	char heur = 0;
	char ** board = NULL;
	Remaining ** remain = NULL;
	map<Restriction, char> restricts = map<Restriction, char>();


	Futoshiki()
	{
		this->size = 0;
		this->setted = 0;
		this->heur = 0;
		this->board = NULL;
		this->remain = NULL;
		this->restricts = map<Restriction, char>();
	}

	Futoshiki(char size)
	{
		this->size = size;
		this->setted = 0;
		this->heur = 0;
		this->restricts = map<Restriction, char>();

		this->board = new char*[size];
		for(char i=0; i<size; i++) {
			this->board[i] = new char[size];
			memset(this->board[i], -1, size * sizeof(char));
		}
		
		this->remain = new Remaining*[size];
		for (char i=0; i<size; i++){
			this->remain[i] = new Remaining[size];
			for (char j=0; j<size; j++) this->remain[i][j] = Remaining(size);
		}
	}
	
	/*
	~Futoshiki()
	{
		for (int i=0; i<this->size; i++) delete this->board[i];
		delete this->board;
		
		for (int i=0; i<this->size; i++) {
			for (int j=0; j<this->size; j++) this->remain[i][j].~Remaining();
			delete this->remain[i];
		}
		delete this->remain;
	}*/


	bool valid()
	{
		map<Restriction, char>::iterator it;
		for (it=this->restricts.begin(); it!=this->restricts.end(); it++){
			Pos p1 = it->first.p1(), p2 = it->first.p2();
			
			if (this->board[p1.i()][p1.j()]!=-1 && this->board[p2.i()][p2.j()]!=-1){
				if (it->second == LESSER && this->board[p1.i()][p1.j()] >= this->board[p2.i()][p2.j()]) return false;
				else if (it->second == GREATER && this->board[p1.i()][p1.j()] <= this->board[p2.i()][p2.j()]) return false;
			}
		}

		bool found[this->size];
		for (char i=0; i<this->size; i++){
			memset(found, false, sizeof(bool)*this->size);
			for (char j=0; j<this->size; j++){
				if (this->board[i][j]!=-1){
					if (found[this->board[i][j]]) return false;
					else found[this->board[i][j]] = true;
				}
			}
		}

		for (char i=0; i<this->size; i++){
			memset(found, false, sizeof(bool)*this->size);
			for (char j=0; j<this->size; j++){
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
		for (char i=0; i<this->size; i++){
			for (char j=0; j<this->size; j++){
				printf("%d", this->board[i][j]+1);
				if (j != this->size-1) printf(" ");
			}
			printf("\n");
		}
	}
};



int no_heur(Futoshiki *, Pos);
int set_possibility(Futoshiki *, Pos, char, bool);
int update_restriction(Futoshiki *, Pos, Pos, char, bool);
int update_possibilities(Futoshiki *, Pos, char, bool);
int foward_checking(Futoshiki *, Pos);
void backtracking(Futoshiki *);



int no_heur(Futoshiki * game, Pos pos)
{
	for (char i=0; i<game->size; i++){
		game->board[pos.i()][pos.j()] = i;
		if (game->valid()){
			game->setted++;
			if (game->ended()) return GAME_COMPLETED; 

			backtracking(game);
			if (game->ended()) return GAME_COMPLETED;

			game->setted--;
		}
	}
	
	game->board[pos.i()][pos.j()] = -1;
	return KEEP_GOING;
}


int set_possibility(Futoshiki * game, Pos pos, char value, bool plus_minus)
{
	if (game->remain[pos.i()][pos.j()].vect[value] == 1 && !plus_minus) game->remain[pos.i()][pos.j()].count--;
	if (game->remain[pos.i()][pos.j()].vect[value] == 0 && plus_minus) game->remain[pos.i()][pos.j()].count++;
	
	game->remain[pos.i()][pos.j()].vect[value] += plus_minus ? 1 : -1;
	return ((game->remain[pos.i()][pos.j()].count <= 0 && game->board[pos.i()][pos.j()]==-1) ? 1 : 0);
}

int update_restriction(Futoshiki * game, Pos p1, Pos p2, char value, bool plus_minus)
{
	if (p2.i() < 0 || p2.i() >=game->size || p2.j() < 0 || p2.j() >= game->size) return 0;
	
	int dead_end = 0;
	Restriction res(p1, p2);
	if (game->restricts.count(res)){
		if (game->restricts[res] == LESSER){
			for (int i=value; i>=0; i--){
				dead_end += set_possibility(game, p2, i, plus_minus);
			}
		}
		else{
			for (int i=value; i<game->size; i++){
				dead_end += set_possibility(game, p2, i, plus_minus);
			}
		}
	}
				
	return dead_end;
}

int update_possibilities(Futoshiki * game, Pos pos, char value, bool plus_minus)
{
	int dead_end = 0;
	
	Pos aux_pos = pos;
	for (int i=0; i<game->size; i++) {
		dead_end += set_possibility(game, Pos(pos.i(), i), value, plus_minus);
		dead_end += set_possibility(game, Pos(i, pos.j()), value, plus_minus);
	}
	
	dead_end += update_restriction(game, pos, Pos(pos.i(), pos.j()+1), value, plus_minus);
	dead_end += update_restriction(game, pos, Pos(pos.i(), pos.j()-1), value, plus_minus);
	dead_end += update_restriction(game, pos, Pos(pos.i()+1, pos.j()), value, plus_minus);
	dead_end += update_restriction(game, pos, Pos(pos.i()-1, pos.j()), value, plus_minus);
	
	return dead_end;
}

int foward_checking(Futoshiki * game, Pos pos)
{
	for (int i=0; i<game->size; i++){
		if (game->remain[pos.i()][pos.j()].vect[i] >= 1){
			game->board[pos.i()][pos.j()] = i;
			game->setted++;
			if (game->ended()) return GAME_COMPLETED;
			
			if (update_possibilities(game, pos, i, false) <= 0){
				backtracking(game);
				if (game->ended()) return GAME_COMPLETED;
			}
			update_possibilities(game, pos, i, true);
			game->setted--;
		}
	}
	
	game->board[pos.i()][pos.j()] = -1;
	return KEEP_GOING;
}


void backtracking(Futoshiki * game)
{
	if (game->heur != 2){
		for (int i=0; i<game->size; i++){
			for (int j=0; j<game->size; j++){
				if (game->board[i][j] == -1){
					if (game->heur==0){
						if (no_heur(game, Pos(i, j)) == GAME_COMPLETED) return;
					}
					
					else{
						if (foward_checking(game, Pos(i, j)) == GAME_COMPLETED) return;
					}
				}
			}
		}
	}
	
	else{
		int min = INT_MAX, min_line = -1, min_col = -1;
		for (int i=0; i<game->size; i++){
			for (int j=0; j<game->size; j++){
				if (game->board[i][j] == -1 && game->remain[i][j].count < min){
					min = game->remain[i][j].count;
					min_line = i;
					min_col = j;
				}
			}
		}
		
		if (min_line==-1 || min_col==-1 || foward_checking(game, Pos(min_line, min_col)) == GAME_COMPLETED) return;
	}
		
}



int main(int argc, char * argv[])
{	
	long testCases;
	scanf("%ld", &testCases);
	for (long k=0; k<testCases; k++){
		char size, nRestricts;
		scanf("%d%d", &size, &nRestricts);

		Futoshiki * game = new Futoshiki(size);
		for(char i=0; i<size; i++){
			for (char j=0; j<size; j++) {
				char aux;
				scanf("%d", &aux);
				
				game->board[i][j] = aux - 1;
			}
		}
		
		for(char i=0; i<nRestricts; i++){
			char i1, j1, i2, j2;
			scanf("%d%d%d%d", &i1, &j1, &i2, &j2);
			i1--;i2--;j1--;j2--;

			game->restricts[Restriction(Pos(i1, j1), Pos(i2, j2))] = LESSER;
			game->restricts[Restriction(Pos(i2, j2), Pos(i1, j1))] = GREATER;
		}
		
		for(char i=0; i<size; i++){
			for (char j=0; j<size; j++) {
				if (game->board[i][j] != -1){
					game->setted++;
					update_possibilities(game, Pos(i, j), game->board[i][j], false);
				}
			}
		}
		
		if (argc == 2) sscanf(argv[1], "%d", &(game->heur));
		else game->heur = STD_HUERISTIC_LEVEL;
		
		printf("%d\n", game->valid());
		backtracking(game);
		game->print_board();
		
		printf("%d\n", game->valid());
		delete game;
	}


	return 0;
}
