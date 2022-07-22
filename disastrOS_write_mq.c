#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_mq.h"


void internal_MessageQueue_write(){

  printf("Sono entrato nella write internal\n");

  //1 get from the PCB the resource id of the resource to open
  int id=running->syscall_args[0];
  char* write_buffer = (char*)running -> syscall_args[1];
  int message_length = running -> syscall_args[2];

  if(message_length > MAX_LEN_MESSAGE){
    running -> syscall_retvalue = -14;
    return;
  }

  //Descriptor* mq_des = DescriptorList_byFd(&running -> descriptors, id);

  //MessageQueue* mq = mq_des -> mq;

  MessageQueue* mq = MessageQueueList_byId(&mq_list, id);

  MessageQueue_print(mq);

  if(mq == NULL){
    running -> syscall_retvalue = DSOS_ERESOURCENOFD;
    return;
  }

  
  if(mq->num_written == MAX_NUM_MESSAGES){

    printf("HO PROVATO A SCRIVERE IN UNA CODA PIENA\n");

    running->status=Waiting;
    List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
    List_insert(&mq->waiting_to_write, mq->waiting_to_write.last, (ListItem*) PCBPtr_alloc(running));

    disastrOS_printStatus();

    PCB* next_running = (PCB*) List_detach(&ready_list, ready_list.first);
        
    next_running -> status = Running;
    running=next_running;
    return;

  }
  
  Message* message = Message_alloc(write_buffer, message_length);


  List_insert(&mq->messages, mq->messages.last, (ListItem*)message);


  printf("\nHO SCRITTO IL MESSAGGIO %s\n", message->message);

  while(mq->waiting_to_read.size > 0){

    printf("FACCIO RIPARTIRE QUALCUNO CHE VOLEVA LEGGERE UNA MQ VUOTA");

        //Caccio il puntatore dallo struct di liste di ptr al PCB che deve leggere
        PCBPtr* reader_ptr = (PCBPtr*)List_detach(&mq->waiting_to_read, mq->waiting_to_read.first);
        //Rimuovo il processo che deve ripartire dalla lista dei processi in waiting
        List_detach(&waiting_list, (ListItem*)reader_ptr->pcb);

        PCB* reader = (PCB*) reader_ptr->pcb;
        reader -> status = Ready;

        //Inserisco il reader nella lista dei ready
        List_insert(&ready_list, ready_list.last, (ListItem*)reader_ptr->pcb);

    }

  mq -> num_written += 1;
  return;
}
