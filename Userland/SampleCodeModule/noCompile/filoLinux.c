#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>

#define MAXPHILOS 10
#define MINPHILOS 3

typedef enum {
    THINKING=0,
    HUNGRY,
    EATING
} state_t;

typedef struct Philosopher {
    pid_t pid;
    int rightForkId;
    int leftForkId;
    int haveToLeave;
    state_t state;
}philo_t;

typedef struct shared_data {
    philo_t philosophers[MAXPHILOS];
    sem_t forks[MAXPHILOS];
    int cantPhilos;
    sem_t mutex;
} shared_data_t;

shared_data_t *shared_data;

int cantPhilos;

void create_shared_memory() {
    int shm_fd = shm_open("/shared_data_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    
    if (ftruncate(shm_fd, sizeof(shared_data_t)) == -1) {
        perror("ftruncate");
        exit(1);
    }
    
    shared_data = mmap(NULL, sizeof(shared_data_t), 
                      PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    
    close(shm_fd);
    
    shared_data->cantPhilos = cantPhilos;
}

void open_shared_memory() {
    int shm_fd = shm_open("/shared_data_shm", O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    
    shared_data = mmap(NULL, sizeof(shared_data_t), 
                      PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    
    close(shm_fd);
}

void cleanup_shared_memory() {
    munmap(shared_data, sizeof(shared_data_t));
    shm_unlink("/shared_data_shm");
}

void philo_init(int id, pid_t pid) {
    open_shared_memory();
    sem_wait(&shared_data->mutex);
    shared_data->philosophers[id].pid = pid;
    shared_data->philosophers[id].state = THINKING;
    shared_data->philosophers[id].rightForkId = -1;
    shared_data->philosophers[id].leftForkId = -1;
    shared_data->philosophers[id].haveToLeave = 0;
    sem_post(&shared_data->mutex);
}


void init_sems() {
    for (int i = 0; i < MAXPHILOS; i++) {
        sem_init(&shared_data->forks[i], 1, 1);
    }
    sem_init(&shared_data->mutex, 1, 1);
}

void destroy_sems() {
    for (int i = 0; i < MAXPHILOS; i++) {
        sem_destroy(&shared_data->forks[i]);
    }
}

void take_forks(int id) {
    
    if(id%2){
        sem_wait(&shared_data->mutex);
        shared_data->philosophers[id].leftForkId =id;
        sem_post(&shared_data->mutex);
        
        sem_wait(&shared_data->forks[id]);

        sem_wait(&shared_data->mutex);
        shared_data->philosophers[id].rightForkId = (id + 1) % (shared_data->cantPhilos);
        sem_post(&shared_data->mutex);
        sem_wait(&shared_data->forks[shared_data->philosophers[id].rightForkId]);
        
    }else{
        sem_wait(&shared_data->mutex);
        shared_data->philosophers[id].rightForkId = (id + 1) % (shared_data->cantPhilos);
        sem_post(&shared_data->mutex);
        sem_wait(&shared_data->forks[shared_data->philosophers[id].rightForkId]);
        
        sem_wait(&shared_data->mutex);
        shared_data->philosophers[id].leftForkId =id;
        sem_post(&shared_data->mutex);
        sem_wait(&shared_data->forks[id]);
    }
    return;
}

void put_forks(int id) {
    sem_post(&shared_data->forks[shared_data->philosophers[id].leftForkId]);
    sem_post(&shared_data->forks[shared_data->philosophers[id].rightForkId]);
}


void thinking() {
    usleep(100);
}

void eating(){
    usleep(100);
}

void philosophing(int id) {
    philo_init(id, getpid());
    int haveToLeave = 0;
    while (!haveToLeave) {
        thinking();
        sem_wait(&shared_data->mutex);
        shared_data->philosophers[id].state = HUNGRY;
        sem_post(&shared_data->mutex);
        take_forks(id);
        sem_wait(&shared_data->mutex);
        shared_data->philosophers[id].state = EATING;
        sem_post(&shared_data->mutex);
        eating();
        put_forks(id);
        sem_wait(&shared_data->mutex);
        shared_data->philosophers[id].state = THINKING;
        if(shared_data->philosophers[id].haveToLeave) {
            haveToLeave = 1;
        }
        sem_post(&shared_data->mutex);
    }
    return;
}

void printPhilos() {
    sem_wait(&shared_data->mutex);
    for (int i = 0; i < shared_data->cantPhilos; i++) {
        printf("%s",shared_data->philosophers[i].state == EATING? "E ": ". ");    
    }
    putchar('\n');
    sem_post(&shared_data->mutex);

}

void addPhilo() {
    if(shared_data->cantPhilos < MAXPHILOS) {
        sem_wait(&shared_data->mutex);
        int aux = (shared_data->cantPhilos)++;
        sem_post(&shared_data->mutex);
        if(fork() == 0) {
            philosophing(aux);
            return;
        }
        printf("Added philosopher %d\n", aux );
    } else {
        printf("Cannot add more philosophers, maximum reached.\n");
    }
}

void removePhilo(){
    if(shared_data->cantPhilos > MINPHILOS) {
        sem_wait(&shared_data->mutex);
        shared_data->philosophers[shared_data->cantPhilos - 1].haveToLeave = 1;
        shared_data->cantPhilos--;
        sem_post(&shared_data->mutex);
        waitpid(shared_data->philosophers[shared_data->cantPhilos].pid, NULL, 0);
        printf("Removed philosopher %d\n", shared_data->cantPhilos);
    } else {
        printf("Cannot remove philosopher, minimum three must remain.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_philosophers>\n", argv[0]);
        return 1;
    }

    cantPhilos = atoi(argv[1]);
    if (cantPhilos < MINPHILOS || cantPhilos > MAXPHILOS) {
        fprintf(stderr, "Number of philosophers must be between 1 and %d\n", MAXPHILOS);
        return 1;
    }
    create_shared_memory();
    init_sems();
    

    for (int i = 0; i < cantPhilos; i++) {
        if(fork() == 0) {
ç            philosophing(i);
            return 0;
        }
    }
    int aux=0;
    int exitFlag = 0;
    while(!exitFlag ) {
        aux++;
        usleep(100000);
        printPhilos();
        if(aux % 15 == 0){
            printf("Press 'A' to add a philosopher, 'q' to remove the last philosopher, 'S' to stop the simulation: ");
            char c;
            while(((c=getchar())== 'A' || c == 'a') || (c == 'q' || c == 'Q') || (c =='s' || c == 'S') || (c == 'c' || c == 'C')) {
                if(c== 'q' || c == 'Q') {
                    removePhilo();
                } else if(c == 's' || c == 'S') {
                    exitFlag = 1;
                    break;
                } else if(c == 'A' || c == 'a') {
                    addPhilo();
                }
            }
        }
    }
    sem_wait(&shared_data->mutex);
    for(int i = 0; i < shared_data->cantPhilos; i++) {
        shared_data->philosophers[i].haveToLeave = 1;
    }
    sem_post(&shared_data->mutex);
    
    for(int i = 0; i < shared_data->cantPhilos; i++) {
        waitpid(shared_data->philosophers[i].pid, NULL, 0);
    }
    destroy_sems();
    cleanup_shared_memory();
    return 0;
}