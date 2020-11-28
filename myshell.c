/*
My shell project.
Developed by:
- Alvaro Martinez Quiroga.
- Patricia Tarazaga Cozas.
Any academic copies of this code are not under our responsibilities.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include "parser.h"

#define numeroComandos 4

//Estructuras
//idx pertenece a un indice para la estructura
//el pid del proceso se guarda en id
struct Estado{
	int idx;
	int id;
	char nombre[1024];
	char status[1024];
	struct Estado* next;
};

//Variables
char* prompt = "msh> ";
int i,j,k;
int **pipes;
pid_t pid, pidnumberFg;
pid_t *hijos;
int posicionComandoInterno;
char* comandosInternos[numeroComandos] = {"cd", "jobs", "fg", "exit"};
struct Estado *head = NULL;
int imHome = 0;
static int esBackground = 0;

//Funciones
void imprimirPrompt();
void cerrarPipes(int primero, int medio, int ultimo, int** pipes, int i, int n);
void redireccionEntrada(tline* line);
void redireccionSalida(tline* line);
void redireccionError(tline* line);
struct Estado* crearEstado(char *nombreComando, int id);
void insertarEstado(char *nombreComando,int id);
void imprimirEstados();
int borrarNodo(struct Estado *nodo);
void fgUltimoNodo();
void imprimirErrores(int error, tline* line);
void validarComandos(tline* line);
int comprobarComando(char* comando);
int comprobarBackground(tline* line, pid_t pidbg);
void manejador_background(int sig);
void liberaCabeza();
int existeEnBackground(tline* line);
char* devuelveNombre(int numero);
void borrarNodoID(int idBorrar);
void manejador_sigint(int sig);

/* MAIN */

int main(int argc, char** argv){
	fprintf(stderr, "\033[1;35m");
	fprintf(stderr, "\nInicializando MiniShell...\n");
	fprintf(stderr, "*-- AUTORES --*\n");
	fprintf(stderr, " Patricia Tarazaga Cozas \n");
	fprintf(stderr, " Alvaro Martinez Quiroga \n\n");
	fprintf(stderr, "Para salir ejecute el comando 'exit'\n\n");
    fprintf(stderr, "\033[0;0m");

    char buf[1024];

    int comandosInternos;
    int contiene_cd;

    tline* line;

	signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGCHLD, manejador_background);

    imprimirPrompt();

    while(fgets(buf, 1024, stdin)){
        if(strcmp(buf, "\n") == 0){
            //imprimir el prompt de nuevo
        }else{
        	signal(SIGINT,manejador_sigint);
            //guardamos todos los comandos
            line = tokenize(buf);
            validarComandos(line);
            /* COMANDO CD */
            //comprobamos si tiene el comando cd la linea introducida
            contiene_cd = 0;
            for(i = 0; i < line->ncommands && contiene_cd == 0; i++){
                for(j = 0; j < line->commands[i].argc; j++){
                    if(strcmp("cd", line->commands[i].argv[0]) == 0){
                        contiene_cd = 1;
                    }
                }
            }
            if(contiene_cd == 1 && line->ncommands > 1){
                //contine cd y mas de un comando, cd se ejecuta solo.
                //avisar del error
				imprimirErrores(1,line);
            }else if(contiene_cd == 1 && line->ncommands == 1){
                if(line->commands[0].argv[1] != NULL && line->commands[0].argv[2] != NULL && imHome != 1){
					imprimirErrores(2,line);
                }else{
                    if(line->commands[0].argv[1] == NULL){
						//si ya estas en /home no hagas nada
						if(imHome == 0){
							chdir(getenv("HOME"));
							imHome = 1;
						}else{
							fprintf(stderr, "\033[1;31m");
            				fprintf(stderr, "El directorio actual ya es el directorio raiz del sistema.\n");
            				fprintf(stderr, "\033[0;0m");
						}
                    }else if(line->commands[0].argv[1] != NULL && line->commands[0].argv[2] != NULL){
						imprimirErrores(2,line);
					}else{
                        int fallo = chdir(line->commands[0].argv[1]);
						imHome = 0;
                        if(fallo == -1){
                        	imprimirErrores(3,line);
                        }
                    }
                }
            }
            /* FIN CD */
            /* COMANDO JOBS */
            else if(strcmp(line->commands[0].argv[0],"jobs") == 0) {
            	if(head != NULL)
            		imprimirEstados();
            }
            /* FIN JOBS */
			/* COMANDO FG */
			else if(strcmp(line->commands[0].argv[0],"fg") == 0){
				if(line->commands[0].argv[2] != NULL || line->commands[0].argv[1] == NULL){
					signal(SIGINT, manejador_sigint);
					fgUltimoNodo();
				}else{
					int omgomg = existeEnBackground(line);
					if(omgomg == 0){
						imprimirErrores(9, line);
					}else{
						signal(SIGINT, manejador_sigint);
						char* nombre = devuelveNombre(omgomg);
						printf("%s\n", nombre);
						borrarNodoID(omgomg);
						pidnumberFg = omgomg;
						waitpid(pidnumberFg, NULL, 0);
					}
				}
			}
			/* FIN COMANDO FG */
            /* 1 COMANDO */
            else if(line->ncommands == 1 && line->background == 0){
                int salir = strcmp(line->commands[0].argv[0], "exit");
                if(salir == 0){
                    imprimirErrores(7, line);
					liberaCabeza();
                    return(0);
                }else{
                    pid_t pid;
                    int status;
                    pid = fork();
                    if(pid==0) {
                        //Soy el hijo, ejecuto el comando
                        redireccionEntrada(line);
                        redireccionSalida(line);
                        redireccionError(line);
                        execvp(line->commands[0].filename,(line->commands[0].argv));
                        imprimirErrores(4,line);
                        exit(1);
                    }
                    if(pid > 0) {
                        //Soy el padre, espero a que termine el hijo
                        waitpid(pid,&status,0);
                        if(WIFEXITED(status)) {
                        	int exit_status = WEXITSTATUS(status);
                        	if(exit_status == 1) {
                        		//mensaje de error del propio comando
                        	}
                        }
                    } else {
                       	imprimirErrores(5,line);
                	}
                }
            }
            /* FIN 1 COMANDO */
            /* VARIOS COMANDOS [PIPES] */ /* Primero pipes luego fork(), asi hijos conocen una copia de los pipes */
            //Si alguno de los comandos es el comando exit, la shell terminara de ejecutar
			else if(line->ncommands >= 2){
				int salir = 1;
				int contadorComandos;
				while(contadorComandos < line->ncommands && salir != 0){
					salir = strcmp(line->commands[contadorComandos].argv[0], "exit");
					contadorComandos++;
				}
                if(salir == 0){
            		imprimirErrores(7, line);
					liberaCabeza();
        			return(0);
				}
				if(line->background == 1 && salir == 0){
					imprimirErrores(7, line);
					liberaCabeza();
        			return(0);
				}
                pipes = (int**) malloc((line->ncommands-1)*(sizeof(int*)));
                hijos = (pid_t*) malloc(line->ncommands*(sizeof(int)));
                for(i = 0; i<line->ncommands-1; i++) {
                    pipes[i] = (int*) malloc(2*sizeof(int));
					pipe(pipes[i]);
                }
                int n = (line->ncommands-1);
                int status;
                for(i = 0; i<line->ncommands; i++) {
                    //Creamos los hijos
                    pid = fork();
                    if(pid==0) {
                        if(i==0) {
                        	//Primer comando
                        	cerrarPipes(1,0,0,pipes,i,n);
                        	close(pipes[i][0]); //voy a escribir
                        	redireccionEntrada(line);
                        	//Redirigimos la salida estandar al siguiente comando
                        	dup2(pipes[i][1],1);	
                        }		
                        if(i!=0 && i!=line->ncommands-1) {
                        	//Comando intermedio
                        	cerrarPipes(0,1,0,pipes,i,n);
                        	close(pipes[i-1][1]); //voy a leer del comando anterior
                        	close(pipes[i][0]); //voy a escribir al comando siguiente
                        	//Redirigimos la entrada estandar al comando anterior
                        	dup2(pipes[i-1][0],0);
                        	//Redirigimos la salida estandar al siguiente comando
                        	dup2(pipes[i][1],1);
                        }		
                        if(i==line->ncommands-1) {
                        	//Ultimo comando
                        	cerrarPipes(0,0,1,pipes,i,n);
                        	close(pipes[i-1][1]); //voy a leer del comando anterior
                        	redireccionSalida(line);
                        	redireccionError(line);
                        	//Redirigimos la entrada estandar al comando anterior
                        	dup2(pipes[i-1][0],0);                	
                        }
                        execvp(line->commands[i].filename, line->commands[i].argv);
						imprimirErrores(4,line);
                    }
					if(pid > 0) {
						hijos[i] = pid;
                    } else {
                  		imprimirErrores(5,line);
                    }
                }
                //Cerramos todas las pipes
                for(k = 0; k<n; k++) {
                    close(pipes[k][0]);
                    close(pipes[k][1]);                        		
                }	
				//wait para cada hijo  
				if(comprobarBackground(line,hijos[n]) == 1) {
					//Si el ultimo comando es background esperamos con WNOHANG
					fprintf(stderr, "[%d] %s\n", hijos[n], line->commands[n].argv[0]);
					for(k = 0; k<=n; k++) {
						waitpid(hijos[k],NULL,WNOHANG);
					}
				} else {
	                for(k = 0; k<=n; k++) {
	                    waitpid(hijos[k],&status,0);
	                    if(WIFEXITED(status)) {
	                    	int exit_status = WEXITSTATUS(status);
	                    	if(exit_status == 1) {
	                    		//mensaje de error del propio comando
	                    	}
	                    }
	                }					
				}              	
                //Liberar memoria dinamica
                for(k = 0; k<n; k++) {
                    free(pipes[k]);
                }
                free(pipes);
                free(hijos);
            }
            /* FIN VARIOS COMANDOS */
            /* 1 COMANDO EN BACKGROUND */
            else if(line->background == 1 && line->commands[0].filename != NULL){
				//Ejecucion de un comando en background
                pid_t pidbg;
                int s;
                pidbg = fork();
                signal(SIGINT, SIG_IGN);
                insertarEstado(line->commands[0].argv[0],pidbg);
                if(pidbg < 0){
					imprimirErrores(5,NULL);
                    exit(-1);
                }else if(pidbg == 0){ //Soy el hijo
                    execvp(line->commands[0].filename,line->commands[0].argv);
					imprimirErrores(4,line);
                    exit(1);
                }else{
					fprintf(stderr, "[%d] %s\n", pidbg, line->commands[0].argv[0]);
				}
            }
            else if (line->background == 1 && strcmp(line->commands[0].argv[0], "exit") == 0) {
				fprintf(stderr, "\033[1;31m");
				fprintf(stderr, "\nSe ha intentado ejecutar el comando [exit] en segundo plano.\nLa mini shell terminara su ejecucion actual.\n");
                imprimirErrores(7, line);
				liberaCabeza();
                return(0);
				
            }
            /* FIN 1 COMANDO BACKGROUND */
            /* LECTURA ERRONEA DE COMANDOS */
            else{
            	imprimirErrores(6,line);
            }
            /* FIN LECTURA ERRONEA DE COMANDOS */
        }
        signal(SIGINT, SIG_IGN);
        imprimirPrompt();
    }
	//Por si se da el caso de que el codigo llegue a este return, liberar el nodo principal
	if(head != NULL)
		liberaCabeza();
    return 0;
}
/* FIN MAIN */

/* FUNCIONES */
void imprimirPrompt(){
    char cwd[1024];
    fprintf(stderr, "\033[1;34m");
    fprintf(stderr, "%s", getcwd(cwd, 200));
    fprintf(stderr, "\033[1;32m");
    fprintf(stderr, " %s", prompt);
    fprintf(stderr, "\033[0;0m");
}

void redireccionEntrada(tline *line) {
	//Los datos nos entran por un fichero
	if(line->redirect_input != NULL) {
		FILE* fd = fopen(line->redirect_input,"r");
		if(fd==NULL) {
			fprintf(stderr,"%s: Error. %s\n",line->redirect_input,strerror(errno));
			kill(getpid(),SIGKILL);
		} else {
			int f = fileno(fd);
			dup2(f,0); //entra por la entrada estandar al proceso que toque
        	fclose(fd);
		}
	}
}

void redireccionSalida(tline *line) {
	//Los datos salen a un fichero
	if(line->redirect_output != NULL) {
		FILE* fd = fopen(line->redirect_output,"w");
		if(fd==NULL) {
			fprintf(stderr,"%s: Error. %s\n",line->redirect_output,strerror(errno));
			kill(getpid(),SIGKILL);
		} else {
			int f = fileno(fd);
			dup2(f,1); //sale por la salida estandar del proceso que toque
       		fclose(fd);
		}
	}
}

void redireccionError(tline *line) {
	//Los datos van a la salida error
	if(line->redirect_error != NULL) {
		FILE* fd = fopen(line->redirect_error,"w");
		if(fd==NULL) {
			fprintf(stderr,"%s: Error. %s\n",line->redirect_error,strerror(errno));
			kill(getpid(),SIGKILL);
		} else {
			int f = fileno(fd);
			dup2(f,2); //sale por la salida de error del proceso que toque
       		fclose(fd);
		}
	}
}

void cerrarPipes(int primero, int medio, int ultimo, int** pipes, int i, int n) {
	//Cierro el resto de pipes, me quedo con la mia
	if(primero==1) {
	    for(j=0; j<n; j++) {
	        if(j!=i) {
	            close(pipes[j][0]);
	            close(pipes[j][1]);
	        }
	    }
	}
	if(medio==1){
	    for(j=0; j<n; j++) {
	        if(j!=i && j!=i-1) {
	            close(pipes[j][0]);
	            close(pipes[j][1]);
	        }
	    }	
	}
	if(ultimo==1) {
	    for(j=0; j<n; j++) {
	        if(j!=i-1) {
	            close(pipes[j][0]);
	            close(pipes[j][1]);
	        }
	    }	
	}
}

struct Estado* crearEstado(char *nombreComando, int id) {
	struct Estado *e = (struct Estado*)malloc(sizeof(struct Estado));
	if(e != NULL) {
		e->idx = 1;
		e->id = id;
		strcpy(e->nombre,nombreComando);
		strcpy(e->status,"Ejecutando");
		e->next = NULL;
	} else {
        fprintf(stderr, "\033[1;31m");
		fprintf(stderr,"No se pudo escribir el estado en memoria\n");
        fprintf(stderr, "\033[0;0m");
	}
	return e;
}

void insertarEstado(char *nombreComando, int id) {
	struct Estado *e = crearEstado(nombreComando, id);
	struct Estado *paux;
	if(head == NULL) {
		head = e;
	} else {
		paux = head;
		while(paux->next != NULL) {
			paux = paux->next;
		}
		paux->next = e;
		e->idx = paux->idx+1;
	}
}

void imprimirEstados() {
	struct Estado *paux;
	paux = head;
	fprintf(stderr,"Num\tPID\tNOMBRE\t\tSTATUS\n");
	while(paux != NULL) {
		fprintf(stderr,"[%d]\t%d\t%s\t\t%s\n",paux->idx,paux->id,paux->nombre,paux->status);
		paux = paux->next;
	}
}

int borrarNodo(struct Estado *nodo) {
	if(head->id == nodo->id) {
		//Estamos al principio
		head = head->next;
		free(nodo);
	} else {
		struct Estado *paux = head;
		if(nodo->next == NULL) {
			//Somos el ultimo
			while(paux->next->next != NULL) {
				paux = paux->next;
			}
			paux->next = NULL;
			free(nodo);
		} else {
			//Estamos en medio
			while(paux->next != NULL && paux->next->id !=nodo->id) {
				paux = paux->next;
			}
			paux->next = nodo->next;
			free(nodo);
		}
	}
}

void fgUltimoNodo() {
	struct Estado *nodo;
	struct Estado *nodo2;
	nodo = head;
	if(nodo != NULL) {
		while(nodo->next != NULL) {
			nodo2 = nodo;
			nodo = nodo->next;
		}
		pidnumberFg = nodo->id;
		printf("%s\n",nodo->nombre);
		borrarNodo(nodo);
		waitpid(pidnumberFg,NULL,0);		
	} else {
		    fprintf(stderr, "\033[1;31m");
            fprintf(stderr, "No hay procesos en segundo plano.\n");
            fprintf(stderr, "\033[0;0m");
	}

	
}

void borrarNodoID(int idBorrar){
	struct Estado *nodo;
	int encontradoNodo = 0;
	nodo = head;
	while(nodo != NULL && encontradoNodo != 1){
		if(nodo->id == idBorrar){
			encontradoNodo = 1;
			borrarNodo(nodo);
		}else{
			nodo = nodo->next;
		}
	}
}

void liberaCabeza(){
	free(head);
}

void imprimirErrores(int error, tline* line) {
	switch(error) {
		case 1:
		    fprintf(stderr, "\033[1;31m");
            fprintf(stderr, "El comando cd debe utilizarse solo.\n");
            fprintf(stderr, "\033[0;0m");	
            break;
        case 2:
        	fprintf(stderr, "\033[1;31m");
            fprintf(stderr, "Demasiados parametros recibidos.\n");
            fprintf(stderr, "\033[0;0m");
            break;
        case 3:
        	fprintf(stderr, "\033[1;31m");
            fprintf(stderr, "No se ha podido cambiar el directorio de trabajo.\n");
            fprintf(stderr, "El directorio introducido [%s] no existe.\n", line->commands[0].argv[1]);
            fprintf(stderr, "\033[0;0m");
            break;
        case 4:
        	fprintf(stderr, "\033[1;31m");
            fprintf(stderr,"%s: No se encuentra el comando\n",line->commands[j].argv[0]);
            fprintf(stderr, "\033[0;0m");
            break;
        case 5:
        	fprintf(stderr, "\033[1;31m");
            fprintf(stderr,"Ha ocurrido un error con la creacion del proceso hijo\n");
            fprintf(stderr, "\033[0;0m");
            break;
        case 6:
        	fprintf(stderr, "\033[1;31m");
            fprintf(stderr, "Error en la lectura de comandos.\n");
            fprintf(stderr, "\033[0;0m");
			break;
		case 7:
			fprintf(stderr, "\033[1;35m");
			fprintf(stderr, "\nSaliendo de la MiniShell.\n\n");
            fprintf(stderr, "\033[0;0m");
			break;
		case 9:
			fprintf(stderr, "\033[1;31m");
			fprintf(stderr, "\nNo se ha encontrado el proceso.\n\n");
            fprintf(stderr, "\033[0;0m");
			break;
		default:
			break;
	}
}

void validarComandos(tline* line) {
	for(j=0; j<line->ncommands; j++) {
		if(line->commands[j].filename==NULL && comprobarComando(line->commands[j].argv[0])!=0) {
			imprimirErrores(4,line);
		}
	}
}

int comprobarComando(char* comando){
    int encontrado = 1;
    for(posicionComandoInterno = 0; posicionComandoInterno < numeroComandos && encontrado == 1; posicionComandoInterno++){
        if(strcmp(comandosInternos[posicionComandoInterno], comando) == 0){
            encontrado = 0;
        }
    }
    return encontrado;
}

void manejador_background(int sig) {
	pid_t pidbg = waitpid(-1,NULL,WNOHANG);
	struct Estado *paux = head;
	int encontrado = 0;
	while(paux != NULL && encontrado != 1) {
		if(paux->id == pidbg) {
			strcpy(paux->status,"Hecho");
			encontrado = 1;
			printf("\n");
			imprimirEstados();
			borrarNodo(paux);
			imprimirPrompt();	
		}
		paux = paux->next;
	}
}

int comprobarBackground(tline* line, pid_t pidbg) {
	if(line->background==1) {
		insertarEstado(line->commands[line->ncommands-1].argv[0],pidbg);
		return 1;
	}
	return 0;
}

int existeEnBackground(tline* line){
	struct Estado *paux = head;
	int numeroLine;
	numeroLine = atoi(line->commands[0].argv[1]); //almacenamos el pid o numero de proceso
	int pid;
	while(paux != NULL){
		if(paux->id == numeroLine || paux->idx == numeroLine){
			return paux->id;
		}
		paux = paux->next;
	}
	free(paux);
	return 0;
}

char* devuelveNombre(int numBuscar){
	struct Estado *paux = head;
	while(paux != NULL){
		if(paux->id == numBuscar){
			return paux->nombre;
		}
		paux = paux->next;
	}
	return "No encontrado";
}

void manejador_sigint(int sig) {
	if(pidnumberFg != 0) {	
		kill(pidnumberFg,SIGKILL);
		pidnumberFg = 0;
	}
	printf("\n");	
}
