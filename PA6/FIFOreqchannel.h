
 
#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include "reqchannel.h"
using namespace std;

void EXITONERROR (string msg);

class FIFORequestChannel:public RequestChannel
{

private:



	/*  The current implementation uses named pipes. */ 

	int wfd;
	int rfd;

	string pipe_name(Mode _mode);
	void create_pipe (string _pipe_name);
	void open_read_pipe(string _pipe_name);
	void open_write_pipe(string _pipe_name);
	
	
public:

	/* -- CONSTRUCTOR/DESTRUCTOR */

	FIFORequestChannel(const string _name, const Side _side);

	~FIFORequestChannel();
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

	int read_fd();
	/* Returns the file descriptor used to read from the channel. */

	int write_fd();
	/* Returns the file descriptor used to write to the channel. */
};




