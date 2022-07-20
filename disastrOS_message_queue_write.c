#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_message_queue.h"
#include "disastrOS_message.h"
#include "disastrOS_constants.h"

void internal_MessageQueue_write(){

    //Estraggo i parametri dalla system call
    int fd = running -> syscall_args[0];
    char* write_buffer = (char*)running -> syscall_args[1];
    int message_length = running -> syscall_args[2];

    //Controllo che il messaggio che voglio scrivere non sia troppo lungo
    if(message_length > MAX_LEN_MESSAGE){
        running -> syscall_retvalue = DSOS_EMESSAGETOOLONG;
        return;
    }

    //Trovo il descrittore a partire dal fd
    Descriptor* des = DescriptorList_byFd(&running -> descriptors, fd);
    if(des == NULL){
        running -> syscall_retvalue = DSOS_ERESOURCENOFD;
        return;
    }
    MessageQueue* mq = (MessageQueue*) des -> resource;

    //Controllo se la MQ ha raggiunto il massimo numero di messaggi e se è il caso:
    //1.Metto il processo in attesa
    //2.Inserisco il processo nella lista dei processi in attesa
    //3.Selezioni come runner il primo processo nella ready list

    if(mq->num_written == MAX_NUM_MESSAGES){

        running->status=Waiting;
        List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
        running = (PCB*) List_detach(&ready_list, ready_list.first);
        return;

    }

    //Allochiamo un messaggio 
    Message* message = Message_alloc(running -> pid, write_buffer, message_length);

    //Controllo se è stato possibile allocare il messaggio
    if (message == 0){
        running -> syscall_retvalue = DSOS_EMESSAGEALLOC;
        return;
    }

    //Inseriamo il messaggio nella lista di messaggi della MQ, dopo l'ultimo elemento
    List_insert(&mq->messages, mq->messages.last, (ListItem*)message);

    mq -> num_written += 1;


    //Setto come valore di ritorno della write la lunghezza del messaggio scritto
    running->syscall_retvalue = message_length;
    return;

}
