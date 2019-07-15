#include "functions.h"

extern string sprompt;

vector<string> split(const std::string& str, char delim = ' '){
    vector<string> tokens;
    string arg, temp;
	
	//Begin string stream, parse by space
    stringstream ss(str);
    while(getline(ss, temp, delim)){
		temp=mytrim(temp);
		if(temp!=""){
			//Special cases for awk or jobs
			if(temp=="awk"){
			   tokens.push_back(temp);
			   temp = mytrim(str.substr(4));
			   cout<<"~"+temp+"~"<<endl;
			   temp = temp.substr(1, temp.length()-2);
			   tokens.push_back(temp);
			   break;
			 }
			 else if (temp=="jobs"){
			   long pid = (long)getpid(); 
			   string spid = "ps -" + to_string(pid);          
			   tokens.push_back(spid);           
			 }
			 else{
			   tokens.push_back(temp);
			 }
       }
       
    }
    return tokens;
}

string mytrim(const string& str){
	//Trims string a little bit
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
        return str;
		
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

char** CopyToArray(vector<string> vstr){
	//Converts string to char array
	char** s = new char*[vstr.size()+1];
		for (int i=0; i<vstr.size(); i++)
			*(s + i) = &((vstr[i])[0]);
	
	
	int last;
	last = vstr.size();
	*(s+last) = '\0'; 
	return s; 
}

void run_command_without_pipe(string cmd){
	int argc;
	char **argv;	
	vector<string> argv_vector;
   
	//Split arguments by spaces, put into vector
	argv_vector = split(cmd, ' ');
	argv=CopyToArray(argv_vector);
	argc = argv_vector.size();
	
	//Run it w/o pipes
	run_cmd(argc, argv);
  
}

void output(vector<string> v){
	//Just used to print out contents
    for(int i = 0; i < v.size(); i++)
        cout << v[i] << endl;
}

void run_cmd(int argc, char** argv) {
	pid_t pid;
	const char *amp;
	amp = "&";
	bool found_amp = false;

	//Set flag if theres a &
	if (strcmp(argv[argc - 1], amp) == 0)
		found_amp = true;

	//Start fork
	pid = fork();

	if (pid == 0) {			// child process
		//Remove the amp here
		if (found_amp) {
			argv[argc - 1] = NULL;
			argc--;
		}

		//Run the command using execvp
		execvp(argv[0], argv);
		perror("execvp error");
	}
	else if (!found_amp)	// parent process
		waitpid(pid, NULL, 0); //Only wait if no ampersand
}

vector<string> read_command() {
	
	//Read the input
	vector<string> commandsList;
	string arg;

	//Read in whole line, break by | (pipe)
	//Then store it in vector
	getline(cin, arg);
  
	commandsList = split(arg, '|');
  
	//Let the user exit out if their input suggests they want to.
	if (arg == "quit" || arg == "exit") {
	cout << "Shell Ended\n";
	exit(0);
	}
   
	//Return the vector
	return commandsList;
}

void run_commands(vector<string> commandsList){
  int fd [2];
  pid_t pid;
  int in = 0;
 
  char* fileout=nullptr;
  char* filein=nullptr;
  string temp;
   
  for(int i = 0; i < commandsList.size(); i++){
    string cmdInput = commandsList[i];
    
    
    //needs check directive for ">"
    std::size_t found = cmdInput.find(">"); 
    if (found!=std::string::npos){
        //found ">"
        temp = cmdInput.substr(found+1);
        fileout = new char(temp.length()+1);
        strcpy(fileout, temp.c_str());
        cmdInput = cmdInput.substr(0, found); 
    }
    
	//needs check directive for "<"
    found = cmdInput.find("<");
    if (found!=std::string::npos){
        //found "<"
        temp = cmdInput.substr(found+1);
        filein = new char(temp.length()+1);
        strcpy(filein, temp.c_str());
        
        cmdInput = cmdInput.substr(0, found);           
    }
         
      
    //convert to command char*
    vector<string> argv_vector;
    argv_vector = split(cmdInput, ' ');
	
    char **argv = CopyToArray(argv_vector);
    int argc = argv_vector.size();
    
	//Special case for cd
    found = cmdInput.find("cd");   
    if(found==0){
        chdir(argv[1]);
        break;
     }
    
	
	//Used for multipiping
    int fdo;
    int fdi;
	
	//Begin piping
    pipe (fd);
   	pid = fork();
     
    if(pid==-1){		//error
		cout << "Fork error" <<endl;
		exit(1);
    }
    if (pid == 0){
    
      if(filein!=nullptr){
        fdi = open(filein, O_RDONLY, 0);
        dup2 (fdi, 0); // overwriting stdin with the new file
      
      }
      else{
        dup2(in, 0);
      }
      
      
      if(i!=commandsList.size()-1){
        dup2 (fd[1], 1); // overwriting stdout to the pipe’s WRITE end
       
      }
      
      if(fileout!=nullptr){
        
        fdo= open (fileout, O_CREAT|O_WRONLY, S_IRUSR | S_IWUSR);
       
        dup2 (fdo, 1); // overwriting stdout with the new file
      }
      
      close (fd[0]); // close unused pipe end
      execvp(argv[0], argv);
      
      if(fileout!=nullptr){
        close(fdo);
        dup2(1,1);
      }
      
      if(filein!=nullptr){
        close(fdi);
        dup2(0,0);
      }
    }
    else{	//Parent
		wait (NULL);
		close (fd[1]); // close unused pipe end
		in = fd[0];	//used for the other portions
   wait (NULL);
    }
    
    
  }
    
}


string getprompt( bool wantInfo, bool wantEmpty){
	//get username
	char *buf;
	buf =(char *)malloc(80*sizeof(char));
	buf = getlogin();
	string name(buf);
  
	//Get current directory
	char buf1[80];
	getcwd(buf, 80);
	string curdir(buf);
  
  
	//get updated time
	string timeinfo = return_time();
  
	string default_prompt = "Shell>";
	//Full string ready
	if(wantEmpty)
		default_prompt = "";
	if(wantInfo)
		default_prompt =  "[" + name + "]" + "[" + curdir + "]" + timeinfo + "Shell>";
  
	//Modified shell tag done
	return default_prompt;
}

string return_time(){
  //Source https://stackoverflow.com/questions/16357999/current-date-and-time-as-string
  time_t rawtime;

  struct tm * timeinfo;
  char buffer[100];

  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,sizeof(buffer),"[%d-%m-%Y %H:%M:%S] ",timeinfo);
  string str(buffer);
  return str;
}
