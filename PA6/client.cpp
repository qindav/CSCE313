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
#include <signal.h>

#include "reqchannel.h"
#include "FIFOreqchannel.h"
#include "MQreqchannel.h"
#include "SHMreqchannel.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
using namespace std;

// Special function to update the current table
Histogram hist;
static void DisplayHis(int sig, siginfo_t *siginfo, void *context){
   system("clear");
   hist.print();
}

/*--------------------------------------------------------------------------*/
/* Structure for Thread Arguements */
/*--------------------------------------------------------------------------*/

struct reqThread{
    int n;
    string clientName;
    BoundedBuffer *buffer;
};
struct workThread{
    int ID;
    int workload;

    BoundedBuffer *requestBuffer;
    RequestChannel* workerChannel;
    
    BoundedBuffer *b1;
    BoundedBuffer *b2;
    BoundedBuffer *b3;
};
struct statisticsThread{
    int n;
    string request;
    BoundedBuffer *responseBuffer;
    Histogram *hist;
};


/*--------------------------------------------------------------------------*/
/* Function Calls*/
/*--------------------------------------------------------------------------*/

void* request_thread_function(void* arg) {
	reqThread *input = (reqThread*)arg;
	
    for(int i = 0; i < (*input).n; ++i) {
     (*input).buffer->push("data " + (*input).clientName);
      //cout << (*input).clientName + "-push request:"<<i<<endl;
       
    }
    pthread_exit(NULL);
}
void* worker_thread_function(void* arg) {
    workThread *input = (workThread*)arg;
    RequestChannel* workerChannel = (*input).workerChannel;

    while(true) {
        for(int i=0; i<(*input).workload; ++i){
            string request = (*input).requestBuffer->pop();
            //cout<<"pop request:" + request <<endl;
            workerChannel->cwrite(request);
            
            if(request == "quit") {
                //delete workerChannel;
                break;
            }
            else{
                string response = workerChannel->cread();
                BoundedBuffer *rb;
                if(request == "data John Smith")
                    rb =  (*input).b1;
                else if(request == "data Jane Smith")
                    rb =  (*input).b2;
                else if(request == "data Joe Smith")
                    rb =  (*input).b3;
                    
                rb->push(response);
                //cout<<"Response with request w:" + request + " R:" +response<<endl;
    		}
        }
        break;
    }
}
void* stat_thread_function(void* arg) {
    statisticsThread *input = (statisticsThread*)arg;
    Histogram *hist = (*input).hist;
    
    for(int i = 0; i < (*input).n; ++i) {
        string response = (*input).responseBuffer->pop();
        hist->update ((*input).request, response);
        //cout<<(*input).request + ":update:" + response <<endl;
    }
   
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 2; //default number of requests per "patient"
    int w = 1; //default number of worker threads
    int b = 3 * n; // default capacity of the request buffer, you should change this default
    std::string channel_type = "s";
     
    int opt = 0;
    
    
    
    //Read arguements and set numbers
    while ((opt = getopt(argc, argv, "n:w:b:i:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg); //This won't do a whole lot until you fill in the worker thread function
                break;
            case 'b':
                b = atoi (optarg);
                break;
            case 'i':
                channel_type = optarg;
                break;
		}
    }
     

    //SIGNALS AND TIMERS
    struct sigaction act;
    act.sa_sigaction = DisplayHis;
    act.sa_flags = SA_SIGINFO;
    if(sigaction(SIGALRM, &act, NULL)<0){
         EXITONERROR ("");
    }
    timer_t timerid;
     
    struct sigevent sevt;
    sevt.sigev_notify = SIGEV_SIGNAL;
    sevt.sigev_signo=SIGALRM;
    sevt.sigev_value.sival_ptr = &timerid;
    
    timer_create(CLOCK_REALTIME, &sevt, &timerid);
    
    struct itimerspec value; 
    value.it_interval.tv_sec = 2; 
    value.it_interval.tv_nsec = 0; 
    value.it_value.tv_sec = 2; 
    value.it_value.tv_nsec = 0; 
   
    //cout << channel_type <<endl;
    char * serverarg = new char[2];
    serverarg[0] = channel_type[0];
    serverarg[1] = '\0';
    
    //PROCESS BEGINS HERE
    int pid = fork();
	if (pid == 0){
		execl("dataserver", serverarg);
	}
	else {
        cout << "n == " << n << endl;
        cout << "w == " << w << endl;
        cout << "b == " << b << endl;
        cout << "CLIENT STARTED:" << endl;
        cout << "Establishing control channel... " << flush;
        RequestChannel *chan;
        
        usleep(3000);
        if (channel_type=="f"){
            //FIFO channel
            chan = new FIFORequestChannel("control", RequestChannel::CLIENT_SIDE);
        }
        else if(channel_type=="q"){
            //Message Queue
            mkfifo("queuefile", 0600);
            chan = new MQRequestChannel("control", RequestChannel::CLIENT_SIDE);
        }
        else{
            //Shared memory
            mkfifo("shmfile", 0600);
            chan = new SHMRequestChannel("control", RequestChannel::CLIENT_SIDE);
        }
        cout << "done." << endl<< flush;
        
        //Begin timer
        auto start = std::chrono::steady_clock::now();
 
        BoundedBuffer request_buffer(b);
        timer_settime(timerid,0,&value, NULL);
		
		 //create 3 request threads
	 	statisticsThread input1,input2,input3;
        reqThread John, Joe, Jane;
        void *res;
        John.n = n; John.clientName = "John Smith";  John.buffer = &request_buffer;
        pthread_t thread_id_John;
	    if (pthread_create(& thread_id_John, NULL, request_thread_function, (void*)&John) < 0 ) {
		   EXITONERROR ("");
     	}

      	Jane.n = n; Jane.clientName = "Jane Smith"; Jane.buffer = &request_buffer;
     	pthread_t thread_id_Jane;
	    if (pthread_create(& thread_id_Jane, NULL, request_thread_function,(void*)&Jane) < 0 ) {
		   EXITONERROR ("");
     	}

	    Joe.n = n; Joe.clientName = "Joe Smith"; Joe.buffer = &request_buffer;
     	pthread_t thread_id_Joe;
	    if (pthread_create(& thread_id_Joe , NULL, request_thread_function, (void*)&Joe) < 0 ) {
		   EXITONERROR ("");
     	}
	
	  
	    vector<RequestChannel*> workerChannels;
	    vector<pthread_t> workingThreadIDs;
	    vector<BoundedBuffer *> responseBuffers;
	    
	    //Create bound buffers
	    BoundedBuffer* response_buffer1 = new BoundedBuffer(n);
	    BoundedBuffer* response_buffer2 = new BoundedBuffer(n);
	    BoundedBuffer* response_buffer3 = new BoundedBuffer(n);

	    RequestChannel* workerChannel;
	    
		pthread_t thread_id_Work;
		 //Create some work threads
		
	    for(int i = 0; i < w; ++i) {
	       
            chan->cwrite("newchannel");
            string s = chan->cread ();
           
            if (channel_type=="f"){
                workerChannel= new FIFORequestChannel(s, RequestChannel::CLIENT_SIDE);
            }
            else if(channel_type=="q"){
                workerChannel= new MQRequestChannel(s, RequestChannel::CLIENT_SIDE);
            }
            else{
                workerChannel= new SHMRequestChannel(s, RequestChannel::CLIENT_SIDE);
            }
            workerChannels.push_back(workerChannel);
          
            workThread inputs;
          
            inputs.workload = 3*n;
            inputs.ID = w; 
            inputs.workerChannel = workerChannel;
            inputs.requestBuffer = &request_buffer;
            inputs.b1 = response_buffer1;
            inputs.b2 = response_buffer2;
            inputs.b3 = response_buffer3;

            if (pthread_create(&thread_id_Work, NULL, worker_thread_function, (void*)&inputs) < 0 ) {
		        EXITONERROR ("");
     	    }
            workingThreadIDs.push_back(thread_id_Work);
        }
      
       
        
        //Creates stats
      
	
        input1.hist = &hist;
        input1.responseBuffer = response_buffer1;
        input1.n=n;
        input1.request = "data John Smith"; 
        
        input2.hist = &hist;
        input2.responseBuffer = response_buffer2;
        input2.n=n;
        input2.request = "data Jane Smith"; 
        
        input3.hist = &hist;
        input3.responseBuffer = response_buffer3;
        input3.n=n;
        input3.request = "data Joe Smith"; 
       
       //Create new threads
        
        pthread_t s1,s2,s3;
        pthread_create(&s1, NULL, stat_thread_function, (void*)&input1);
        pthread_create(&s2, NULL, stat_thread_function, (void*)&input2);
        pthread_create(&s3, NULL, stat_thread_function, (void*)&input3);
        
       
         
        //Join threads
        pthread_join(s1, 0);
        pthread_join(s2, 0);
        pthread_join(s3, 0);
        pthread_join(thread_id_John, 0);
        pthread_join(thread_id_Joe, 0);
        pthread_join(thread_id_Jane, 0);
        
        
        //Push Quits
        cout << "Done populating request buffer" << endl;
        cout << "Pushing quit requests... ";
        for(int i = 0; i < w; ++i) {
            request_buffer.push("quit");
        }
        cout << "done." << endl;
        

        //Join worker threads, delete control
        for(int i = 0; i < w; ++i) {
            pthread_t thread_id_Work = workingThreadIDs[i];
            pthread_join(thread_id_Work, 0);
        }
        
        //Delete channels
        for(int i = 0; i < w; ++i) {
             
            if (channel_type=="f"){
                FIFORequestChannel * workerChannel= (FIFORequestChannel *)workerChannels[i];
                delete workerChannel;
            }
            else if(channel_type=="q"){
                 MQRequestChannel * workerChannel= (MQRequestChannel *)workerChannels[i];
                delete workerChannel;
            }
            else{
                SHMRequestChannel * workerChannel= (SHMRequestChannel *)workerChannels[i];
                delete workerChannel;
            }
            
        }
        
        auto end = std::chrono::steady_clock::now();
        double nsec = std::chrono::nanoseconds(end - start).count();
        
        //delete control channel
        sleep(3000);
        chan->cwrite ("quit");
        if (channel_type=="f"){
            FIFORequestChannel * workerChannel= (FIFORequestChannel *)chan;
            delete workerChannel;
        }
        else if(channel_type=="q"){
            MQRequestChannel * workerChannel= (MQRequestChannel *)chan;
            delete workerChannel;
        }
        else{
            SHMRequestChannel * workerChannel= (SHMRequestChannel *)chan;
            delete workerChannel;
        }
        
        cout << "All Done!!!" << endl;
        cout << "Time taken: " << nsec/1000000000.0 << "s" << std::endl;
        hist.print ();
        
        //system("for n in `ipcs -s | grep ubuntu | awk '{print $2}'`; do ipcrm -s $n; done");
        //system("for n in `ipcs -m | grep ubuntu | awk '{print $2}'`; do ipcrm -m $n; done");
        //system("for n in `ipcs -q | grep ubuntu | awk '{print $2}'`; do ipcrm -q $n; done");
         
    }
    
 
}


