#include <assert.h>
#include <stdio.h>
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_message_queue.h"
#include "disastrOS_message.h"
#include "pool_allocator.h"
#include "linked_list.h"

#define M_SIZE sizeof(Message)
#define M_MEMSIZE (sizeof(Message)+sizeof(int))
#define M_BUFFER_SIZE MAX_NUM_MESSAGES*M_MEMSIZE //Dimensione massima del buffer

static char _m_buffer[M_BUFFER_SIZE]; //Buffer che verrà usato
static PoolAllocator _m_allocator; //Struttura del pool allocator m

void Message_init(){
    int result=PoolAllocator_init(&_m_allocator,
				  M_SIZE,
				  MAX_NUM_MESSAGES,
				  _m_buffer,
				  M_BUFFER_SIZE);
    assert(!result);
}

Message* Message_alloc(int sender_pid, char* message, int length){
  Message* out_mex = (Message*) PoolAllocator_getBlock(&_m_allocator);

  //Se il pool allocator da errore, esci
  if (!out_mex) return 0;
  
  //Riempi i campi dello struct Message
  out_mex -> sender = sender_pid;
  for (int i=0; i<length; i++) out_mex -> message[i] = message[i];
  out_mex -> length = length;

  return out_mex;
}

int Message_free(Message* m) {
  //COMPLETARE
  return PoolAllocator_releaseBlock(&_m_allocator, m);
}

/** COMPLETARE PER LA FASE DI TEST

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

**/
