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
#include <sys/ipc.h>
#include <sys/shm.h>

#include "SHMreqchannel.h"
#include "reqchannel.h"

 
std::string SHMRequestChannel::shm_name(Mode _mode) {
	std::string pname = "shm_" + my_name;

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

SHMBB * SHMRequestChannel::create_shm (string _shm_name){
	int projectid = get_projectid(_shm_name);
	//cout<<side_name <<":queuename:"+_shm_name<<endl;
	//cout<<my_side;
	//cout<<side_name<<":project id:" << projectid<<endl;
	
	key_t key = ftok("shmfile", projectid);
	
	//cout<<side_name<<":key" << key<<endl;
	
	SHMBB *bb;
	
	if(my_side == SERVER_SIDE){
		bb = new SHMBB(projectid) ;
	}
	else{
		bb = new SHMBB(projectid) ;
	}
	
	cout<<side_name<<":shmid:"<<bb->get_shmid()<<endl;
	return bb;
}

int SHMRequestChannel::get_projectid (string _shm_name){
	if(_shm_name.compare(0,8,"shm_data")==0){
		//data queue
		return atoi(_shm_name.substr(8, _shm_name.size()-10).c_str())*10 + atoi(_shm_name.substr(_shm_name.size()-1, 1).c_str());
	}
	else if(_shm_name.compare(0,11,"shm_control")==0){
		//control channel
		return 9990 + atoi(_shm_name.substr(_shm_name.size()-1, 1).c_str());
	}
	else{
		EXITONERROR (_shm_name);
	}
}

void SHMRequestChannel::open_write_shm(string _shm_name) {
	
	//if (my_side == SERVER_SIDE)
	rbb = create_shm (_shm_name);

 
}

void SHMRequestChannel::open_read_shm(string _shm_name) {

	//if (my_side == SERVER_SIDE)
	wbb = create_shm(_shm_name);
 
	 
}

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

SHMRequestChannel::SHMRequestChannel(const std::string _name, const Side _side) 
{
	my_name = _name;
	my_side = _side;
	side_name = (_side == RequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";
	
	if (_side == SERVER_SIDE) {
		open_write_shm(shm_name(WRITE_MODE).c_str());
		open_read_shm(shm_name(READ_MODE).c_str());
	}
	else {
		open_read_shm(shm_name(READ_MODE).c_str());
		open_write_shm(shm_name(WRITE_MODE).c_str());
	}
}

SHMRequestChannel::~SHMRequestChannel() {
	delete wbb;
	delete rbb;
}

const int MAX_MESSAGE = 255;

string SHMRequestChannel::cread() {
    string s = rbb->pop();
	//cout<<side_name<<":recv:"<<s<<endl;
	return s;
}

int SHMRequestChannel::cwrite(string msg) {
	//cout<<side_name<<":write1:"<<msg<<endl;
	wbb->push(msg);
	//cout<<side_name<<":write2:"<<msg<<endl;
}

std::string SHMRequestChannel::name() {
	return my_name;
}





