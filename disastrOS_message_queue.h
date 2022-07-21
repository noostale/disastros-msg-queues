#pragma once
#include "linked_list.h"
#include "disastrOS_resource.h"
#include "disastrOS_pcb.h"


/**
Aggiungo nel sistema operativo un nuovo struct MessageQueue il quale è
un estensione di Resource. Lo struct conterrà una lista di messaggi ed
un numero che rappresenterà il numero di messaggi scritti.
**/

typedef struct {
  Resource resource; //Estensione di resource
  ListHead messages; //lista dei messaggi scritti
  int num_written; //numero di messaggi scritti
  ListHead waiting_to_read; //lista di processi in attesa di leggere
  ListHead waiting_to_write; //lista di processi in attesa di scrivere
} MessageQueue;

void MessageQueue_init();
Resource* MessageQueue_alloc();
int MessageQueue_free(Resource* r);
void MQ_print(MessageQueue* mq);


/** DA COMPLETARE PER LA FASE DI TEST

typedef ListHead ResourceList;

Resource* ResourceList_byId(ResourceList* l, int id);

void ResourceList_print(ListHead* l);

**/