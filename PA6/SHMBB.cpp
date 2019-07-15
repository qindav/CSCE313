
 
#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include "SHMBB.h"
using namespace std;

extern void EXITONERROR (string msg);

void SHMBB::push(string msg){
	e->P();
	strncpy(buf, msg.c_str(), msg.size());
	f->V();
}
string SHMBB::pop(){
	f->P();
	string s = buf;
	strcpy(buf, "    ");
	e->V();
	return s;
	
}

SHMBB::SHMBB(int projectid){
	key_t kshm = ftok("shmfile", projectid*10);
	key_t kf = ftok("shmfile", projectid*10+1);
	key_t ke = ftok("shmfile", projectid*10+2);
	
	shmid=shmget(kshm, 1024, 0666 | IPC_CREAT|IPC_EXCL); 
	if(shmid<0){
		shmid=shmget(kshm, 1024, 0666); 
	}

	
	buf= (char *)shmat(shmid, 0, 0);
	
	f = new KernelSemaphore(0, kf);
	e = new KernelSemaphore(1, ke);
}

SHMBB::~SHMBB(){
	
	shmctl(shmid, 0, IPC_RMID);

	delete f;
	delete e;
}


int SHMBB::get_shmid(){
	return shmid;
}

KernelSemaphore::KernelSemaphore(int _value, key_t _key){
	semid = semget(_key, 1, 0666 | IPC_CREAT | IPC_EXCL);
	if(semid<0){
		semid = semget(_key, 1, 0666);
	}
	else{
		struct sembuf sb = {0, _value, 0};
		semop(semid, &sb, 1);
	}
}

KernelSemaphore::~KernelSemaphore(){
	semctl(semid, 0, IPC_RMID);
}


void KernelSemaphore::P(){
	struct sembuf sb = {0, -1, 0};
	semop(semid, &sb, 1);
	
}
void KernelSemaphore::V(){
	struct sembuf sb = {0, 1, 0};
	semop(semid, &sb, 1);
}
