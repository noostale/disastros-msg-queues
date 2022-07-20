#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "disastrOS.h"

int controllo = 0;

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Ciao, sono la funzione sleep partita dal processo n. %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
    printf("controllo: %d\n",controllo);
  }
}


void childFunction(void* args){
  printf("\nSono la childFunction con pid n. %d\n",disastrOS_getpid());
  printf("Farò uno sleep prima di terminare\n");

  int* id = (int*) args;
  
  int type=MESSAGE_QUEUE;  
  int mode=DSOS_READ;
  //Apro la risorsa con id 

  printf("Sto per aprire una risorsa che come id %d\n",*id);
  int fd = disastrOS_openResource(*id,type,mode);
  

  if (fd >= 0){  //Sennò un pid negativo mi manderebbe in seg_fault

    printf("Ho aperto una risorsa che ha fd=%d\n", fd);
    controllo--;
    char message[5];
    int len = disastrOS_readMessageQueue(fd, message, 5);
    printf("Ho letto il messaggio %s di lunghezza %d dal fd:%d\n", message, len, fd);
  }
  printf("PID: %d, terminating\n", disastrOS_getpid());

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }

  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {

  printf("Sto per stamapare lo stato primordiale del SO\n");
  disastrOS_printStatus();

  printf("FUNZIONE INIT APPENA PARTITA\n\n");
  //Inizializzo il contesto per la funzione sleeperFunction
  disastrOS_spawn(sleeperFunction, 0); 

  printf("Faccio partire %d writers\n", MAX_NUM_MESSAGES);
  int alive_children=0;
  int id[MAX_NUM_MESSAGES];

  for (int i=0; i<MAX_NUM_MESSAGES; ++i) {
    
    //Imposto che sto per aprire una risorsa di tipo MQ
    int type=MESSAGE_QUEUE;
    //Imposto che se la risorsa non è già presente la creo
    int mode=DSOS_CREATE;

    printf("Sto aprendo una risorsa (la creo se necessario) con id %d\n", i);
    int fd = disastrOS_openResource(i,type,mode);
    controllo++;
    printf("Ho aperto con successo una risorsa con fd=%d ed id=%d\n", fd, i);
    
    //Creo un buffer statico i cui dati dovranno essere scritti in un messaggio
    char message[5] = "test";

    //Scrivo il messaggio nella mq puntata da fd
    disastrOS_writeMessageQueue(fd, message, 5);
    
    printf("Ho scritto il messaggio %s in %d\n", message, fd);

    
    id[i] = i;

    disastrOS_spawn(childFunction, (void*) &(id[i]));
    disastrOS_printStatus();
    alive_children++;
    printf("FINE CICLO WRITER\n\n");
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  //Finchè ci sono processi attivi, salvo in ret il loro valore di ritorno
  while(alive_children > 0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminato, retval:%d, alive: %d \n", pid, retval, alive_children);
    --alive_children;
  }

  printf("Spegnimento del SO");
  disastrOS_shutdown();
}


//Funzione main che farà partire la initFunction

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("\n---------------TEST MESSAGE QUEUE IN DISASTROS - FRASCA EMANUELE---------------\n");
  printf("\nthe function pointer is: %p\n", childFunction);
  // spawn an init process
  
  printf("AVVIO SISTEMA OPERATIVO\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
