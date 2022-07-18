#pragma once
#include "linked_list.h"
#include "disastrOS_resource.h"
#include "disastrOS_pcb.h"


/**
Aggiungo nel sistema operativo un nuovo struct Message il quale sarà
l'elemento che comporrà la lista di MessageQueue
**/

typedef struct {
  ListItem item; //Estensione di ListItem
  char* message; //Messaggio
  int sender; //PID di chi ha inviato il messaggio
} Message;

void Message_init();
Message* Message_alloc();
int Message_free(Message* m);


/** DA COMPLETARE PER LA FASE DI TEST

typedef ListHead ResourceList;

Resource* ResourceList_byId(ResourceList* l, int id);

void ResourceList_print(ListHead* l);

**/