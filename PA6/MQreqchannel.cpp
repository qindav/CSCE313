/* 
    File: requestchannel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/msg.h>

#include "MQreqchannel.h"
#include "reqchannel.h"

 
std::string MQRequestChannel::queue_name(Mode _mode) {
	std::string pname =  my_name;

	if (my_side == CLIENT_SIDE) {
		if (_mode == READ_MODE)
			pname += "1";
		else
			pname += "2";
	}
	else {
	/* SERVER_SIDE */
		if (_mode == READ_MODE)
			pname += "2";
		else
			pname += "1";
	}
	return pname;
}

int MQRequestChannel::create_queue (string _queue_name){
	int projectid = get_projectid(_queue_name);
	cout<<side_name <<":queuename:"+_queue_name<<endl;
	//cout<<my_side;
	cout<<side_name<<":project id:" << projectid<<endl;
	
	key_t key = ftok("queuefile", projectid);
	
	cout<<side_name<<":key" << key<<endl;
	
	int msqid;
	
	msqid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL); 
	if(msqid < 0){
		msqid = msgget(key, 0666); 
	}
	cout<<side_name<<":queueid:"<<msqid<<endl;
	return msqid;
}

int MQRequestChannel::get_projectid (string _queue_name){
	if(_queue_name.compare(0,4,"data")==0){
		//data queue
		return atoi(_queue_name.substr(4, _queue_name.size()-6).c_str())*10 + atoi(_queue_name.substr(_queue_name.size()-1, 1).c_str());
	}
	else if(_queue_name.compare(0,7,"control")==0){
		//control channel
		return 9990 + atoi(_queue_name.substr(_queue_name.size()-1, 1).c_str());
	}
	else{
		EXITONERROR (_queue_name);
	}
}

void MQRequestChannel::open_write_queue(string _queue_name) {
	
	//if (my_side == SERVER_SIDE)
	wmsqid = create_queue (_queue_name);

	if (wmsqid < 0) {
		EXITONERROR (_queue_name);
	}
}

void MQRequestChannel::open_read_queue(string _queue_name) {

	//if (my_side == SERVER_SIDE)
	rmsqid = create_queue(_queue_name);
 
	if (rmsqid < 0) {
		perror ("");
		exit (0);
	}
}


/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

MQRequestChannel::MQRequestChannel(const std::string _name, const Side _side) 
{
	my_name = _name;
	my_side = _side;
	side_name = (_side == RequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";
	
	if (_side == SERVER_SIDE) {
		open_write_queue(queue_name(WRITE_MODE).c_str());
		open_read_queue(queue_name(READ_MODE).c_str());
	}
	else {
		open_read_queue(queue_name(READ_MODE).c_str());
		open_write_queue(queue_name(WRITE_MODE).c_str());
	}
}

MQRequestChannel::~MQRequestChannel() {
	msgctl(wmsqid, IPC_RMID, NULL);
	msgctl(rmsqid, IPC_RMID, NULL); 
}

const int MAX_MESSAGE = 255;

string MQRequestChannel::cread() {
	struct msgbuf buf;
	msgrcv(rmsqid, &buf, sizeof(buf.mtext), 1, 0 );
	//string s = string(buf.mtext);
	string s=buf.mtext;
	//cout<<side_name<<":rev:"<<s<<endl;
	return s;

}

int MQRequestChannel::cwrite(string msg) {

	if (msg.size() > MAX_MESSAGE) {
		EXITONERROR ("cwrite");
	}
	//cout<<side_name<<":send:"<<msg<<endl;
	int len=msg.size();
	struct msgbuf buf;
	buf.mtype = 1;
	strcpy(buf.mtext, msg.c_str());
	buf.mtext[len]='\0';
	msgsnd(wmsqid, &buf, len+1, 0);
}

std::string MQRequestChannel::name() {
	return my_name;
}




