/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
 */
#include "common.h"
#include "FIFOreqchannel.h"
#include <fstream>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;


int main(int argc, char *argv[]){
    int n = 100;    // default number of requests per "patient"
	int p = 15;		// number of patients
	char * fileName = NULL;
	char* bs = new char[sizeof(int)];
	bs=NULL;
    srand(time_t(NULL));
    double t = -1;
    int e = -1;
    p = -1;
    int bufferSize = MAX_MESSAGE;
    int c=0;
	while((c = getopt(argc, argv, "p:t:e:f:m:")) != -1){
  		switch(c){
  			case 'p':
  				p = atoi(optarg);
  				break;
  			case 't':
  				t = atof(optarg);
  				break;
  			case 'e':
  			  	e = atoi(optarg);
  			  	break;
  			case 'f':
  				fileName = optarg;
  				break;
  			case 'm':
  			  	bufferSize = atoi(optarg);
  			  	bs = optarg;
  			  	break;
  		}
  	}



	pid_t parent = getpid();
		pid_t pid = fork();

		if (pid == -1)
		{
			cout<<"here in error"<<endl;
		}
		else if (pid > 0)
		{
		    int status;
		    //waitpid(pid, &status, 0);
		}
		else
		{
			char *argsm[] = {"./server","-m", bs, NULL};
			char *args[]= {"./server",NULL};
			if(bs)
			{
				execv(argsm[0], argsm);
			}
			else
			{
				 execv(args[0],args);
			}

		    _exit(EXIT_FAILURE);
		}
    
		

    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
	struct timeval startTime, endTime;

	if(p>0 && t>0)

	{
		 // start timer.
		gettimeofday(&startTime, NULL);
		char buf [MAX_MESSAGE];
	    datamsg d(p, t, e);

	    chan.cwrite(&d, sizeof(d));
	    int nbytes = chan.cread(buf, MAX_MESSAGE);
	    double * result = (double *)buf;
	    cout<<*result<<endl;
	    gettimeofday(&endTime, NULL);

	    	    // Calculating total time taken by the program.
	    	    double time_taken;
	    	time_taken = (endTime.tv_sec - startTime.tv_sec) * 1e6;
	    	time_taken = (time_taken + (endTime.tv_usec -
	    	                              startTime.tv_usec)) * 1e-6;
	    	cout<<"time taken="<<time_taken<<endl;
	}

	//Writing the info of one patient to file x1.csv

	else if (p>0 && t==-1)
	{
		 // start timer.
		  gettimeofday(&startTime, NULL);
		  t=0;
		  char buf[MAX_MESSAGE];
		  char buf2[MAX_MESSAGE];

		  ofstream outdata("x"+to_string(p)+".csv");
		  if( !outdata ) {
			  cerr << "Error: file could not be opened" << endl;
			  exit(1);
		  }
		  for(int i=0; i<=14999;i++)
		  {
			  	e=1;
			  	datamsg d(p, t, e);
			  	chan.cwrite(&d, sizeof(d));
			  	int nbytes = chan.cread(buf, MAX_MESSAGE);
			  	double * e1 = (double *)buf;
			  	e=2;
			  	datamsg d2(p, t, e);
			  	chan.cwrite(&d2, sizeof(d2));
			  	nbytes = chan.cread(buf2, MAX_MESSAGE);
			  	double * e2 = (double *)buf2;
			  	outdata<<t<<","<<*e1<<","<<*e2<<endl;
			  	t=t+0.004;
		  }
		  gettimeofday(&endTime, NULL);
		  	    double time_taken;
		  	time_taken = (endTime.tv_sec - startTime.tv_sec) * 1e6;
		  	time_taken = (time_taken + (endTime.tv_usec -
		  	                              startTime.tv_usec)) * 1e-6;
		  	cout<<"time taken="<<time_taken<<endl;

	}

	// File transfer

	else if (fileName!=NULL)
	{

    gettimeofday(&startTime, NULL);
	int length = 0;
	__int64_t offset=0;
	char buf2[bufferSize];
	filemsg* f = new filemsg(offset, length);
	char *fileBuffer = new char[sizeof(filemsg)+strlen(fileName)+1];
	memcpy(fileBuffer, f, sizeof(filemsg));
	strcpy(fileBuffer+sizeof(filemsg), fileName);
	chan.cwrite(fileBuffer, sizeof(filemsg)+strlen(fileName)+1);
	int nbytes = chan.cread(&buf2, bufferSize);
	__int64_t * e2 = (__int64_t *)buf2;
	__int64_t fileSize = *e2;
	int arraySize = ceil((float)fileSize/(float)bufferSize);
	__int64_t start = 0;
	delete f;
	delete fileBuffer;
	cout<<"bufferSize="<<bufferSize;
	for(int i=0; i<arraySize; i++)
	{
		filemsg * f = new filemsg(start, bufferSize);
		char *fileBuffer = new char[sizeof(filemsg)+strlen(fileName)+1];
		memcpy(fileBuffer, f, sizeof(filemsg));
		strcpy(fileBuffer+sizeof(filemsg), fileName);
		chan.cwrite(fileBuffer, sizeof(filemsg)+strlen(fileName)+1);
		int nbytes = chan.cread(buf2, bufferSize);
		string recvdFileName =   fileName;
		recvdFileName = "received/" + recvdFileName;
		FILE* fp = fopen (recvdFileName.c_str(), "a");
		if(!fp) fp = fopen(recvdFileName.c_str(), "w");
		if (!fp){
				cerr << "Server received request for file: " << recvdFileName << " which cannot be opened" << endl;
				//return;
			}
		fwrite (buf2 , sizeof(char), bufferSize, fp);
		fclose (fp);
		start = start + bufferSize;
		if(start+bufferSize>fileSize)
			bufferSize = fileSize-start;
		delete f;
		delete fileBuffer;
	}
	gettimeofday(&endTime, NULL);

	    // Calculating total time taken by the program.
	    double time_taken;
	time_taken = (endTime.tv_sec - startTime.tv_sec) * 1e6;
	time_taken = (time_taken + (endTime.tv_usec -
	                              startTime.tv_usec)) * 1e-6;
	cout<<" time="<<time_taken<<endl;

	}

	
/*
	//New Channel 

	char newChannelName[30];
       	FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
	MESSAGE_TYPE nc = NEWCHANNEL_MSG;
	chan.cwrite(&nc, sizeof(MESSAGE_TYPE));
	chan.cread(newChannelName, 30);
	FIFORequestChannel newChan (newChannelName, FIFORequestChannel::CLIENT_SIDE);
	char buf [MAX_MESSAGE];
	p=10; t=59.004; e=2;
	    datamsg d(p, t, e);
	    newChan.cwrite(&d, sizeof(d));
	    int nbytes = newChan.cread(buf, MAX_MESSAGE);
	    double * result = (double *)buf;
	    cout<<*result<<endl;
	    MESSAGE_TYPE m1 = QUIT_MSG;
    	newChan.cwrite (&m1, sizeof (MESSAGE_TYPE));


	*/



    // closing the channel

    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite (&m, sizeof (MESSAGE_TYPE));
}
