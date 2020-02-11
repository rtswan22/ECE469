#ifndef __USERPROG__
#define __USERPROG__

/* typedef struct missile_code {
  int numprocs;
  char really_important_char;
} missile_code; */

#define BUFFER_SIZE 10 

typedef struct buffer {
  int head;
  int tail;
  char item[BUFFER_SIZE];  //NOTE: size is arbitrary
} buffer;

//insert code
/*
char produced_item; //ASSIGN TO CHAR IN STRING
if((circ_buff->head + 1) % BUFFER_SIZE != tail){ // if not full
  circ_buff->item[circ_buff->head] = produced_item;
  circ_buff->head = (circ_buff->head + 1) % BUFFER_SIZE;
}
*/
//remove code
/*
char consumed_item;
if(circ_buff->head != circ_buff->tail) { // if not empty
  consumed_item = circ_buff->item[circ_buff->tail];
  circ_buff->tail = (circ_buff->tail + 1) % BUFFER_SIZE;
}
*/

#define PRODUCER_TO_RUN "producer.dlx.obj"
#define CONSUMER_TO_RUN "consumer.dlx.obj"

#endif
