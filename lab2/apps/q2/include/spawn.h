#ifndef __USERPROG__
#define __USERPROG__

/* typedef struct missile_code {
  int numprocs;
  char really_important_char;
} missile_code; */

typedef struct buffer {
  char *head;
  char *tail;
  char item[10];  //CHECK buffer size?
} buffer;

//CHECK the below define statements
#define PRODUCER_TO_RUN "producer.dlx.obj"
#define CONSUMER_TO_RUN "consumer.dlx.obj"

#endif
