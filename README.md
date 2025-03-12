# Message Queues in DisastrOS

## What?  
The project aims to add a message queue to **DisastrOS**, an operating system simulator written in C.

## How?  
The project was made possible through the use of some predefined functions in **DisastrOS** and the creation of a structure that models a **Message Queue**:

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

An auxiliary structure was also created to model a **Message**:

```C
typedef struct {
  ListItem list;
  char message[MAX_LEN_MESSAGE];
  int length;
} Message;
```

To allow the OS to manage these structures, four new **system calls** were implemented, enabling opening, closing, writing, reading, and destroying a **Message Queue**:

```C
int disastrOS_openMessageQueue(int id, int mode)
int disastrOS_closeMessageQueue(int fd)
int disastrOS_MessageQueue_write(int id, char* write_buffer, int len)
int disastrOS_MessageQueue_read(int id, char* buf, int len)
int disastrOS_MessageQueue_destroy(int id)
```

These system calls have been appropriately added to the **System Call Vector**.

Finally, a **control mechanism** has been implemented to handle cases where:
- A **Message Queue is full** and a process wants to write to it → the process is placed in the **waiting list**.
- A **Message Queue is empty** and a process wants to read from it → the process is placed in the **waiting list**.

## How to run?  
In the **constants.h** file, the system can be configured as desired.  

- **Compilation command:** `make`  
- **Execution command:** `./disastrOS_test`
