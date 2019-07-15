#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include <stdio.h>
#include <queue>
#include <string>
#include <pthread.h>
using namespace std;

class BoundedBuffer {
private:
	pthread_mutex_t mut;
	queue<string> q;
	int cap;
	pthread_cond_t cond1;
    pthread_cond_t cond2;
public:
    BoundedBuffer(int);
	~BoundedBuffer();
	int size();
    void push (string);
    string pop();

   
    
};

#endif /* BoundedBuffer_ */
