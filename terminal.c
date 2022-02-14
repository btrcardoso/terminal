#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void loop();
char* ler_linha(void);
char** divide_linha(char *linha);
int launch(char **args);
int executar(char **args);

// Comandos do terminal
int md(char **args);
int ajuda(char **args);
int sair(char **args);

char *comandos_str[] = {
	"md",
	"ajuda",
	"sair"
};

int (*comandos_func[]) (char**) = {
	&md,
	&ajuda,
	&sair
};

int num_comandos();

int main (int argc, char **argv){
	loop();
	return EXIT_SUCCESS;
}

// loop para inserir comandos
void loop(){
	char *linha;
	char **args;
	int status;

	do{
		printf("<3: ");
		linha = ler_linha();
		args = divide_linha(linha);
		status = executar(args);

		free(linha);
		free(args);

	} while(status);
}

// lê a linha digitada no terminal
char *ler_linha(void){
	char *linha = NULL;
	ssize_t tam = 0;

	if(getline(&linha, &tam, stdin) == -1){
		if(feof(stdin)){
			exit(EXIT_SUCCESS); // recebemos um EOF
		} else {
			perror("readline");
			exit(EXIT_FAILURE);
		}
	}

	return linha;
}

#define QTD_TOKS_LINHA 64
#define TOK_DELIMITADOR " \t\r\n\a"

// retorna um vetor com os tokens da linha
char **divide_linha(char *linha){

	int toks_linha = QTD_TOKS_LINHA;
	int posicao = 0;
	char **tokens = malloc(sizeof(char*) * toks_linha);
	char *token;

	if(!tokens){
		fprintf(stderr, "erro de alocação\n");
		exit(EXIT_FAILURE);
	}

	// pega o primeiro token
	token = strtok(linha, TOK_DELIMITADOR);

	// pega os proximos tokens
	while(token!=NULL){

		tokens[posicao] = token;
		posicao++;

		if(posicao >= QTD_TOKS_LINHA){
			toks_linha += QTD_TOKS_LINHA;
			tokens = realloc(tokens, toks_linha * sizeof(char*));

			if(!tokens){
				fprintf(stderr, "erro de alocação\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TOK_DELIMITADOR);
	}

	tokens[posicao] = NULL;
	return tokens;
}

int launch(char **args){
	pid_t pid, wpid;
	int status;

	pid = fork();

	if(pid == 0){
		// Processo filho

		/* execvp recebe um nome de arquivo ou caminho de 
		um novo arquivo de programa para carregar e executar 
		como o primeiro argumento, e o segundo argumento
		recebe un array de argumentos do programa */
		if(execvp(args[0], args) == -1){          
			perror("Comando não existe. Escreva <ajuda> para conhecer os comandos.");
		}

	} else if(pid < 0){
		// Processo deu erro
		perror("Erro ao iniciar novo processo");
	} else {
		// Processo pai

		/* O pai espera o comando do filho finalizar, ou seja,
		o waitpid() espera que o estado do processo filho mude.*/
		do{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));

		// WIFEXITED Verifica se o processo filho finalizou com sucesso
		// WIFSIGNALED determina se o processo filho foi encerrado porque gerou um sinal que fez com que ele fosse encerrado
	}

	return 1;
}

int executar(char **args){
	int i;

	if(args[0] == NULL){
		return 1;
	}

	for(i = 0; i < num_comandos(); i++){
		if(strcmp(args[0], comandos_str[i]) == 0){
			return (*comandos_func[i])(args);
		}
	}

	return launch(args);
}


int num_comandos(){
	return sizeof(comandos_str) / sizeof(char*);
}

int md(char **args){
	if(args[1] == NULL){
		fprintf(stderr, "Argumento esperado para \"cd\"\n");
	} else {
		if(chdir(args[1]) != 0){
			perror("Erro ao mudar de diretório");
		}
	}
	return 1;
}

int ajuda(char **args){
	int i;
	printf("Terminal feito por Beatriz Cardoso seguindo este tutorial: https://brennan.io/2015/01/16/write-a-shell-in-c/ :)\n");
	printf("Escreva algum dos comandos no terminal e seus argumentos e aperte ENTER para ver a mágica acontecer.\nOs comandos são:\n");

	for(i = 0; i < num_comandos(); i++){
		printf(" %s\n", comandos_str[i]);
	}

	return 1;
}

int sair(char **args){
	return 0;
}


/*
#define TAM_LINHA 6
char *ler_linha(void){
	int tam = TAM_LINHA;
	char *linha = malloc(sizeof(char) * tam);

	if(!linha){
		fprintf(stderr, "erro de alocação\n");
		exit(EXIT_FAILURE);
	}

	int c;
	int posicao = 0;

	while(1){
		c = getchar();

		if(c == EOF || c == '\n'){
			linha[posicao] = '\0';
			return linha;
		} else {
			linha[posicao] = c;
		}

		posicao ++;

		if(posicao >= tam){
			tam += TAM_LINHA;
			linha = realloc(linha, tam);

			if(!linha){
				fprintf(stderr, "erro de alocação\n");
        		exit(EXIT_FAILURE);
			}
		}

	}
}
*/