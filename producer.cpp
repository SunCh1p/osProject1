#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>


int main() {
    srand(time(0));

    //create buffer that can hold two integers
    const int SIZE = 2 * sizeof(int);

    //name of shared memory object which in our case is "table"
    const char* name = "table";

    //shared memory file descriptor
    int shm_fd;
    //pointer to shared memory object
    void* ptr;

    //Open shared memory object
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error creating shared memory object!" << std::endl;
        return 1;
    }

    //configure size of shared memory object
    if (ftruncate(shm_fd, SIZE) == -1) {
      std::cerr << "Error resizing shared memory object!" << std::endl;
      return 1;
    }

    //create/open semaphores
    const char* sem_full = "/sem_full";
    const char* sem_empty = "/sem_empty";
    const char* sem_mutex = "/sem_mutex";
    //initialize full semaphore to 0
    sem_t* full = sem_open(sem_full, O_CREAT, 0666, 0);
    //itialize empty semaphore to 2
    sem_t* empty = sem_open(sem_empty, O_CREAT, 0666, 1);
    //initialize mutex semaphore to 1 to protect critical section
    sem_t* mutex = sem_open(sem_mutex, O_CREAT, 0666, 1);
    if(full == SEM_FAILED || empty == SEM_FAILED || mutex == SEM_FAILED) {
        std::cerr << "Error creating semaphores!" << std::endl;
        return 1;
    }

    // Memory map the shared memory object
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "Error mapping shared memory!" << std::endl;
        return 1;
    }

    int* int_ptr = static_cast<int*>(ptr);
    int count = 0;
    //produce items 10 times just so the producer doesn't go on forever
    for(int i = 0; i < 10; i++){
      //wait for empty
      sem_wait(empty);
      //wait for mutex so you can enter critical section
      sem_wait(mutex);
      
      //time delay so it's easy to see output
      int timeAsleep = rand() % 3 + 1;
      sleep(timeAsleep);
      //produce 2 items
      int_ptr[0] = count;
      int_ptr[1] = count + 1;
      count += 2;
      std::cout << "Produced items: " << int_ptr[0] << " and " << int_ptr[1]<< std::endl;
      //signal 2 items are produced
      sem_post(full);

      //leave critical section
      sem_post(mutex);
    }

    //close shared memory
    close(shm_fd);
    //unlink sempaores
    sem_close(full);
    sem_close(empty);
    sem_close(mutex);

    //Unlink semaphores (remove them from the system)
    sem_unlink(sem_full);
    sem_unlink(sem_empty);
    sem_unlink(sem_mutex);

    //Unlink the shared memory object
    shm_unlink(name);
    std::cout << "Producer finished producing items." << std::endl;
    return 0;
}