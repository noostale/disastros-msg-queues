#include <assert.h>
#include <stdio.h>
#include "disastrOS_mq.h"
#include "disastrOS_descriptor.h"
#include "pool_allocator.h"
#include "linked_list.h"

#define MESSAGEQUEUE_SIZE sizeof(MessageQueue)
#define MESSAGEQUEUE_MEMSIZE (sizeof(MessageQueue)+sizeof(int))
#define MESSAGEQUEUE_BUFFER_SIZE MAX_NUM_MESSAGEQUEUES*MESSAGEQUEUE_MEMSIZE

static char _messagequeues_buffer[MESSAGEQUEUE_BUFFER_SIZE];
static PoolAllocator _messagequeues_allocator;

void MessageQueue_init(){
    int result=PoolAllocator_init(& _messagequeues_allocator,
				  MESSAGEQUEUE_SIZE,
				  MAX_NUM_MESSAGEQUEUES,
				  _messagequeues_buffer,
				  MESSAGEQUEUE_BUFFER_SIZE);
    assert(! result);
}

MessageQueue* MessageQueue_alloc(int id){
  MessageQueue* r=(MessageQueue*) PoolAllocator_getBlock(&_messagequeues_allocator);
  if (!r)
    return 0;
  r->list.prev=r->list.next=0;
  r->id=id;
  r->num_written = 0;
  List_init(&r->descriptors_ptrs);
  List_init(&r->messages);
  List_init(&r->waiting_to_read);
  List_init(&r->waiting_to_write);
  return r;
}

int MessageQueue_free(MessageQueue* r) {
  assert(r->descriptors_ptrs.first==0);
  assert(r->descriptors_ptrs.last==0);
  return PoolAllocator_releaseBlock(&_messagequeues_allocator, r);
}

MessageQueue* MessageQueueList_byId(MessageQueueList* l, int id) {
  ListItem* aux=l->first;
  while(aux){
    MessageQueue* r=(MessageQueue*)aux;
    if (r->id==id)
      return r;
    aux=aux->next;
  }
  return 0;
}

void MessageQueue_print(MessageQueue* r) {
  printf("mq di id: %d, contiene %d messaggi, i suoi valori sono:", r->id, r->num_written);
  DescriptorPtrList_print_mq(&r->descriptors_ptrs);
  if(r == NULL)
    return;

  /**
  
  ListItem* m = r -> messages.first;
  int i = 0;
  if(m == NULL){
    printf("\nEmpty message queue.\n");
  }
  while(m){
    printf("\n%d\n",r->id);
    sleep(10);
    printf("MESSAGGIO %d: %s \n", i, (char*)(((Message*)m)->message));
    i++;
    m = m -> next;
  }
  //MessageList_print(&(r->messages));
  **/
}

void MessageQueueList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("{\n");
  while(aux){
    MessageQueue* r=(MessageQueue*)aux;
    printf("\t");
    MessageQueue_print(r);
    if(aux->next)
      printf(",");
    printf("\n");

    

    aux=aux->next;
  }
  printf("}\n");
}

//Messaggi

#define MESSAGE_SIZE sizeof(Message)
#define MESSAGE_MEMSIZE (sizeof(Message)+sizeof(int))
#define MESSAGE_BUFFER_SIZE MAX_NUM_MESSAGES*MESSAGE_MEMSIZE

static char _messages_buffer[MESSAGE_BUFFER_SIZE];
static PoolAllocator _messages_allocator;

void Message_init(){
    int result=PoolAllocator_init(& _messages_allocator,
				  MESSAGE_SIZE,
				  MAX_NUM_MESSAGES,
				  _messages_buffer,
				  MESSAGE_BUFFER_SIZE);
    assert(! result);
}

Message* Message_alloc(char* message, int length){
  Message* r=(Message*) PoolAllocator_getBlock(&_messages_allocator);
  if (!r)
    return 0;
  r->list.prev=r->list.next=0;
  r->length=length;
  for (int i=0; i<length; i++) r -> message[i] = message[i];
  return r;
}

int Message_free(Message* r) {
  return PoolAllocator_releaseBlock(&_messages_allocator, r);
}

/** NON PENSO SERVA
Message* Message_byId(ResourceList* l, int id) {
  ListItem* aux=l->first;
  while(aux){
    Resource* r=(Resource*)aux;
    if (r->id==id)
      return r;
    aux=aux->next;
  }
  return 0;
}
**/

void Message_print(Message* r) {
  printf("MESSAGGIO: '%s' di lunghezza %d", r->message, r->length);
}


void MessageList_print(ListHead* l){
  
  ListItem* aux=l->first;
  printf("{\n");
  while(aux){
    sleep(40);
    Message* r=(Message*)aux;
    printf("\t");
    Message_print(r);
    if(aux->next)
      printf(",");
    printf("\n");
    aux=aux->next;
  }
  printf("}\n");
}

