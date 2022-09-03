# Message Queues in DisastrOS - Frasca Emanuele 

## What?
Il progetto si prefigge di aggiungere una message queue a DisastrOS.

## How?
Il progetto è stato reso possibile grazie all'utilizzo di alcune funzioni preimpostate in DisastrOS ed alla creazione di una struttura la quale modella una Message Queue.
```C
typedef struct {
  ListItem list;
  int id;
  int num_written;
  ListHead descriptors_ptrs;
  ListHead messages;
  ListHead waiting_to_read;
  ListHead waiting_to_write;
} MessageQueue;
```
 È stata poi creata una struttura accessoria che modella un messaggio 
```C
typedef struct {
  ListItem list;
  char message[MAX_LEN_MESSAGE];
  int length;
} Message;
```

Per far gestire al SO queste strutture sono state installate quattro nuove system call le quali permettono di aprire, chiudere, scrivere o leggere un messaggio.

```C
int disastrOS_openMessageQueue(int id, int mode)
int disastrOS_closeMessageQueue(int fd)
int disastrOS_MessageQueue_write(int id, char* write_buffer, int len)
int disastrOS_MessageQueue_read(int id, char* buf, int len)
```
Ed esse sono state opportunamente aggiunte al System Call Vector.

Infine è stato implementato un sistema di controllo che permette di capire se una Message Queue è piena e qualcuno vuole scrivergli o se una MQ è vuota e qualcuno vuole leggervi mettendo il suddetto processo in waiting list.

## How to run?
Nel file constants.h è possibile settare il sistema come si preferisce  
**Comando di compilazione:** make  
**Comando di esecuzione:** ./disastrOS_test

