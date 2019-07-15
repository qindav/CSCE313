
 
#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include "reqchannel.h"
#include "SHMBB.h"
using namespace std;

extern void EXITONERROR (string msg);

class SHMRequestChannel:public RequestChannel
{

private:



	/*  The current implementation uses named pipes. */ 

	SHMBB* rbb;
	SHMBB* wbb;

	string shm_name(Mode _mode);
	SHMBB * create_shm (string _shm_name);
	int get_projectid (string _shm_name);
	void open_read_shm(string _shm_name);
	void open_write_shm(string _shm_name);
	
	
public:

	/* -- CONSTRUCTOR/DESTRUCTOR */

	SHMRequestChannel(const string _name, const Side _side);

	~SHMRequestChannel();
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




