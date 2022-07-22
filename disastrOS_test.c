#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "disastrOS.h"
#include "disastrOS_resource.h"
#include "disastrOS.c"
#include "disastrOS_mq.h"

int controllo = 0;

#define CHILDREN 5

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Ciao, sono la funzione sleep partita dal processo n. %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
    printf("controllo: %d\n",controllo);
  }
}


void child_reader(void* args){
  
  /**
  printf("PID: %d, terminating\n", disastrOS_getpid());

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }
  **/

  printf("SONO UN READER");

  sleep(5);

  //int fd = running->syscall_retvalue;

  char dst[5];

  
  for(int j=0; j<MAX_NUM_MESSAGES; j++){
    disastrOS_MessageQueue_read(0,dst,5);
    disastrOS_printStatus();
  }
  
  disastrOS_exit(disastrOS_getpid()+1);
}


void child_writer(void* args) {

  /**
  int* id = (int*) args;
  int mode=DSOS_CREATE;

  printf("Sto aprendo una risorsa (la creo se necessario) con il writer %d\n", *id);
  printf("Io writer voglio scrivere nella mq con id = %d\n\n",*id);
  int fd = disastrOS_openMessageQueue(*id,mode);
  controllo++;
    
  printf("Ho aperto con successo una risorsa con fd=%d ed id=%d\n", fd, *id);
  //Creo un buffer statico i cui dati dovranno essere scritti in un messaggio
  **/
  char src[5] = "test";


  //disastrOS_openMessageQueue(2,DSOS_CREATE);
  //disastrOS_openMessageQueue(3,DSOS_CREATE);
  //disastrOS_openMessageQueue(4,DSOS_CREATE);


  //int fd = running->syscall_retvalue;
 
  for(int j=0; j<MAX_NUM_MESSAGES; j++){
    disastrOS_MessageQueue_write(0,src,5);
    sleep((running->pid)*(running->pid)+10);
    disastrOS_printStatus();
  }


  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {

  printf("Sto per stamapare lo stato primordiale del SO\n");
  disastrOS_printStatus();
  disastrOS_spawn(sleeperFunction, 0); 

  printf("FUNZIONE INIT APPENA PARTITA\n\n");

  //Apro le MQ in init
  for (int i=0; i<MAX_NUM_MESSAGEQUEUES; i++){
    disastrOS_openMessageQueue(i,DSOS_CREATE);
  }

  //int fd = running->syscall_retvalue;

  //printf("fd:%d\n",fd);

  
  int alive_children=0;

  disastrOS_spawn(child_writer, 0);
  disastrOS_spawn(child_reader, 0); 
  

  alive_children+=2;
  
  

  disastrOS_printStatus();

  printf("3\n");

  int retval;
  int pid;
  //Finchè ci sono processi attivi, salvo in ret il loro valore di ritorno
  while(alive_children > 0 && (pid=disastrOS_wait(0, &retval))>=0){
    disastrOS_printStatus();
    printf("initFunction, child: %d terminato, retval:%d, alive: %d \n", pid, retval, alive_children);
    --alive_children;
  }

  printf("Spegnimento del SO\n");
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
  // spawn an init process
  
  printf("AVVIO SISTEMA OPERATIVO\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
