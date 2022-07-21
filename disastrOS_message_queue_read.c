#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_message_queue.h"
#include "disastrOS_message.h"

void internal_MessageQueue_read(){

    //Estraggo i parametri dalla system call
    int fd = running -> syscall_args[0];
    char* read_buffer = (char*)running -> syscall_args[1];
    int buffer_length = running -> syscall_args[2];
    
    Descriptor* mq_destination = DescriptorList_byFd(&running -> descriptors, fd);
    MessageQueue* mq = (MessageQueue*) mq_destination -> resource;

    Message* first_message;
    if(mq==NULL) { first_message = NULL; }
    else {first_message = (Message*)mq -> messages.first;}

    

    //Controllo se la MQ ha messaggi da leggere, se non li ha:
    //1.Metto il processo in attesa
    //2.Inserisco il processo nella lista dei processi in attesa
    //3.Inserisco il processo nella lista dei processi in attesa di leggere
    //4.Selezioni come runner il primo processo nella ready list

    if(mq->messages.first == NULL){

        running->status=Waiting;
        List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
        List_insert(&mq->waiting_to_read, mq->waiting_to_read.last, (ListItem*) PCBPtr_alloc(running));

        PCB* next_running= (PCB*) List_detach(&ready_list, ready_list.first);
        next_running->status = Running;
        running = next_running;
        return;
    }

    //Salvo il puntatore al primo elemento del messaggio in m
    Message* m = (Message*)List_detach(&mq->messages, mq->messages.first);

    int message_length = first_message -> length;

    //Controllo che il messaggio che voglio leggere non sia troppo lungo
    if(message_length > buffer_length+1){
        running -> syscall_retvalue = DSOS_EMESSAGETOOLONG;
        return;
    }

    //Scrivo il messaggio nel buffer di lettura
    for(int i = 0; i<message_length; i++){
        read_buffer[i]= m -> message[i];
    }

    //Quando leggo un messaggio e qualcuno che deve scrivere è rimasto in attesa,
    //lo faccio ripartire

    while(mq->waiting_to_write.size > 0){

        //Caccio il puntatore dallo struct di liste di ptr al PCB che deve scrivere
        PCBPtr* writer_prt = (PCBPtr*)List_detach(&mq->waiting_to_write, mq->waiting_to_write.first);
        //Rimuovo il processo che deve ripartire dalla lista dei processi in waiting
        List_detach(&waiting_list, (ListItem*)writer_prt->pcb);

        PCB* writer = (PCB*) writer_prt->pcb;
        writer -> status = Ready;

        //Inserisco il writer nella lista dei ready
        List_insert(&ready_list, ready_list.last, (ListItem*)writer_prt->pcb);

    }

    mq->num_written -= 1;
    //Setto come valore di ritorno della read la lunghezza del messaggio letto
    running->syscall_retvalue = message_length;
    return;
    
}
