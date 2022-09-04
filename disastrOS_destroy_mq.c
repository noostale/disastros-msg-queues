#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_mq.h"
#include "disastrOS_descriptor.h"

void internal_destroyMessageQueue(){
  int id=running->syscall_args[0];

  // find the mq in with the id
  MessageQueue* res=MessageQueueList_byId(&mq_list, id);
  if (! res){
    running->syscall_retvalue=DSOS_ERESOURCECLOSE;
    return;
  }

  // ensure the mq is not used by any process
  if(res->descriptors_ptrs.size){
    running->syscall_retvalue=DSOS_ERESOURCEINUSE;
    return;
  }

  res=(MessageQueue*) List_detach(&mq_list, (ListItem*) res);
  assert(res);
  MessageQueue_free(res);
  running->syscall_retvalue=0;
}
