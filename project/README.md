Compile and run:
A Makefile has been provided for both the versions of the project. 
For the compilation of the pure C++ threads and mechanisms version C++ 11 is required:

g++ -Wall -std=c++11 -pthread -O3 *.cpp -o main

For the compilation of the FastFlow version C++ 17 has been used and it is necessary to include the path to the FastFlow directory 
(usually $HOME/fastflow):

g++ -Wall -std=c++17 -I $HOME/fastflow -pthread -O3 *.cpp -o main

In order to run the test file (the test is the same for the two versions) it is necessary to insert some parameters 
(the usage is shown by invoking ./main without parameters).
Mandatory parameters are: 
•	vector size 
•	scan type (i means inclusive, e means exclusive)
•	parallelism degree (0 for sequential execution)
Optional parameters are:
•	seed for the generation of the input vector (otherwise a random seed is used)
•	test number (default is test number one: vector of integers and addition operation)
