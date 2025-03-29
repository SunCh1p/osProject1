# OS PROJECT 1
## Description
Producer and Consumer problem sample implementation for Kent State university Operating systems class
## Usage
Two ways to compile and run
1. Makefile: type 'make build' and then 'make run' in directory of project folder. 
This assumes you have make installed.
2. Manual way:
    1. type 'g++ consumer.cpp -o consumer -lrt -pthread'
    2. type 'g++ producer.cpp -o producer -lrt -pthread'
    Congratulations, compilation is done. To run the program, type:
    './producer & ./consumer&'

