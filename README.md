# thompson-sampling
Implemented Thompson Sampling algorithm for multi-armed bandit-agent interactions

The server is implemented as bandit.cpp in the server directory.

The client is implemented as bandit-agent.cpp in the client directory

Run the command g++ bandit-agent.cpp -lgsl -lgslcblas -lm -std=gnu++11 to make the bandit file. I have already provided the makefile.

Run the command start_experiment.sh to run the experiment. 

You can see the output in the serverlog.txt file

This was my assignment 1 of the CS 747 course at IIT Bombay.
