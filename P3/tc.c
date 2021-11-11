#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>


//sem_t semaphore;

sem_t lock_headOfLine[4];
sem_t lock_exit[4];


//left turns 
//sem_t NW;
//sem_t EN;
//sem_t SE;
//sem_t WS;
sem_t lock_left[4];

//right turns
//sem_t NE;
//sem_t ES;
//sem_t SW;
//sem_t WS
sem_t lock_right[4];

//straight 
//sem_t NN;
//sem_t EE;
//sem_t SS;
//sem_t WW;
sem_t lock_straight[4];



sem_t lock_collision;

int vehicle_arrival[4] = {0,0,0,0};
double t0, t;

typedef struct _directions { 
    char dir_original; 
    char dir_target; } directions;

typedef struct _car {
    int cid;
    double arival_time;
    directions dir;
} car;


double GetTime()
{
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec / 1e6;
}

void Spin(int howlong)
{
    double t = GetTime();
    while ((GetTime() - t) < (double)howlong)
        ; // do nothing in loop
}

void Debugging(int id, char dirO, char dirT, char* str)
{
    printf("Time %4.1f: Car %2d (%c-> %c->) %s\n", GetTime() - t0, id, dirO, dirT, str);
}

void Car(void* arg) { 
    car* vehicle = (car*) arg;
    ArriveIntersection(vehicle);
    CrossIntersection(vehicle);
    ExitIntersection(vehicle);
}

void threadfunc(directions dir) {
    while (1) {
        sem_wait(&semaphore);
        printf("Hello from da thread!\n");
        sem_post(&semaphore);
        sleep(1);
    }
}

void ArriveIntersection(car *dir)
{
    Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "arriving");
    if (dir->dir.dir_original == 'N')
    {
        sem_wait(&lock_headOfLine[0]);
        vehicle_arrival[0]++;
        if (dir->dir.dir_target == 'N')
        {
            
            sem_wait(&lock_straight[1]);
            sem_wait(&lock_straight[3]);
            sem_wait(&lock_left[2]);
            sem_wait(&lock_left[1]);
           
        }
        else if (dir->dir.dir_target == 'E')
        {
            sem_wait(&lock_right[0]);
            sem_wait(&lock_straight[1]);
        }
        else if (dir->dir.dir_target == 'W')
        {
            sem_wait(&lock_left[0]);
            sem_wait(&lock_straight[1]);
            sem_wait(&lock_straight[3]);
            sem_wait(&lock_left[2]);
            
        }
        //sem_post(&lock_headOfLine[0]);
        sleep(1);
    }
    else if (dir->dir.dir_original == 'E')
    {
        sem_wait(&lock_headOfLine[1]);
        vehicle_arrival[1]++;
        if (dir->dir.dir_target == 'E')
        {
            if (vehicle_arrival[1] == 1)
            {
                sem_wait(&lock_collision);
            }
            sem_wait(&lock_straight[1]);
        }
        else if (dir->dir.dir_target == 'N')
        {
            if (vehicle_arrival[1] == 1)
            {
                sem_wait(&lock_collision);
            }
            sem_wait(&lock_right[1]);
        }
        else if (dir->dir.dir_target == 'S')
        {
            if (vehicle_arrival[1] == 1)
            {
                sem_wait(&lock_collision);
            }
            sem_wait(&lock_left[1]);
        }
        //sem_post(&lock_headOfLine[1]);
        sleep(1);
    }
    else if (dir->dir.dir_original == 'S')
    {
        sem_wait(&lock_headOfLine[2]);
        vehicle_arrival[2]++;
        if (dir->dir.dir_target == 'S')
        {
           /* if (vehicle_arrival[2] == 1)
            {
                sem_wait(&lock_collision);
            }*/
            
            sem_wait(&lock_straight[1]);
            sem_wait(&lock_straight[2]);
            sem_wait(&lock_straight[3]);
            sem_wait(&lock_left[0]);
        }
        else if (dir->dir.dir_target == 'W')
        {
            if (vehicle_arrival[2] == 1)
            {
                sem_wait(&lock_collision);
            }
            sem_wait(&lock_right[2]);
        }
        else if (dir->dir.dir_target == 'E')
        {
            if (vehicle_arrival[2] == 1)
            {
                sem_wait(&lock_collision);
            }
            sem_wait(&lock_left[2]);
            sem_wait(&lock_straight[0]);
            sem_wait(&lock_straight[1]);
            sem_wait(&lock_straight[3]);
            sem_wait(&lock_left[1]);
        }
        //sem_post(&lock_headOfLine[2]);
        sleep(1);
    }
    else if (dir->dir.dir_original == 'W')
    {
        sem_wait(&lock_headOfLine[3]);
        vehicle_arrival[3]++;
        if (dir->dir.dir_target == 'N')
        {
            if (vehicle_arrival[3] == 1)
            {
                sem_wait(&lock_collision);
            }
            sem_wait(&lock_straight[3]);
        }
        else if (dir->dir.dir_target == 'W')
        {
            if (vehicle_arrival[3] == 1)
            {
                sem_wait(&lock_collision);
            }
            sem_wait(&lock_right[3]);
        }
        else if (dir->dir.dir_target == 'E')
        {
            if (vehicle_arrival[3] == 1)
            {
                sem_wait(&lock_collision);
            }
            sem_wait(&lock_left[3]);
        }
        sem_post(&lock_headOfLine[3]);
        sleep(1);
    }
    
}

void CrossIntersection(car *dir)
{
    
    //printf("cross\n");
    ////If our original direction is north
    if (dir->dir.dir_original == 'N')
    {
        
        //straight
        if (dir->dir.dir_target == 'N')
        {
            //printf("crossing N");
            sem_post(&lock_headOfLine[0]);
                Spin(1);
                Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
        }
        //right 
        else if (dir->dir.dir_target == 'E')
        {
            //printf("crossing E");
            sem_post(&lock_headOfLine[0]);
                Spin(2);
                Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
        }
        //left
        else if (dir->dir.dir_target == 'W')
        {
            //printf("crossing W");
            sem_post(&lock_headOfLine[0]);
                Spin(3);
                Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
        }
    }

    ////If our original direction is east
    if (dir->dir.dir_original == 'E')
    {
        //straight
        if (dir->dir.dir_target == 'E')
        {
            sem_post(&lock_headOfLine[1]);
                Spin(1);
                Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
        }
        //right
        else if (dir->dir.dir_target == 'S')
        {
            sem_post(&lock_headOfLine[1]);
                Spin(2);
                Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
        }
        //left
        else if (dir->dir.dir_target == 'N')
        {
            sem_post(&lock_headOfLine[1]);
                Spin(3);
                Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
        }
    }
        //If our original direction is south
    if (dir->dir.dir_original == 'S')
    {
        //straight
        if (dir->dir.dir_target == 'S')
        {
            sem_post(&lock_headOfLine[2]);
                Spin(1);
                Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
        }
        //right
        else if (dir->dir.dir_target == 'W')
        {
            sem_post(&lock_headOfLine[2]);
                Spin(2);
                Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
        }
        //left
        else if (dir->dir.dir_target == 'E')
        {
            sem_post(&lock_headOfLine[2]);
                Spin(3);
                Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
        }

    }
      //If our original direction is west
            if (dir->dir.dir_original == 'W')
            {
                //straight
                if (dir->dir.dir_target == 'W')
                {
                    sem_post(&lock_headOfLine[3]);
                        Spin(1);
                        Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
                }
                //right
                else if (dir->dir.dir_target == 'N')
                {
                    sem_post(&lock_headOfLine[3]);
                        Spin(2);
                        Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
                }
                //left
                else if (dir->dir.dir_target == 'E')
                {
                    sem_post(&lock_headOfLine[3]);
                        Spin(3);
                        Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "crossing");
                }

            }
        
    }

void ExitIntersection(car *dir)
{
    
    //sem_post(&);
    if (dir->dir.dir_original == 'N')
    {
        sem_wait(&lock_exit[0]);
        vehicle_arrival[0]--;
        if (dir->dir.dir_target == 'N')
        {

            sem_post(&lock_straight[1]);
            sem_post(&lock_straight[3]);
            sem_post(&lock_left[2]);
            sem_post(&lock_left[1]);
            
        }
        else if (dir->dir.dir_target == 'E')
        {

            sem_post(&lock_right[0]);
            sem_post(&lock_straight[1]);
        }
        else if (dir->dir.dir_target == 'W')
        {
            sem_post(&lock_left[0]);
            sem_post(&lock_straight[1]);
            sem_post(&lock_straight[3]);
            sem_post(&lock_left[2]);
        }
        sem_post(&lock_exit[0]);
        Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "Exiting");
        //sleep(1);
    }
    else if (dir->dir.dir_original == 'E')
    {
        sem_wait(&lock_exit[1]);
        vehicle_arrival[1]--;
        if (dir->dir.dir_target == 'E')
        {
            if (vehicle_arrival[1] == 0)
            {
                sem_post(&lock_collision);
            }
            sem_post(&lock_straight[1]);
        }
        else if (dir->dir.dir_target == 'S')
        {
            if (vehicle_arrival[1] == 0)
            {
                sem_post(&lock_collision);
            }
            sem_post(&lock_right[1]);
        }
        else if (dir->dir.dir_target == 'N')
        {
            if (vehicle_arrival[1] == 0)
            {
                sem_post(&lock_collision);
            }
            sem_post(&lock_left[1]);
        }
        sem_post(&lock_exit[1]);
        Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "Exiting");
        //sleep(1);
    }
    else if (dir->dir.dir_original == 'S')
    {
        sem_wait(&lock_exit[2]);
        vehicle_arrival[2]--;
        if (dir->dir.dir_target == 'S')
        {
            if (vehicle_arrival[2] == 0)
            {
                sem_post(&lock_collision);
            }
            sem_post(&lock_straight[1]);
            sem_post(&lock_straight[2]);
            sem_post(&lock_straight[3]);
            sem_post(&lock_left[0]);
        }
        else if (dir->dir.dir_target == 'W')
        {
            if (vehicle_arrival[2] == 0)
            {
                sem_post(&lock_collision);
            }
            sem_post(&lock_right[2]);
        }
        else if (dir->dir.dir_target == 'E')
        {
            if (vehicle_arrival[2] == 0)
            {
                sem_post(&lock_collision);
            }
            sem_post(&lock_left[2]);
        }
        sem_post(&lock_exit[2]);
        sem_post(&lock_straight[0]);
        sem_post(&lock_straight[1]);
        sem_post(&lock_straight[3]);
        sem_post(&lock_left[1]);
        Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "Exiting");
        //sleep(1);
    }
    else if (dir->dir.dir_original == 'W')
    {
        sem_wait(&lock_exit[3]);
        vehicle_arrival[3]--;
        if (dir->dir.dir_target == 'S')
        {
            if (vehicle_arrival[3] == 0)
            {
                sem_post(&lock_collision);
            }
            sem_post(&lock_straight[3]);
        }
        else if (dir->dir.dir_target == 'W')
        {
            if (vehicle_arrival[3] == 0)
            {
                sem_post(&lock_collision);
            }
            sem_post(&lock_right[3]);
        }
        else if (dir->dir.dir_target == 'E')
        {
            if (vehicle_arrival[3] == 0)
            {
                sem_post(&lock_collision);
            }
            sem_post(&lock_left[3]);
        }
        sem_post(&lock_exit[3]);
        Debugging(dir->cid, dir->dir.dir_original, dir->dir.dir_target, "Exiting");
        //sleep(1);
       
    }
  
}

int main(void) {
    
    printf("main\n");
    //create our cars and a thread for each
    int numOfCars = 8;
    car cars[numOfCars];
    pthread_t threads[numOfCars];

    for (int i = 0; i < numOfCars; i++)
    {
        cars[i].cid = i;
    }

    cars[0].arival_time = 1.1;
    cars[0].dir.dir_original = 'N';
    cars[0].dir.dir_target = 'N';

    cars[1].arival_time = 2.0;
    cars[1].dir.dir_original = 'N';
    cars[1].dir.dir_target = 'N';

    cars[2].arival_time = 3.3;
    cars[2].dir.dir_original = 'N';
    cars[2].dir.dir_target = 'W';

    cars[3].arival_time = 3.5;
    cars[3].dir.dir_original = 'S';
    cars[3].dir.dir_target = 'S';

    cars[4].arival_time = 4.2;
    cars[4].dir.dir_original = 'S';
    cars[4].dir.dir_target = 'E';

    cars[5].arival_time = 4.4;
    cars[5].dir.dir_original = 'N';
    cars[5].dir.dir_target = 'N';

    cars[6].arival_time = 5.7;
    cars[6].dir.dir_original = 'E';
    cars[6].dir.dir_target = 'N';

    cars[7].arival_time = 5.9;
    cars[7].dir.dir_original = 'W';
    cars[7].dir.dir_target = 'N';


    // initialize semaphore, only to be used with threads in this process, set value to 1
    sem_init(&semaphore, 0, 1);

    //headOfLine
    sem_init(&lock_headOfLine[0], 0, 1);
    sem_init(&lock_headOfLine[1], 0, 1);
    sem_init(&lock_headOfLine[2], 0, 1);
    sem_init(&lock_headOfLine[3], 0, 1);

    //exit
    sem_init(&lock_exit[0], 0, 1);
    sem_init(&lock_exit[1], 0, 1);
    sem_init(&lock_exit[2], 0, 1);
    sem_init(&lock_exit[3], 0, 1);

    //straight
    sem_init(&lock_straight[0], 0, 1);
    sem_init(&lock_straight[1], 0, 1);
    sem_init(&lock_straight[2], 0, 1);
    sem_init(&lock_straight[3], 0, 1);

    //left 
    sem_init(&lock_left[0], 0, 1);
    sem_init(&lock_left[1], 0, 1);
    sem_init(&lock_left[2], 0, 1);
    sem_init(&lock_left[3], 0, 1);

    //right
    sem_init(&lock_right[0], 0, 1);
    sem_init(&lock_right[1], 0, 1);
    sem_init(&lock_right[2], 0, 1);
    sem_init(&lock_right[3], 0, 1);

    sem_init(&lock_collision, 0, 1);
    
    t0 = GetTime();
    
    for (int i = 0; i < numOfCars; i++)
    {
        pthread_create(&threads[i], NULL, Car, (void*)&cars[i]);
        usleep((useconds_t)(cars[i].arival_time * 1000000));
    }

    for (int i = 0; i < numOfCars; i++)
    {
        pthread_join(threads[i], NULL);
    }

    //pthread_t *mythread;
    //
    //mythread = (pthread_t *)malloc(sizeof(*mythread));
    //
    //// start the thread
    //printf("Starting thread, semaphore is unlocked.\n");
    ////pthread_create(mythread, NULL, (void*)threadfunc, NULL);
    //
    //pthread_create(mythread, NULL, (void*)threadfunc(car.dir), car1);

    //getchar();
    //
    //sem_wait(&semaphore);
    //printf("Semaphore locked.\n");
    //
    //// do stuff with whatever is shared between threads
    //getchar();
    //
    //printf("Semaphore unlocked.\n");
    //sem_post(&semaphore);
    //
    //getchar();
    
    return 0;
}