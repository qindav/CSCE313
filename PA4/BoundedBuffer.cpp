#include "BoundedBuffer.h"
#include <string>
#include <queue>
using namespace std;

BoundedBuffer::BoundedBuffer(int _cap) {
	pthread_mutex_init (&mut, NULL);
	pthread_cond_init(&cond1, NULL);
	pthread_cond_init(&cond2, NULL);
	cap = _cap;

}

BoundedBuffer::~BoundedBuffer() {
	pthread_mutex_destroy (&mut);
	pthread_cond_destroy(&cond1);
	pthread_cond_destroy(&cond1);
}

int BoundedBuffer::size() {
	pthread_mutex_lock (&mut);
	int length = q.size();
	pthread_mutex_unlock (&mut);
	return length;
}

void BoundedBuffer::push(string str) {
 
	pthread_mutex_lock (&mut);
	while(q.size()==cap){
		
		pthread_cond_wait(&cond1, &mut);
	}
 
	q.push (str);
	pthread_mutex_unlock (&mut);
	pthread_cond_signal(&cond2);
 
}

string BoundedBuffer::pop() {
	pthread_mutex_lock (&mut); 
	while(q.size()==0){
		pthread_cond_wait(&cond2, &mut);
	}
	string s = q.front();
	q.pop();
	pthread_mutex_unlock (&mut);
	pthread_cond_signal(&cond1);
	return s;
}

