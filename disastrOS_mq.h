#pragma once
#include "linked_list.h"
#include "disastrOS_pcb.h"
#include "disastrOS_constants.h"


typedef struct {
  ListItem list;
  int id;
  ListHead descriptors_ptrs;
  ListHead messages;
  int num_written;
  ListHead waiting_to_read;
  ListHead waiting_to_write;
} MessageQueue;

void MessageQueue_init();

MessageQueue* MessageQueue_alloc(int id);
int MessageQueue_free(MessageQueue* resource);

typedef ListHead MessageQueueList;

MessageQueue* MessageQueueList_byId(MessageQueueList* l, int id);

void MessageQueueList_print(ListHead* l);


//Messaggi

typedef struct {
  ListItem list;
  char message[MAX_LEN_MESSAGE];
  int length;
} Message;

void Message_init();

Message* Message_alloc(char* message, int length);
int Message_free(Message* resource);

typedef ListHead MessageList;

Message* MessageList_byId(MessageList* l, int id);

void MessageList_print(ListHead* l);
