#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>

int main() {
    srand(time(0));

    // The size (in bytes) of shared memory object
    const int SIZE = 2 * sizeof(int);

    // Name of the shared memory object
    const char* name = "table";

    // Shared memory file descriptor
    int shm_fd;

    // Pointer to shared memory object
    void* ptr;

    // Open the shared memory object
    shm_fd = shm_open(name, O_RDONLY, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error opening shared memory object!" << std::endl;
        return 1;
    }

    // Memory map the shared memory object
    ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "Error mapping shared memory!" << std::endl;
        return 1;
    }

    int* int_ptr = static_cast<int*>(ptr);

    //create/open semaphores
    const char* sem_full = "/sem_full";
    const char* sem_empty = "/sem_empty";
    const char* sem_mutex = "/sem_mutex";
    //initialize full semaphore to 0
    sem_t* full = sem_open(sem_full, 0);
    //get empty semaphore
    sem_t* empty = sem_open(sem_empty, 0);
    //initialize mutex semaphore to 1 to protect critical section
    sem_t* mutex = sem_open(sem_mutex, 0);
    if(full == SEM_FAILED || empty == SEM_FAILED || mutex == SEM_FAILED) {
        std::cerr << "Error creating semaphores!" << std::endl;
        return 1;
    }

    //Consume items 20 times
    for(int i = 0; i < 10; i++){
      //wait for items to be produced
      sem_wait(full);
      //wait for mutex so can enter critical section
      sem_wait(mutex);

      //consume item with time delay so it's easy to see the output
      int timeAsleep = rand() % 3 + 1;
      sleep(timeAsleep);
      std::cout << "Consumed item: " << int_ptr[0] << " and " << int_ptr[1]<< std::endl;
      //leave critical section
      sem_post(mutex);
      //signal that item has been consumed
      sem_post(empty);
      
    }

    //close shared memory
    close(shm_fd);
    //unlink sempaores
    sem_close(full);
    sem_close(empty);
    sem_close(mutex);

    //unlink semaphores
    sem_unlink(sem_full);
    sem_unlink(sem_empty);
    sem_unlink(sem_mutex);

    //unlink the shared memory object
    shm_unlink(name);

    std::cout << "Consumer finished consuming items" << std::endl;
    return 0;
}