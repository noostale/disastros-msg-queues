#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include <assert.h>


void internal_MessageQueue_read() {
  printf("Il processo con ID: %d sta per leggere un messaggio della Message Queue con ID: %ld\n",
         running->pid, running->syscall_args[0]);

  //1 get from the PCB the resource id of the resource to open
  int id=running->syscall_args[0];
  char* buf_des = (char*)running -> syscall_args[1];
  int m_length=running->syscall_args[1];


  /**
  Resource* res=ResourceList_byId(&resources_list, id);
  //2 if the resource is opened in CREATE mode, create the resource
  //  and return an error if the resource is already existing
  // otherwise fetch the resource from the system list, and if you don't find it
  // throw an error
  //printf ("CREATING id %d, type: %d, open mode %d\n", id, type, open_mode);
  if (open_mode&DSOS_CREATE){
    if (res) {
      running->syscall_retvalue=DSOS_ERESOURCECREATE;
      return;
    }
    res=Resource_alloc(id, type);
    List_insert(&resources_list, resources_list.last, (ListItem*) res);
  }

  // at this point we should have the resource, if not something was wrong
  if (! res || res->type!=type) {
     running->syscall_retvalue=DSOS_ERESOURCEOPEN;
     return;
  }
  
  if (open_mode&DSOS_EXCL && res->descriptors_ptrs.size){
     running->syscall_retvalue=DSOS_ERESOURCENOEXCL;
     return;
  }

  
  //5 create the descriptor for the resource in this process, and add it to
  //  the process descriptor list. Assign to the resource a new fd
  Descriptor* des=Descriptor_alloc(running->last_fd, res, running);
  if (! des){
     running->syscall_retvalue=DSOS_ERESOURCENOFD;
     return;
  }
  running->last_fd++; // we increment the fd value for the next call
  DescriptorPtr* desptr=DescriptorPtr_alloc(des);
  List_insert(&running->descriptors, running->descriptors.last, (ListItem*) des);
  
  //6 add to the resource, in the descriptor ptr list, a pointer to the newly
  //  created descriptor
  des->ptr=desptr;
  List_insert(&res->descriptors_ptrs, res->descriptors_ptrs.last, (ListItem*) desptr);

  // return the FD of the new descriptor to the process
  running->syscall_retvalue = des->fd;
  **/

  //Descriptor* mq_des = DescriptorList_byFd(&running -> descriptors, id); //modificato attento

  //MessageQueue* mq = mq_des -> mq; //modificato attento

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

    printf("Il processo con ID: %d ha provato a leggere un messaggio da una Message Queue vuota, viene sospeso\n",
          running->pid);

    running->status=Waiting;
    List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
    List_insert(&mq->waiting_to_read, mq->waiting_to_read.last, (ListItem*) PCBPtr_alloc(running));
    PCBPtr_alloc(running);


    PCB* next_running= (PCB*) List_detach(&ready_list, ready_list.first);
    next_running->status = Running;
    running = next_running;
    
    return;

  }

  if((ListItem*)first_message == NULL) printf("TEST\n\n\n\n");

  List_detach(&mq->messages, (ListItem*)first_message);

  printf("Il processo con ID: %d ha appena letto un messaggio dalla Message Queue di ID: %d, \"%s\"\n",
         running->pid, id, first_message->message);


  while(mq->waiting_to_write.size > 0){
    printf("Il processo con ID: %d riattiva un writer che era precedentemente in attesa\n", running->pid);

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
  
  mq -> num_written -= 1;
  running -> syscall_retvalue = m_length;

  return;
}
