#include <map>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <ctime>

#define LESSER -1
#define GREATER 1

#define GAME_COMPLETED 0
#define KEEP_GOING 1

#define DEAD_END 1

#define STD_HUERISTIC_LEVEL 2

#define BACKTRACK_LIMIT 1000000

using namespace std;



class Pos;
class Restriction;
class Remaining;
class Futoshiki;

char no_heur(Futoshiki *, Pos);
bool set_possibility(Futoshiki *, Pos, char, bool);
bool update_restriction(Futoshiki *, Pos, Pos, char, bool);
bool update_possibilities(Futoshiki *, Pos, char, bool);
char foward_checking(Futoshiki *, Pos);
void backtracking(Futoshiki *);



/*
Classe para osicao de uma celula no tabuleiro
Serve como classe "wrapper" para pair<char, char> 
*/
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
	
	inline char get_i()
	{
		return this->pos.first;
	}
	
	inline char get_j()
	{
		return this->pos.second;
	}
	
	inline void set_i(char i)
	{
		this->pos.first = i;
	}
	
	inline void set_j(char j)
	{
		this->pos.second = j;
	}


	const bool operator < (const Pos b) const
	{
		return this->pos < b.pos;
	}
};


/*
Classe para as restricoes (maior ou menos)
entre duas posicoes do tabuleiro 
Serve como classe "wrapper" para pair<Pos, Pos> 
*/
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
	
	inline Pos get_p1()
	{
		return this->rest.first;
	}
	
	inline Pos get_p2()
	{
		return this->rest.second;
	}
	
	inline const Pos get_p1() const
	{
		return this->rest.first;
	}
	
	inline const Pos get_p2() const
	{
		return this->rest.second;
	}
	
	inline void set_p1(Pos p1)
	{
		this->rest.first = p1;
	}
	
	inline void set_p2(Pos p2)
	{
		this->rest.second = p2;
	}

	const bool operator < (const Restriction b) const
	{
		return this->rest < b.rest;
	}
};


/*
Classe que possui o vetor de valores posiveis para uma
certa posicao. Os valores do vetor comecam todos como 1. Caso
apareca uma restricao (Ex: Um valor ser definido na linha da posicao)
a posicao corresponde a restricao é decrementada. Dessa forma,
caso uma posicao do vetor possua valor menor que 1, o valor dessa
posicao e invalido.
A classe tambem possui um contador com o numero de valores validos (>=1)
*/
class Remaining
{
public:
	char * vect;
	char count;
	
	Remaining()
	{
		this->vect = NULL;
		this->count = 0;
	}
	
	Remaining(char size)
	{
		this->vect = new char[size];
		//Iniciar todos os valores com 1
		for (char i=0; i<size; i++) vect[i] = 1;
		
		this->count = size;
	}
	
	/*
	~Remaining()
	{
		delete this->vect;
	}*/
};


/*
Classe que possui as informacoes relevantes ao jogo.
*/
class Futoshiki
{
public:
	char size; 		//Tamanho do tabuleiro
	char setted;	//Numero de posicoes ja definidas
	char heur;		//Qual heuristica utilizar
	char ** board;	//Matriz do tabuleiro
	long ops;		//Numero de operacoes feitas
	
	//Matriz dos valores possiveis remanescentes para cada posicao
	Remaining ** remain;
	
	//Mapa das restricoes. Cada Restricao (par de posicoes) esta
	//mapeado para o tipo da restricao(Maior ou menos)
	map<Restriction, char> restricts;


	Futoshiki()
	{
		this->size = 0;
		this->setted = 0;
		this->heur = STD_HUERISTIC_LEVEL;
		this->ops = 0;
		this->board = NULL;
		this->remain = NULL;
		this->restricts = map<Restriction, char>();
	}

	Futoshiki(char size)
	{
		this->size = size;
		this->setted = 0;
		this->heur = STD_HUERISTIC_LEVEL;
		this->ops = 0;
		this->restricts = map<Restriction, char>();

		this->board = new char*[size];
		for(char i=0; i<size; i++) {
			this->board[i] = new char[size];
			//Todos os valores comecam como -1, que equivale a nao definido
			memset(this->board[i], -1, size * sizeof(char));
		}
		
		this->remain = new Remaining*[size];
		for (char i=0; i<size; i++){
			this->remain[i] = new Remaining[size];
			for (char j=0; j<size; j++) this->remain[i][j] = Remaining(size);
		}
	}
	
	
	~Futoshiki()
	{		
		for (char i=0; i<this->size; i++) delete this->board[i];
		delete this->board;
	}

	
	/*
	Funcao que faz a leitura do tabuleiro pela stdin
	*/
	void read_board(char size, char nRestricts)
	{
		for(char i=0; i<size; i++){
			for (char j=0; j<size; j++) {
				scanf("%hhd", this->board[i] + j);
				this->board[i][j]--;
			}
		}
		
		for(char i=0; i<nRestricts; i++){
			char i1, j1, i2, j2;
			scanf("%hhd%hhd%hhd%hhd", &i1, &j1, &i2, &j2);
			i1--;i2--;j1--;j2--;
			
			if (abs(i1 - i2) + abs(j1 - j2) != 1){
				printf("Tabuleiro Invalido\n");
				exit(-2);
			}
			
			this->restricts[Restriction(Pos(i1, j1), Pos(i2, j2))] = LESSER;
			this->restricts[Restriction(Pos(i2, j2), Pos(i1, j1))] = GREATER;
		}
		
		for(char i=0; i<size; i++){
			for (char j=0; j<size; j++) {
				if (this->board[i][j] != -1){
					this->setted++;
					update_possibilities(this, Pos(i, j), this->board[i][j], false);
				}
			}
		}
				
		if (!this->valid()){
			printf("Tabuleiro Invalido\n");
			exit(-2);
		}
	}
	
	/*
	Funcao de impressao do tabuleiro na stdout
	*/
	void print_board()
	{
		for (char i=0; i<this->size; i++){
			for (char j=0; j<this->size; j++){
				printf("%hhd", this->board[i][j]+1);
				if (j != this->size-1) printf(" ");
			}
			printf("\n");
		}
	}

	
	/*
	Funcao que verifica se alguma restrcao do jogo foi quebrada,
	depois retorna se o tabuleiro atual é valido ou não.
	*/
	bool valid()
	{
		map<Restriction, char>::iterator it;
		for (it=this->restricts.begin(); it!=this->restricts.end(); it++){
			Pos p1 = it->first.get_p1(), p2 = it->first.get_p2();
			
			if (this->board[p1.get_i()][p1.get_j()]!=-1 && this->board[p2.get_i()][p2.get_j()]!=-1){
				if (it->second == LESSER && this->board[p1.get_i()][p1.get_j()] >= this->board[p2.get_i()][p2.get_j()]) return false;
				else if (it->second == GREATER && this->board[p1.get_i()][p1.get_j()] <= this->board[p2.get_i()][p2.get_j()]) return false;
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
	
	/*
	Funcao que verifica se o tabuleiro esta completo comparando
	o numero de posicoes ja definidas(setted) com o numero
	maximo possiivel(size^2)
	*/
	inline bool ended()
	{
		return (this->setted >= this->size * this->size);
	}
};



/*
Funcao de backtracking sem heuristica.
Simplesmente define um valor e continua na recursao caso
o tabulieor esteja valido. Termina quando o numero
de posicoes definidas for igual ao maximo possivel
*/
char no_heur(Futoshiki * game, Pos pos)
{
	for (char i=0; i<game->size; i++){
		game->board[pos.get_i()][pos.get_j()] = i;
		if (game->valid()){
			game->setted++;
			game->ops++;
			if (game->ended()) return GAME_COMPLETED; 

			backtracking(game);
			if (game->ended()) return GAME_COMPLETED;

			game->setted--;
		}
	}
	
	game->board[pos.get_i()][pos.get_j()] = -1;
	return KEEP_GOING;
}


/*
Funcao que remove ou adiciona "value"
das possibilidades de "pos", alterando o contador
quando necessário. Retorna se a posicao ficou com
0 possibilidades.
*/
inline bool set_possibility(Futoshiki * game, Pos pos, char value, bool plus_minus)
{
	//Altera o contador caso mude de 0 para 1 ou de 1 para 0
	if (game->remain[pos.get_i()][pos.get_j()].vect[value] == 1 && !plus_minus) game->remain[pos.get_i()][pos.get_j()].count--;
	if (game->remain[pos.get_i()][pos.get_j()].vect[value] == 0 && plus_minus) game->remain[pos.get_i()][pos.get_j()].count++;
	
	//Soma ou subtrai do vetor
	game->remain[pos.get_i()][pos.get_j()].vect[value] += plus_minus ? 1 : -1;
	//Retorna se ha 0 possibilidades
	return (game->remain[pos.get_i()][pos.get_j()].count <= 0 && game->board[pos.get_i()][pos.get_j()]==-1);
}

/*
Funcao que atualiza os valores possiveis
considerando apenas as restricoes de maior ou menor
entre duas posicoes. Retorna se alguma posicao
ficou com 0 possibilidades;
*/
bool update_restriction(Futoshiki * game, Pos p1, Pos p2, char value, bool plus_minus)
{
	bool dead_end = false;
	
	Restriction res(p1, p2);
	if (game->restricts.count(res)){ //Se a restricao existe
		if (game->restricts[res] == LESSER){
			//Atualizar todas as possibilidades menores
			for (char i=value-1; i>=0; i--){
				dead_end += set_possibility(game, p2, i, plus_minus);
			}
		}
		else{
			//Atualizar todas as possibilidades maiores
			for (char i=value+1; i<game->size; i++){
				dead_end += set_possibility(game, p2, i, plus_minus);
			}
		}
	}
				
	return dead_end;
}

/*
Funcao que atualiza as possibilidades de todas as
posicoes na mesma linha e coluna que "pos", e tambem das
adjacentes com relacao as restricoes de menor e maior.
*/
bool update_possibilities(Futoshiki * game, Pos pos, char value, bool plus_minus)
{
	bool dead_end = false;
	
	Pos aux_pos;
	//Atualizar possibilidades das linhas e colunas
	for (char i=0; i<game->size; i++) {
		aux_pos.set_i(pos.get_i());
		aux_pos.set_j(i);
		dead_end += set_possibility(game, aux_pos, value, plus_minus);
		
		aux_pos.set_i(i);
		aux_pos.set_j(pos.get_j());
		dead_end += set_possibility(game, aux_pos, value, plus_minus);
	}
	
	//Atualizar possibilidades das restricoes de maior e menor
	aux_pos.set_i(pos.get_i());aux_pos.set_j(pos.get_j()+1);
	dead_end += update_restriction(game, pos, aux_pos, value, plus_minus);
	aux_pos.set_j(pos.get_j()-1);
	dead_end += update_restriction(game, pos, aux_pos, value, plus_minus);
	aux_pos.set_i(pos.get_i()+1);aux_pos.set_j(pos.get_j());
	dead_end += update_restriction(game, pos, aux_pos, value, plus_minus);
	aux_pos.set_i(pos.get_i()-1);
	dead_end += update_restriction(game, pos, aux_pos, value, plus_minus);
	
	return dead_end;
}

/*
Funcao que define um valor possivel para a posicao "pos",
depois atualiza as possibilidades das outras posicoes do tabuleiro.
Caso o jogo esteja completo a recursao para. Caso contrario, 
ela verifica se alguma posicao fica com 0 possibilidades. Caso não
continua a recursao chamando a funcao backtracking.
*/
char foward_checking(Futoshiki * game, Pos pos)
{
	for (char i=0; i<game->size; i++){
		if (game->remain[pos.get_i()][pos.get_j()].vect[i] >= 1){
			game->board[pos.get_i()][pos.get_j()] = i;
			game->setted++;
			game->ops++;
			if (game->ended()) return GAME_COMPLETED;
			
			if (update_possibilities(game, pos, i, false) != DEAD_END){
				backtracking(game);
				if (game->ended()) return GAME_COMPLETED;
			}
			update_possibilities(game, pos, i, true);
			game->setted--;
		}
	}
	
	game->board[pos.get_i()][pos.get_j()] = -1;
	return KEEP_GOING;
}


/*
Funcao de bactracking, que percorre o tabuleiro e
chama uma funcao de heurista que aplicará a heuristica
e chamará a recursão indireta.
*/
void backtracking(Futoshiki * game)
{
	if (game->ops > BACKTRACK_LIMIT) return;

	if (game->heur != 2){
		for (char i=0; i<game->size; i++){
			for (char j=0; j<game->size; j++){
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
		/*
		Heuristica MVR que percorrera todo o tabuleiro]
		antes de chamar a recursao, procurando a posicao
		com menos possibilidades.
		*/
		char min = CHAR_MAX, min_line = -1, min_col = -1;
		for (char i=0; i<game->size; i++){
			for (char j=0; j<game->size; j++){
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
		scanf("%hhd%hhd", &size, &nRestricts);
		
		Futoshiki * game = new Futoshiki(size);
		game->read_board(size, nRestricts);
	
		//Definicao da heuristica por flag na chamada do programa
		if (argc == 2) sscanf(argv[1], "%hhd", &(game->heur));
		else game->heur = STD_HUERISTIC_LEVEL;
		
		clock_t start = clock();
		backtracking(game);
		clock_t end = clock();
		if (game->ops <= BACKTRACK_LIMIT){
			if (!game->valid()){
				printf("Errado\n");
				return -1;
			}
			else{
				game->print_board();
				printf("\n");
			}
		}
		else printf("Numero de atribuicoes excede limite maximo\n");

		printf("Tempo decorrido: %lf\nOperacoes feitas: %ld\n", (1000.0*(end-start))/CLOCKS_PER_SEC, game->ops);

		delete game;
	}


	return 0;
}
