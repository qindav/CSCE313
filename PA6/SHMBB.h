
#include <stdio.h>
#include <string.h> 
#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <sys/sem.h>
#include <sys/shm.h>

using namespace std;

//for n in `ipcs -m | grep ubuntu | awk '{print $2}'`; do ipcrm -m $n; done
//for n in `ipcs -s | grep ubuntu | awk '{print $2}'`; do ipcrm -s $n; done
//lsof | grep 1763147796

extern void EXITONERROR (string msg);


class KernelSemaphore{
private:
	int semid;
public: 
	KernelSemaphore (int _value, key_t _key);
	~KernelSemaphore();
	
	void P();
	void V();
};

class SHMBB
{

	private:
		int shmid;
		char *buf;
		KernelSemaphore *f;
		KernelSemaphore *e;
	public:
		void push(string msg);
		string pop();
		SHMBB(int projectid);
		~SHMBB();
		int get_shmid();
};
