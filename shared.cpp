#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

using namespace std;

#define MAX_COUNT 1000

int main() {
    // Deschide sau creaza un obiect mapat in memoria partajata
    int fd = shm_open("/my_shared_memobry", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(int));

    // Mapeaza obiectul in spatiul de adrese
    int* shared_memory = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Initializare semafor
    sem_t* semaphore = sem_open("/my_semaphore", O_CREAT, 0666, 1);

    srand(time(NULL));

    for(int i = 1; i < MAX_COUNT; ++i) {
        // Asteapta sa obtina acces la memoria partajata
        sem_wait(semaphore);

        // Da cu banul pentru a decide daca sa scrie in memorie
        if(rand() % 2 == 0) {
            *shared_memory = i;
            cout << "Procesul a scris in memorie: " << i << endl;

            // Elibereaza semaforul
            sem_post(semaphore);
        } else {
            cout << "Procesul nu a scris in memorie." << endl;
            sem_post(semaphore);
        }

        // Pauza mica intre iteratii
        usleep(1000);
    }

    // Elibereaza resursele
    munmap(shared_memory, sizeof(int));
    close(fd);
    sem_close(semaphore);

    cout << "Procesul sa terminat." << endl;

    return 0;
}

