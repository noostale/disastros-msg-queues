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
    char* read_buffer = running -> syscall_args[1];
    
    Descriptor* mq_destination = DescriptorList_byFd(&running -> descriptors, fd);
    MessageQueue* mq = (MessageQueue*) mq_destination -> resource;

    //Trovo la lunghezza del messaggio
    int message_length = ((Message*)mq -> messages.first) -> length;

    //Salvo il puntatore al primo elemento del messaggio in m
    Message* m = List_detach(&mq->messages, mq->messages.first);

    //Scrivo il messaggio nel buffer di lettura
    for(int i = 0; i<message_length; i++){
        read_buffer[i]= m -> message[i];
    }

    running->syscall_retvalue = 0;
    return;
    
}
