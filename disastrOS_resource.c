#include <assert.h>
#include <stdio.h>
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_message_queue.h"
#include "disastrOS_message.h"
#include "pool_allocator.h"
#include "linked_list.h"
#include "disastrOS_constants.h"

//Definisco un puntatore a funzione che restituisce un Resource*
static Resource* (*resource_alloc_func)(); 
//Definisco un puntatore a funzione che restituisce un int
static int (*resource_free_func)(Resource*);

void Resource_init(){
  resource_alloc_func = MessageQueue_alloc; //definisco quale sia la funzione per allocare una MQ
  resource_free_func = MessageQueue_free;  //definisco quale sia la funzione per fare il free di una MQ
}

Resource* Resource_alloc(int id, int type){
  if (type>0)
    return NULL;
  Resource* r = (*resource_alloc_func)();

  if (!r)
    return 0;
  r->list.prev=r->list.next=0;
  r->id=id;
  r->type=type;
  List_init(&r->descriptors_ptrs);
  return r;
}

int Resource_free(Resource* r) {
  assert(r->descriptors_ptrs.first==0);
  assert(r->descriptors_ptrs.last==0);
  return (*resource_free_func)(r);
}

Resource* ResourceList_byId(ResourceList* l, int id) {
  ListItem* aux=l->first;
  while(aux){
    Resource* r=(Resource*)aux;
    if (r->id==id)
      return r;
    aux=aux->next;
  }
  return 0;
}

void Resource_print(Resource* r) {
  printf("id: %d, type:%d, pids:", r->id, r->type);
  DescriptorPtrList_print(&r->descriptors_ptrs);
}

void ResourceList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("{\n");
  while(aux){
    Resource* r=(Resource*)aux;
    printf("\t");
    Resource_print(r);
    if(aux->next)
      printf(",");
    printf("\n");
    aux=aux->next;
  }
  printf("}\n");
}
