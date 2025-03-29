.PHONY: build clean run check kill

build:
	g++ consumer.cpp -o consumer -lrt -pthread
	g++ producer.cpp -o producer -lrt -pthread

clean:
	rm producer
	rm consumer

run: 
	./producer &
	./consumer &

check:
	ps aux | grep consumer
	ps aux | grep producer

kill: 
	pkill -f producer
	pkill -f consumer