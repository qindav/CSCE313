#include <iostream>
#include <string.h>
#include <iomanip>
using namespace std;

#include "Histogram.h"

Histogram::Histogram(){
	pthread_mutex_init (&mut1, NULL);
	pthread_mutex_init (&mut2, NULL);
	pthread_mutex_init (&mut3, NULL);
	pthread_mutex_init (&printer, NULL);
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
	pthread_mutex_destroy (&mut1);
	pthread_mutex_destroy (&mut2);
	pthread_mutex_destroy (&mut3);
	pthread_mutex_destroy (&printer);
}

void Histogram::update (string request, string response){
	int person_index = map [request];
	
	
	if(person_index==0){
		pthread_mutex_lock (&mut1);
		hist [0][stoi(response) / 10] ++;
		pthread_mutex_unlock (&mut1); 
	}
	if(person_index==1){
		pthread_mutex_lock (&mut2);
		hist [1][stoi(response) / 10] ++;
		pthread_mutex_unlock (&mut2); 
	}
	if(person_index==2)	{
		pthread_mutex_lock (&mut3);
		hist [2][stoi(response) / 10] ++;
		pthread_mutex_unlock (&mut3);
	}
	
}
void Histogram::print(){
	pthread_mutex_lock (&printer);
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
	pthread_mutex_unlock (&printer);
}