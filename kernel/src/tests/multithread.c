#include <devices.h>
#include <common.h>
#include <klib.h>

#ifndef NO_TEST
static sem_t sem_p, sem_c, mutex;
void producer(void *arg) {
  device_t *tty = dev_lookup("tty1");
  while (1) {
    kmt->sem_wait(&sem_p);
    kmt->sem_wait(&mutex);
    tty->ops->write(tty, 0, "I love ", 7);
    printf("I love ");
    kmt->sem_signal(&mutex);
    kmt->sem_signal(&sem_c);
  }
}
void customer(void *arg) {
  device_t *tty = dev_lookup("tty1");
  while (1) {
    kmt->sem_wait(&sem_c);
    kmt->sem_wait(&mutex);
    tty->ops->write(tty, 0, (char *) arg, strlen((char *) arg));
    printf(arg);
    kmt->sem_signal(&mutex);
    kmt->sem_signal(&sem_p);
  }
}

void multithread_test_init(void){
  kmt->sem_init(&sem_p, "producer-sem", 1);
  kmt->sem_init(&sem_c, "customer-sem", 0);
  kmt->sem_init(&mutex, "mutex", 1);

  kmt->create(pmm->alloc(sizeof(task_t)), "p-task", producer, NULL);

  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "you\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "him\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "her\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "they\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "us\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "it\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "the god\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "american\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "europian\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "japanese\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "russian\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "indian\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "boy next door\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "fucking coming\n");
  kmt->create(pmm->alloc(sizeof(task_t)), "c-task", customer, "jyy and oslabs (X)\n");

}

#endif
