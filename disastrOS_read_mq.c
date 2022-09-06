#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include <assert.h>
#include "disastrOS_constants.h"


void internal_MessageQueue_read() {
  //printf("Il processo con ID: %d sta per leggere un messaggio della Message Queue con ID: %ld\n",
  //       running->pid, running->syscall_args[0]);

  //1 get from the PCB the resource id of the resource to open
  int id=running->syscall_args[0];
  char* buf_des = (char*)running -> syscall_args[1];
  int buf_length=running->syscall_args[2];

  MessageQueue* mq = MessageQueueList_byId(&mq_list, id);

  //Controllo se la MQ ha messaggi da leggere, se non li ha:
  //1.Metto il processo in attesa
  //2.Inserisco il processo nella lista dei processi in attesa
  //3.Inserisco il processo nella lista dei processi in attesa di leggere
  //4.Selezioni come runner il primo processo nella ready list


  Message* first_message;
  if(mq==NULL) { first_message = NULL; }
  else {first_message = (Message*)mq -> messages.first;}

  if(!first_message) {

    //printf("Il processo con ID: %d ha provato a leggere un messaggio da una Message Queue vuota, viene sospeso\n",
    //      running->pid);

    running->status=Waiting;
    List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
    List_insert(&mq->waiting_to_read, mq->waiting_to_read.last, (ListItem*) PCBPtr_alloc(running));

    PCB* next_running= (PCB*) List_detach(&ready_list, ready_list.first);
    next_running->status = Running;
    running = next_running;
    
    return;
  }

  if((ListItem*)first_message == NULL) return;

  
  if(first_message->length > buf_length){
    running -> syscall_retvalue = -15;
    return;
  }
  

  List_detach(&mq->messages, (ListItem*)first_message);

  //printf("Il processo con ID: %d ha appena letto un messaggio dalla Message Queue di ID: %d, \"%s\"\n",
  //       running->pid, id, first_message->message);

  while(mq->waiting_to_write.size > 0){
    //printf("Il processo con ID: %d riattiva un writer che era precedentemente in attesa\n", running->pid);

    //Caccio il puntatore dallo struct di liste di ptr al PCB che deve scrivere
    PCBPtr* writer_prt = (PCBPtr*)List_detach(&mq->waiting_to_write, mq->waiting_to_write.first);
    //Rimuovo il processo che deve ripartire dalla lista dei processi in waiting
    List_detach(&waiting_list, (ListItem*)writer_prt->pcb);

    PCB* writer = (PCB*) writer_prt->pcb;
    writer -> status = Ready;
    writer -> syscall_retvalue = -20;

    //Inserisco il writer nella lista dei ready
    List_insert(&ready_list, ready_list.last, (ListItem*)writer_prt->pcb);
    
    assert(PCBPtr_free(writer_prt)>=0);
  }
  
  
  int m_length=first_message->length;
  
  if (first_message && mq->waiting_to_write.size == 0){
    for(int i=0; i<m_length; i++){
      buf_des[i] = first_message->message[i];
    }
  }
  

  mq -> num_written -= 1;
  running -> syscall_retvalue = m_length;

  return;
}
