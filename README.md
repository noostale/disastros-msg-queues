# Message Queues in DisastrOS - Frasca Emanuele 

## What?
Il progetto si prefigge di aggiungere una message queue a DisastrOS.

## How?
Il progetto è stato reso possibile grazie all'utilizzo di alcune strutture preimpostate in DisastrOS. È stata infatti utilizzata la struttura Resource e le annesse funzioni opportunatamente modificate per creare e manipolare una nuova struttura che modella una Message Queue.
```C
typedef struct {
  Resource resource; //Estensione di resource
  ListHead messages; //lista dei messaggi scritti
  int num_written; //numero di messaggi scritti
  ListHead waiting_to_read; //lista di processi in attesa di leggere
  ListHead waiting_to_write; //lista di processi in attesa di scrivere
} MessageQueue;
```
 È stata poi creata una struttura accessoria che modella un messaggio 
```C
typedef struct {
  ListItem item; //Estensione di ListItem
  char message[MAX_LEN_MESSAGE]; //Messaggio
  int sender; //PID di chi ha inviato il messaggio
  int length; //Lunghezza del messaggio
} Message;
```

Per far gestire al SO queste strutture sono state installate due nuove system call le quali permettono di scrivere o leggere un messaggio.

```C
int disastrOS_readMessageQueue(int fd, char* read_buffer, int buffer_length)
int disastrOS_writeMessageQueue(int fd, char* write_buffer, int message_length)
```
Ed esse sono state opportunamente aggiunte al System Call Vector  
È stato modificato disastrOS_resource.c in modo tale da supportare l'allocazione ed il free delle Message queues.

Infine è stato implementato un sistema di controllo che permette di capire se una Message Queue è piena e qualcuno vuole scrivergli o se una MQ è vuota e qualcuno vuole leggervi mettendo il suddetto processo in waiting list.

## How to run?
Nel file constants.h è possibile settare il sistema come si preferisce  
**Comando di compilazione:** make  
**Comando di esecuzione:** ./disastrOS_test

