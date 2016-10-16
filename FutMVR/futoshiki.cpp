#include <iostream>
#include <map>
#include <utility>
#include <sstream>
#include <climits>
#include <cstdlib>
#include <cstring>

#define LESSER -1
#define GREATER 1

#define GAME_COMPLETED -1
#define KEEP_GOING 0

#define STD_HEURISTIC_LEVEL 2

using namespace std;



class Pos
{
public:
	pair<int, int> pos;


	Pos()
	{
		Pos(0, 0);
	}

	Pos(int i, int j)
	{
		this->pos = pair<int, int>(i, j);
	}
	
	const int i() const{
		return this->pos.first;
	}
	
	const int j() const{
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
	/*
	Restriction(int i1, int j1, int i2, int j2)
	{
		Restriction(Pos(i1, j1), Pos(i2, j2));
	}*/

	Restriction(Pos p1, Pos p2)
	{
		this->rest = pair<Pos, Pos>(p1, p2);
	}
	
	const Pos p1() const
	{
		return this->rest.first;
	}
	
	const Pos p2() const
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
	
	/*
	~Remaining()
	{
		delete this->vect;
	}*/
};


class Futoshiki
{
public:
	int size = 0, setted = 0;
	char heur = 0;
	char ** board = NULL;
	Remaining ** remain = NULL;
	map<Restriction, int> restricts = map<Restriction, int>();


	Futoshiki()
	{
		this->size = 0;
		this->setted = 0;
		this->heur = 0;
		this->board = NULL;
		this->remain = NULL;
		this->restricts = map<Restriction, int>();
	}

	Futoshiki(int size)
	{
		this->size = size;
		this->setted = 0;
		this->heur = 0;
		this->restricts = map<Restriction, int>();

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
		map<Restriction, int>::iterator it;
		for (it=this->restricts.begin(); it!=this->restricts.end(); it++){
			Pos p1 = it->first.p1(), p2 = it->first.p2();
			
			if (it->second == LESSER && this->board[p1.i()][p1.j()] >= this->board[p2.i()][p2.j()]) return false;
			else if (it->second == GREATER && this->board[p1.i()][p1.j()] <= this->board[p2.i()][p2.j()]) return false;
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
				printf("%d", this->board[i][j] + 1);
				if (j != this->size-1) cout<<' ';
			}
			cout<<endl;
		}
	}
};



int no_heur(Futoshiki *, int, int);
int set_possibility(Futoshiki *, int, int, int, bool);
int update_restriction(Futoshiki *, int, int, int, int, int, bool);
int update_possibilities(Futoshiki *, int, int, int, bool);
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


int set_possibility(Futoshiki * game, int line, int col, int value, bool plus_minus)
{
	if (game->remain[line][col].vect[value] == 1 && !plus_minus) game->remain[line][col].count--;
	if (game->remain[line][col].vect[value] == 0 && plus_minus) game->remain[line][col].count++;
	
	game->remain[line][col].vect[value] += plus_minus ? 1 : -1;
	return ((game->remain[line][col].count <= 0 && game->board[line][col]==-1) ? 1 : 0);
}

int update_restriction(Futoshiki * game, int line, int col, int line_2, int col_2, int value, bool plus_minus)
{
	if (line_2 < 0 || line_2 >=game->size || col_2 < 0 || col_2 >=game->size) return 0;
	
	int dead_end = 0;
	Restriction res(Pos(line, col), Pos(line_2, col_2));
	if (game->restricts.count(res)){
		if (game->restricts[res] == LESSER){
			for (int i=value; i>=0; i--){
				dead_end += set_possibility(game, line_2, col_2, i, plus_minus);
			}
		}
		else{
			for (int i=value; i<game->size; i++){
				dead_end += set_possibility(game, line_2, col_2, i, plus_minus);
			}
		}
	}
				
	return dead_end;
}

int update_possibilities(Futoshiki * game, int line, int col, int value, bool plus_minus)
{
	int dead_end = 0;
	
	for (int i=0; i<game->size; i++) {
		dead_end += set_possibility(game, line, i, value, plus_minus);
		dead_end += set_possibility(game, i, col, value, plus_minus);
	}
	
	dead_end += update_restriction(game, line, col, line, col+1, value, plus_minus);
	dead_end += update_restriction(game, line, col, line, col-1, value, plus_minus);
	dead_end += update_restriction(game, line, col, line+1, col, value, plus_minus);
	dead_end += update_restriction(game, line, col, line-1, col, value, plus_minus);
	
	return dead_end;
}

int foward_checking(Futoshiki * game, int line, int col)
{
	for (int i=0; i<game->size; i++){
		if (game->remain[line][col].vect[i] >= 1){
			game->board[line][col] = i;
			game->setted++;
			if (game->ended()) return GAME_COMPLETED;
			
			if (update_possibilities(game, line, col, i, false) == 0){
				backtracking(game);
				if (game->ended()) return GAME_COMPLETED;
			}
			update_possibilities(game, line, col, i, true);
			game->setted--;
		}
	}
	
	game->board[line][col] = -1;
	return KEEP_GOING;
}


void backtracking(Futoshiki * game)
{
	if (game->heur== 0){
		for (int i=0; i<game->size; i++){
			for (int j=0; j<game->size; j++){
				if (game->board[i][j] == -1){
					if (no_heur(game, i, j) == GAME_COMPLETED) return;
					else cout<<"volta"<<endl;
				}
			}
		}
	}

	else{
		if (game->heur == 1){
			for (int i=0; i<game->size; i++){
				for (int j=0; j<game->size; j++){
					if (game->board[i][j] == -1 && game->remain[i][j].count > 0){
						if (foward_checking(game, i, j) == GAME_COMPLETED) return;
						else cout<<"volta"<<endl;
					}
				}
			}
		}

		else{
			int min = INT_MAX, min_line = -1, min_col = -1;
			for (int i=0; i<game->size; i++){
				for (int j=0; j<game->size; j++){
					if (game->board[i][j] == -1 && game->remain[i][j].count > 0 && game->remain[i][j].count < min){
						min = game->remain[i][j].count;
						min_line = i;
						min_col = j;
					}
				}
			}
			
			if (min_line==-1 || min_col==-1 || foward_checking(game, min_line, min_col) == GAME_COMPLETED) return;
			else cout<<"volta"<<endl;
		}
	}
		
}



int main(int argc, char * argv[])
{	
	int testCases;
	cin>>testCases;
	for (int k=0; k<testCases; k++){
		int size, nRestricts;
		scanf("%d%d", &size, &nRestricts);

		Futoshiki * game = new Futoshiki(size);
		for(char i=0; i<size; i++){
			for (char j=0; j<size; j++) {
				char aux;
				scanf("%d", &aux);

				game->board[i][j]=(aux - 1);
			}
		}
		
		for(int i=0; i<nRestricts; i++){
			int i1=0, j1=0, i2=0, j2=0;
			cin>>i1>>j1>>i2>>j2;
			i1--;i2--;j1--;j2--;

			game->restricts[Restriction(Pos(i1, j1), Pos(i2, j2))] = LESSER;
			game->restricts[Restriction(Pos(i2, j2), Pos(i1, j1))] = GREATER;
		}
		
		for(int i=0; i<size; i++){
			for (int j=0; j<size; j++) {
				if (game->board[i][j] != -1){
					game->setted++;
					update_possibilities(game, i, j, game->board[i][j], false);
				}
			}
		}
		
		if (argc == 2) {
			stringstream ss(argv[1]);
			ss>>game->heur;
			game->heur-='0';
		}
		else game->heur = STD_HEURISTIC_LEVEL;

		backtracking(game);
		game->print_board();
		delete game;
	}


	return 0;
}
