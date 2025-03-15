
#include "../include/Semaphore.hpp"

Semaphore::Semaphore() {
    counter=0;
    pthread_cond_init(&cond,NULL);
    pthread_mutex_init(&mutex,NULL);
}

Semaphore::Semaphore(unsigned int val) {
    counter=val;
    pthread_cond_init(&cond,NULL);
    pthread_mutex_init(&mutex,NULL);
}

void Semaphore::up() {
    pthread_mutex_lock(&(mutex));
    counter++;
    pthread_cond_broadcast(&(cond)); 
    pthread_mutex_unlock(&(mutex));
}

void Semaphore::down() {
    pthread_mutex_lock(&(mutex));
    while (counter == 0) {
        pthread_cond_wait(&(cond),&(mutex));
    }
    counter--;
    pthread_mutex_unlock(&(mutex));
}
