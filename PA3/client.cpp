/*
    Based on original assignment by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31
 */


#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

#include <sys/time.h>
#include <cassert>
#include <assert.h>
#include <chrono>

#include <cmath>
#include <numeric>
#include <algorithm>

#include <list>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "reqchannel.h"
#include "SafeBuffer.h"
#include "Histogram.h"
using namespace std;

struct reqThread{
    int n;
    string clientName;
    SafeBuffer *buffer;
};

struct workThread{
    SafeBuffer *buffer;
    RequestChannel* workerChannel;
    Histogram *hist;
};

void* request_thread_function(void* arg) {
	reqThread *input = (reqThread*)arg;
	//cout << "push request";
	
    for(int i = 0; i < (*input).n; ++i) {
        (*input).buffer->push("data " + (*input).clientName);
    }
}

void* worker_thread_function(void* arg) {
    
    workThread *input = (workThread*)arg;
    
    RequestChannel* workerChannel = (*input).workerChannel;
	Histogram *hist = (*input).hist;
	
    while(true) {
        string request = (*input).buffer->pop();
		workerChannel->cwrite(request);
		
		

		if(request == "quit") {
			delete workerChannel;
            break;
        }
        else{
			string response = workerChannel->cread();
			hist->update (request, response);
		}
        
    }
    
}



/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 100; //default number of requests per "patient"
    int w = 1; //default number of worker threads
    int opt = 0;
    
    
    auto start = std::chrono::steady_clock::now();
    while ((opt = getopt(argc, argv, "n:w:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg); //This won't do a whole lot until you fill in the worker thread function
                break;
			}
    }

    int pid = fork();
	if (pid == 0){
		execl("dataserver", (char*) NULL);
	}
	else {
        
        cout << "n == " << n << endl;
        cout << "w == " << w << endl;

        cout << "CLIENT STARTED:" << endl;
        cout << "Establishing control channel... " << flush;
        RequestChannel *chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
        cout << "done." << endl<< flush;

        
        //cout <<"ddddd";
		SafeBuffer request_buffer;
		Histogram hist;
		
		
		reqThread John;
		reqThread Joe,Jane;
		
        //create 3 request threads
        void *res;
        
        John.n = n; 
        John.clientName = "John Smith"; 
        John.buffer = &request_buffer;
        
        pthread_t thread_id_John;
	    if (pthread_create(& thread_id_John, NULL, request_thread_function, (void*)&John) < 0 ) {
		   EXITONERROR ("");
     	}
     
     	Joe.n = n; Joe.clientName = "Joe Smith"; Joe.buffer = &request_buffer;
     	pthread_t thread_id_Joe;
	    if (pthread_create(& thread_id_Joe , NULL, request_thread_function, (void*)&Joe) < 0 ) {
		   EXITONERROR ("");
     	}
     	
     	Jane.n = n; Jane.clientName = "Jane Smith"; Jane.buffer = &request_buffer;
     	pthread_t thread_id_Jane;
	    if (pthread_create(& thread_id_Jane, NULL, request_thread_function,(void*)&Jane) < 0 ) {
		   EXITONERROR ("");
     	}
     	
     	
        pthread_join(thread_id_John, &res);
        free(res);
        pthread_join(thread_id_Joe, &res);
        free(res);
        pthread_join(thread_id_Jane, &res);
        free(res);
        
        cout << "Done populating request buffer" << endl;
        cout << "Pushing quit requests... ";
        for(int i = 0; i < w; ++i) {
            request_buffer.push("quit");
        }
        cout << "done." << endl;

	
	    //Create w working chanels
	    vector<RequestChannel *> workerChannels;
	    vector<pthread_t> workingThreadIDs;
	    
	    RequestChannel* workerChannel;
		pthread_t thread_id_Work;
		
	    for(int i = 0; i < w; ++i) {
	        
            chan->cwrite("newchannel");
            string s = chan->cread ();
            
            
            workerChannel= new RequestChannel(s, RequestChannel::CLIENT_SIDE);
            workerChannels.push_back(workerChannel);
            
            workThread inputs;
          
            inputs.hist = &hist; 
            inputs.workerChannel = workerChannel;
            inputs.buffer = &request_buffer;
            
            if (pthread_create(&thread_id_Work, NULL, worker_thread_function, (void*)&inputs) < 0 ) {
              
		        EXITONERROR ("");
     	    }
     	    
            workingThreadIDs.push_back(thread_id_Work);
        }
        
        //Join working threads
        //cout<<workingThreadIDs.size()<<endl;
        
       for(int i = 0; i < w; ++i) {
            pthread_t thread_id_Work = workingThreadIDs[i];
            pthread_join(thread_id_Work, &res);
            free(res);
        }
       
		
        //delete control
        chan->cwrite ("quit");
        delete chan;
        cout << "All Done!!!" << endl;
        auto end = std::chrono::steady_clock::now();
        double nsec = std::chrono::nanoseconds(end - start).count();
        cout << "Time taken: " << nsec/1000000000.0 << "s" << std::endl;
		hist.print ();
    }
}
