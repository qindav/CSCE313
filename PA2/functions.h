#include <sstream>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cerrno>
#include <vector>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <iterator>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <bits/stdc++.h> 

using namespace std;

//Read args and determines how to run shell
int read_args(char **argv);
string mytrim(const string& str);
bool isSimple(int argc, char **argv);

//Debugs
void output(vector<string> v);

//Stores args into an array, prepares for useage in running
char** CopyToArray();
vector<string> read_command();

//Used to view the results of the function ran
void run_commands(vector<string> commandsList);
void run_command_without_pipe(string cmd);
void run_cmd(int argc, char** argv);

//Used for modifying the "shell>" string
string return_time();
string getprompt( bool wantInfo, bool wantEmpty);