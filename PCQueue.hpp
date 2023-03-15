#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
// Single Producer - Multiple Consumer queue
template <typename T>class PCQueue
{

public:
    PCQueue() {
        pthread_mutex_init(&mutex,NULL);
        pthread_cond_init(&cond,NULL);
        queue1 = new queue<T>();
    }
    ~PCQueue() {
        delete queue1;
    }
    // Blocks while queue is empty. When queue holds items, allows for a single
    // thread to enter and remove an item from the front of the queue and return it.
    // Assumes multiple consumers.
    T pop() {
        pthread_mutex_lock(&(mutex));
        while(queue1->empty()) {
            pthread_cond_wait(&cond,&mutex);
        }
        T item=queue1->front();
        queue1->pop();
        pthread_mutex_unlock(&(mutex));
        return item;
    }

    // Allows for producer to enter with *minimal delay* and push items to back of the queue.
    // Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.
    // Assumes single producer
    void push(const T& item) {
        pthread_mutex_lock(&(mutex));
        queue1->push(item);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&(mutex));
    }


private:
    queue<T>* queue1;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    // Add your class memebers here
};
// Recommendation: Use the implementation of the std::queue for this exercise
#endif
