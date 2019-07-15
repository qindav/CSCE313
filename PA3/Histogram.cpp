#include <iostream>
#include <string.h>
#include <iomanip>
using namespace std;

#include "Histogram.h"

Histogram::Histogram(){
	pthread_mutex_init (&mut, NULL);
	for (int i=0; i<3; i++){
		memset (hist[i], 0, 10 * sizeof (int));	
	}
	map ["data John Smith"] = 0;
	map ["data Jane Smith"] = 1;
	map ["data Joe Smith"] = 2;

	names.push_back ("John Smith");
	names.push_back ("Jane Smith");
	names.push_back ("Joe Smith");
}

Histogram::~Histogram(){
	pthread_mutex_destroy (&mut);
}

void Histogram::update (string request, string response){
	pthread_mutex_lock (&mut);
	int person_index = map [request];
	hist [person_index][stoi(response) / 10] ++;
	pthread_mutex_unlock (&mut); 
}
void Histogram::print(){
	cout << setw(10) << right << "Range";
	for (int j=0; j<3; j++){
		cout << setw(15) << right << names [j];
	}
	cout <<endl;
	cout<<"----------------------------------------------------------" << endl;
	int sum [3];
	memset (sum, 0, 3 * sizeof (int));
	for (int i=0; i<10; i++){
		string range = "[" + to_string(i*10) + " - " + to_string((i+1)*10 - 1) + "]:"; 
		cout << setw (10) << right << range;
		for (int j=0; j<3; j++){
			cout << setw(15) << right << hist [j] [i];
			sum [j] += hist [j] [i];
		}
		cout << endl;
	}
	cout <<"----------------------------------------------------------" << endl;
	cout << setw(10) << right << "Totals:";
	for (int j=0; j<3; j++){
		cout << setw(15) << right << sum [j];
	}
	cout << endl;	
}