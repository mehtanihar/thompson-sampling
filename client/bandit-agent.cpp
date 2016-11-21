#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <vector>
#include <random>
#include <math.h>
#include "gsl/gsl_rng.h"
#include "gsl/gsl_randist.h"
#include <algorithm>

#define MAXHOSTNAME 256

using namespace std;

int fact(int x){
	
	int prod=1;
	
	if(x!=0){
	for(int i=1;i<=x;i++){
		prod=prod*i;
		
	}
}
	return prod;
}

double pow(double a, int b){
	
	double prod=1;
	for(int i=1;i<=b;i++){
		prod=prod*a;
		
		
	}
	return prod;
}

double beta_value(int alpha, int beta, double x){
	
	
	return (fact(alpha+beta-1)*pow(x,alpha-1)*pow(1-x,beta-1))/(fact(alpha-1)*fact(beta-1));
	
	
}

int arm(int S_thompson[], int F_thompson[], int numArms){
	
	
	//srand(time(0));
    int seed = rand() % 1000000; 
	double max=0; int arm_max=0; double theta=0;
	 const gsl_rng_type * T;
    gsl_rng * rd;
    gsl_rng_env_setup();
    T = gsl_rng_default;
    rd = gsl_rng_alloc (T);
    gsl_rng_set(rd, seed+1);
	
	for(int i=0;i<numArms;i++){
		
		
		theta=gsl_ran_beta(rd,S_thompson[i]+1,F_thompson[i]+1);
		
		
		
		//theta=beta_value(S_thompson[i]+1,F_thompson[i]+1,x);
		
		if(theta>max){max=theta; arm_max=i;}
		
	}
	 gsl_rng_free(rd);
	
	cout<<"max_theta:"<<max<<endl;
	return arm_max;
	
}

void options(){

  cout << "Usage:\n";
  cout << "bandit-agent\n"; 
  cout << "\t[--numArms numArms]\n";
  cout << "\t[--randomSeed randomSeed]\n";
  cout << "\t[--horizon horizon]\n";
  cout << "\t[--explorationHorizon explorationHorizon]\n";
  cout << "\t[--hostname hostname]\n";
  cout << "\t[--port port]\n";
}


/*
  Read command line arguments, and set the ones that are passed (the others remain default.)
*/
bool setRunParameters(int argc, char *argv[], int &numArms, int &randomSeed, unsigned long int &horizon,unsigned long int &explorationHorizon, string &hostname, int &port){

  int ctr = 1;
  while(ctr < argc){

    //cout << string(argv[ctr]) << "\n";

    if(string(argv[ctr]) == "--help"){
      return false;//This should print options and exit.
    }
    else if(string(argv[ctr]) == "--numArms"){
      if(ctr == (argc - 1)){
	return false;
      }
      numArms = atoi(string(argv[ctr + 1]).c_str());
      ctr++;
    }
    else if(string(argv[ctr]) == "--randomSeed"){
      if(ctr == (argc - 1)){
	return false;
      }
      randomSeed = atoi(string(argv[ctr + 1]).c_str());
      ctr++;
    }
    else if(string(argv[ctr]) == "--horizon"){
      if(ctr == (argc - 1)){
	return false;
      }
      horizon = atoi(string(argv[ctr + 1]).c_str());
      ctr++;
    }
    else if(string(argv[ctr]) == "--explorationHorizon"){
      if(ctr == (argc - 1)){
	return false;
      }
      explorationHorizon = atoi(string(argv[ctr + 1]).c_str());
      ctr++;
    }
    else if(string(argv[ctr]) == "--hostname"){
      if(ctr == (argc - 1)){
	return false;
      }
      hostname = string(argv[ctr + 1]);
      ctr++;
    }
    else if(string(argv[ctr]) == "--port"){
      if(ctr == (argc - 1)){
	return false;
      }
      port = atoi(string(argv[ctr + 1]).c_str());
      ctr++;
    }
    else{
      return false;
    }

    ctr++;
  }

  return true;
}



int main(int argc, char *argv[]){
	
	//srand (time(NULL));
	
  // Run Parameter defaults.
  int numArms;
  int randomSeed;
  unsigned long int horizon;
  unsigned long int explorationHorizon;
  string hostname;
  int port;

 
  
  

  //Set from command line, if any.
  if(!(setRunParameters(argc, argv, numArms, randomSeed, horizon, explorationHorizon, hostname, port))){
    //Error parsing command line.
    options();
    return 1;
  }
  

  struct sockaddr_in remoteSocketInfo;
  struct hostent *hPtr;
  int socketHandle;

  bzero(&remoteSocketInfo, sizeof(sockaddr_in));
  
  if((hPtr = gethostbyname((char*)(hostname.c_str()))) == NULL){
    cerr << "System DNS name resolution not configured properly." << "\n";
    cerr << "Error number: " << ECONNREFUSED << "\n";
    exit(EXIT_FAILURE);
  }

  if((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    close(socketHandle);
    exit(EXIT_FAILURE);
  }

  memcpy((char *)&remoteSocketInfo.sin_addr, hPtr->h_addr, hPtr->h_length);
  remoteSocketInfo.sin_family = AF_INET;
  remoteSocketInfo.sin_port = htons((u_short)port);

  if(connect(socketHandle, (struct sockaddr *)&remoteSocketInfo, sizeof(sockaddr_in)) < 0){
    //code added
    cout<<"connection problem"<<".\n";
    close(socketHandle);
    exit(EXIT_FAILURE);
  }


  char sendBuf[256];
  char recvBuf[256];
cout<<"Funny"<<numArms;
 int S_thompson[numArms];
  int F_thompson[numArms];
  
  
  for(int i=0;i<numArms;i++){
	  
	  S_thompson[i]=0;
	  F_thompson[i]=0;
	  
  }


  int armToPull = 0;
  sprintf(sendBuf, "%d", armToPull);
  
	
  cout << "Sending action " << armToPull << ".\n";
  while(send(socketHandle, sendBuf, strlen(sendBuf)+1, MSG_NOSIGNAL) >= 0){

    float reward = 0;
    unsigned long int pulls=0;
    char temp;
    recv(socketHandle, recvBuf, 256, 0);
    sscanf(recvBuf, "%f %c %lu", &reward, &temp, &pulls);
    cout << "Received reward " << reward << ".\n";
    cout<<"Num of  pulls "<<pulls<<".\n";
    cout<<"calling function"<<endl;

    //armToPull = pulls % numArms;
    
    if(reward==1){S_thompson[armToPull]++;}
    if(reward==0){F_thompson[armToPull]++;}
    
	
	
	
	armToPull=arm(S_thompson,F_thompson,numArms);
		
	
    sprintf(sendBuf, "%d", armToPull);
    cout << "Sending action " << armToPull << ".\n";
  }
  
  close(socketHandle);

  cout << "Terminating.\n";

  return 0;
}
          
