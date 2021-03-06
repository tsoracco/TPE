#include "producerConsumer.h"
#include "stdio.h"
#include "time.h"
#include "executer.h"
#include "random.h"
#include "ctype.h"
#include "fifo.h"

static void producer();
static void consumer();
static void control_speed();
static void print_speeds();
static void print_commands();

static int producerSleep = INITIAL_PROD_SLEEP;
static int consumerSleep = INITIAL_CONS_SLEEP;

void start_producer_consumer_problem(int buf_size) {
  int empty_fd = sys_fifo_op(PRODUCER_FIFO);
  int full_fd = sys_fifo_op(CONSUMER_FIFO);
  int prod_pid, cons_pid;
  int i;
  char empty = EMPTY_SLOT;

  /* Llena el fifo de slots vacíos con cantidad de mensajes igual al tamaño del buffer */
  for (i = 0; i < buf_size; i++)
    sys_write(empty_fd, &empty, 1);
    //write(empty_fd, &empty, 1);

  print_commands();

  cons_pid = execpn(consumer, "consumer");
  prod_pid = execpn(producer, "producer");
  

  control_speed();
  sys_kill(prod_pid);
  sys_kill(cons_pid);
  sys_fifo_cl(empty_fd);
  sys_fifo_cl(full_fd);
}

static void producer() {
  int empty_fd = sys_fifo_op(PRODUCER_FIFO);
  int full_fd = sys_fifo_op(CONSUMER_FIFO);

  char message;

  /* Lee espacios vacíos en empty_fd y escribe ítems en full_fd */
  while(1) {
    sys_read(empty_fd,  &message, 1);
    int item = rand_int();
    printf("Produced: %d\n", item);
    sleep(producerSleep * SLEEP_MULTIPLIER);
    sys_write(full_fd, &item, sizeof(int));
    //write(full_fd, &item, sizeof(int));
  }
}

static void consumer() {
  int empty_fd = sys_fifo_op(PRODUCER_FIFO);
  int full_fd = sys_fifo_op(CONSUMER_FIFO);

  int item;
  char message = EMPTY_SLOT;

  /* Lee ítems en full_fd y escribe slots vacíos en empt_fd */
  while(1) {
    sys_read(full_fd,  &item, sizeof(int));
    printf("Consumed: %d\n", item);
    sleep(consumerSleep * SLEEP_MULTIPLIER);
    sys_write(empty_fd, &message, 1);
    //write(empty_fd, &message, 1);
  }
}

static void control_speed() {
  char c;
  while((c = getchar())) {
    if (!isspace(c)) {
      switch (c) {
        case 'h':
          print_commands();
          break;
        case 'a':
          producerSleep++;
          print_speeds();
        break;
        case 's':
          producerSleep = producerSleep > 0 ? producerSleep-1 : 0;
          print_speeds();
        break;
        case 'z':
          consumerSleep++;
          print_speeds();
        break;
        case 'x':
          consumerSleep = consumerSleep > 0 ? consumerSleep-1 : 0;
          print_speeds();
        break;
        case 'q':
          getchar(); /* Limpia el \n que queda en el buffer */
          return;
        break;
        default:
          printf("Invalid command %c\n", c);
          print_commands();
          break;
      }
    }
  }
}

static void print_speeds() {
  printf("Producer sleep: %d\n", producerSleep);
  printf("Consumer sleep: %d\n", consumerSleep);
}

static void print_commands() {
  printf("\nCommands\n");
  printf("h   Prints this help.\n");
  printf("a   Increases producer sleep.\n");
  printf("s   Decreases producer sleep.\n");
  printf("z   Increases consumer sleep.\n");
  printf("x   Decreases consumer sleep.\n");
  printf("q   Ends producer consumer problem.\n");
  putchar('\n');
}
