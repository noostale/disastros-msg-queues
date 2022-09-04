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
int letti = 0;
int scritti = 0;


// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Ciao, sono la funzione sleep partita dal processo n. %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}


void child_reader(void* args){
  printf("Sto eseguendo il processo con ID: %d\n", running->pid);

  int i = *(int*)args;
  int fd = disastrOS_openMessageQueue(i, DSOS_WRITE); //attento

  if(disastrOS_getpid()%2 == 0)
    disastrOS_sleep(15);
  if(disastrOS_getpid()%3 != 0)
    disastrOS_sleep(12);

  char arrDst[4];
  int res;

  for(int j = 0; j<MAX_NUM_MESSAGES+MESSAGES_EXTRA; j++){
    res = -20;
    while(res == -20){
      res = disastrOS_MessageQueue_read(i,arrDst,MAX_LEN_MESSAGE);
      disastrOS_printStatus();
    }
    letti++;
  }
  
  disastrOS_closeMessageQueue(fd);
  printf("PID: %d, sta terminando\n", disastrOS_getpid());
  disastrOS_exit(fd);
}


void child_writer(void* args) {
  printf("Sto eseguendo il processo con ID: %d\n", running->pid);

  int i = *(int*)args;
  int fd=disastrOS_openMessageQueue(i,DSOS_WRITE);
  char arrSource[5] = "test";

  if(disastrOS_getpid()%4 == 0)
    disastrOS_sleep(10);
  if(disastrOS_getpid()%5 != 0)
    disastrOS_sleep(7);

  int res;
  
  for(int j = 0; j<MAX_NUM_MESSAGES+MESSAGES_EXTRA; j++){
    res = -20;
    while(res == -20){
      res = disastrOS_MessageQueue_write(i,arrSource,5);
      disastrOS_printStatus();
    }
    scritti++;
  }

  disastrOS_closeMessageQueue(fd);
  printf("PID: %d, sta terminando\n", disastrOS_getpid());
  disastrOS_exit(fd);
}


void initFunction(void* args) {
  printf("Avvio funzione init\n\n");

  int alive_children=0;

  printf("Stato primordiale del SO:\n");
  disastrOS_printStatus();
  disastrOS_spawn(sleeperFunction, 0);

  //Apro le MQ in init
  int qids[MAX_NUM_MESSAGEQUEUES];
  for (int i=0; i<MAX_NUM_MESSAGEQUEUES; i++){
    qids[i]=i;
    disastrOS_openMessageQueue(i,DSOS_CREATE);
  }

  printf("Ho terminato di creare tutte le Message Queues di cui ho bisogno\n");

  
  int counter = 0;
  for(int i=0; i<CHILDREN; i++){

    if(counter == MAX_NUM_MESSAGEQUEUES) counter = 0;

    printf("Sto per spawnare un writer\n");
    disastrOS_spawn(child_writer, qids+counter);
    printf("Sto per spawnare un reader\n");
    disastrOS_spawn(child_reader, qids+counter);
    alive_children+=2;
    ++counter;
  }
  
  /**
  int counter = 0;
  for(int i=0; i<CHILDREN; i++){

    if(counter == MAX_NUM_MESSAGEQUEUES)
      counter = 0;
    printf("Sto per spawnare un writer\n");
    disastrOS_spawn(child_writer, qids+counter);
    ++alive_children;
    ++counter;
  }
  printf("SONO INIT, HO FINITO DI SPAWNARE PROCESSI WRITER\n");

  counter = 0;
  for(int i=0; i<CHILDREN; i++){

    if(counter == MAX_NUM_MESSAGEQUEUES)
      counter = 0;
    printf("Sto per spawnare un reader\n");
    disastrOS_spawn(child_reader, qids+counter);
    ++alive_children;
    ++counter;
  }

  printf("SONO INIT, HO FINITO DI SPAWNARE PROCESSI READER\n");

  //OPPURE

  for(int i=0; i<MAX_NUM_MESSAGEQUEUES; i++){
    disastrOS_spawn(child_writer, qids+i);
    disastrOS_spawn(child_reader, qids+i);
    alive_children+=2;
  }
  
  **/
  
  disastrOS_printStatus();

  int retval = 0;
  int pid = 0;
  //Finchè ci sono processi attivi, salvo in ret il loro valore di ritorno
  while(alive_children > 0 && printf("STO PER FARE UNA WAIT, SONO MAIN\n") && (pid=disastrOS_wait(0, &retval))>=0){
    disastrOS_printStatus();
    printf("initFunction, child: %d terminato, retval:%d, alive: %d \n", pid, retval, alive_children);
    --alive_children;
  }

  printf("SE %d E %d COINCIDONO IL PROGRAMMA HA LETTO E SCRITTO LO STESSO NUMERO DI MESSAGGI\n", scritti, letti);
  printf("Distruggo le MQ\n");
  for (int i=0; i<MAX_NUM_MESSAGEQUEUES; i++){
    disastrOS_MessageQueue_destroy(i);
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
  
  printf(R"(
   _____  _               _              ____   _____ 
  |  __ \(_)             | |            / __ \ / ____| 
  | |  | |_ ___  __ _ ___| |_ _ __ ___ | |  | | (___   
  | |  | | / __|/ _` / __| __| '__/ _ \| |  | |\___ |
  | |__| | \__ \ (_| \__ \ |_| | | (_) | |__| |____) | 
  |_____/|_|___/\__,_|___/\__|_|  \___/ \____/|_____/  
                                                        
  Ver. 2.0 Frasca Emanuele)");
  printf("\n\n");

  sleep(1);
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
