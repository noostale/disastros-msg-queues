#include <assert.h>
#include <stdio.h>
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_message_queue.h"
#include "pool_allocator.h"
#include "linked_list.h"


#define MQ_SIZE sizeof(MessageQueue)
#define MQ_MEMSIZE (sizeof(MessageQueue)+sizeof(int))
#define MQ_BUFFER_SIZE MAX_NUM_RESOURCES*MQ_MEMSIZE //Dimensione massima del buffer

static char _mq_buffer[MQ_BUFFER_SIZE]; //Buffer che verrà usato
static PoolAllocator _mq_allocator; //Struttura del pool allocator mq

void MessageQueue_init(){
    int result=PoolAllocator_init(&_mq_allocator,
				  MQ_SIZE,
				  MAX_NUM_RESOURCES,
				  _mq_buffer,
				  MQ_BUFFER_SIZE);
    assert(!result);
}

MessageQueue* MessageQueue_alloc(){
  MessageQueue* mq=(MessageQueue*) PoolAllocator_getBlock(&_mq_allocator);
  if (!mq)
    return 0;
  mq->num_written=0;
  List_init(&mq->messages);
  return mq;
}

int MessageQueue_free(MessageQueue* mq) {
  //COMPLETARE
  return PoolAllocator_releaseBlock(&_mq_allocator, mq);
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
