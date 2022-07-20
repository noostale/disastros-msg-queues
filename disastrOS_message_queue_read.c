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

    //Trovo la lunghezza del messaggio
    int message_length = ((Message*)mq -> messages.first) -> length;

    //Controllo che il messaggio che voglio leggere non sia troppo lungo
    if(message_length > buffer_length+1){
        running -> syscall_retvalue = DSOS_EMESSAGETOOLONG;
        return;
    }

    //Controllo se la MQ ha messaggi da leggere, se non li ha:
    //1.Metto il processo in attesa
    //2.Inserisco il processo nella lista dei processi in attesa
    //3.Selezioni come runner il primo processo nella ready list

    if(mq->messages.first == NULL){

        running->status=Waiting;
        List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
        running = (PCB*) List_detach(&ready_list, ready_list.first);
        return;
    }

    //Salvo il puntatore al primo elemento del messaggio in m
    Message* m = (Message*)List_detach(&mq->messages, mq->messages.first);

    //Scrivo il messaggio nel buffer di lettura
    for(int i = 0; i<message_length; i++){
        read_buffer[i]= m -> message[i];
    }

    running->syscall_retvalue = message_length;
    return;
    
}
