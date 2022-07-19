#include <assert.h>
#include <stdio.h>
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_message_queue.h"
#include "pool_allocator.h"
#include "linked_list.h"
#include "disastrOS_message.h"


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


Resource* MessageQueue_alloc(){
  MessageQueue* mq=(MessageQueue*) PoolAllocator_getBlock(&_mq_allocator);

  if (!mq) return 0;
  
  mq->num_written=0;
  List_init(&mq->messages);

  return (Resource*)mq;
}


int MessageQueue_free(MessageQueue* mq) {
  ListItem* aux = mq->messages.first;

  while(aux){
    int free_check = Message_free((Message*)aux);
    if(free_check < 0)
      return -64;
    aux = aux -> next;
  }

  return PoolAllocator_releaseBlock(&_mq_allocator, mq);
}


void MQ_print(MessageQueue* mq){
  if(mq == NULL) return;

  ListItem* aux = mq -> messages.first;
  int i = 0;

  while(aux){
    printf("Messaggio n.%d: %s\n", i, (char*)(((Message*)aux)->message));
    i++;
    aux = aux->next;
  }

  return;
}


/**

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

**/
