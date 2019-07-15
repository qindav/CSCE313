
#ifndef _MQreqchannel_H_                   
#define _MQreqchannel_H_

#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include "reqchannel.h"

using namespace std;

//for n in `ipcs -q | grep ubuntu | awk '{print $2}'`; do ipcrm -q $n; done

extern void EXITONERROR (string msg);

class MQRequestChannel:public RequestChannel
{

private:

	int wmsqid;
	int rmsqid;

	struct msgbuf{
		long mtype;
		char mtext[250];
	};

	string queue_name(Mode _mode);
	int create_queue (string _queue_name);
	int get_projectid (string _queue_name);
	void open_read_queue(string _queue_name);
	void open_write_queue(string _queue_name);
	
	
public:

	/* -- CONSTRUCTOR/DESTRUCTOR */

	MQRequestChannel(const string _name, const Side _side);

	~MQRequestChannel();
	/* Destructor of the local copy of the bus. By default, the Server Side deletes any IPC 
	 mechanisms associated with the channel. */

	string cread();
	/* Blocking read of data from the channel. Returns a string of characters
	 read from the channel. Returns NULL if read failed. */

	int cwrite(string _msg);
	/* Write the data to the channel. The function returns the number of characters written
	 to the channel. */

	string name();
	/* Returns the name of the request channel. */

 
};


#endif


