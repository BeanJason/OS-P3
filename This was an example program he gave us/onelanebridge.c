sem_t lock_arrive[2];  // lock for flow at each direction
sem_t lock_exit[2];  // lock for flow at each direction
sem_t lock_capacity;  // lock for capacity limit
sem_t lock_collision;  // lock for collision

int vehicles_arrival[2] = {0,0};
double t0, t;
char dirs[2] = "NS";

typedef struct _vehicle_info {
    int id;
    int dir;
    double inter_arrival_t;
} vehicle_info;


double GetTime()
{
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec/1e6;
}

void Spin(int howlong)
{
    double t = GetTime();
    while ((GetTime() - t) < (double)howlong)
        ; // do nothing in loop
}

void Debugging(int id, int dir, char *str)
{
    printf("Time %4.1f: Vehicle %2d (%c) %s\n", GetTime()-t0, id, dirs[dir], str);
}

void ArriveBridge(vehicle_info *vi) {
    //Debugging(vi->id, vi->dir, "arriving");
    sem_wait(&lock_arrive[vi->dir]);
    vehicles_arrival[vi->dir]++;
    if (vehicles_arrival[vi->dir] == 1)
        sem_wait(&lock_collision);
    sem_wait(&lock_capacity);
    sem_post(&lock_arrive[vi->dir]);
}

void CrossBridge(vehicle_info *vi)
{
    Debugging(vi->id, vi->dir, "crossing");
    Spin(5);
}

void ExitBridge(vehicle_info *vi)
{
    sem_wait(&lock_exit[vi->dir]);
    vehicles_arrival[vi->dir]--;
    sem_post(&lock_capacity);
    if (vehicles_arrival[vi->dir] == 0)
        sem_post(&lock_collision);
    sem_post(&lock_exit[vi->dir]);
    //Debugging(vi->id, vi->dir, "exiting");
}

void* VehicleAction(void* arg) {
    vehicle_info *vi = (vehicle_info *) arg;
    ArriveBridge(vi);
    CrossBridge(vi);
    ExitBridge(vi);
}

int main(int argc, char *argv[])
{
    int i;
    int NN = 13;
    vehicle_info vehicles[NN];
    pthread_t threads[NN];

    sem_init(&lock_arrive[0], 0, 1);
    sem_init(&lock_arrive[1], 0, 1);
    sem_init(&lock_exit[0], 0, 1);
    sem_init(&lock_exit[1], 0, 1);
    sem_init(&lock_collision, 0, 1);
    sem_init(&lock_capacity, 0, 3);
    
    for (i=0; i<NN; i++){
        vehicles[i].id = i;
        vehicles[i].dir = 0;
        vehicles[i].inter_arrival_t = 1;
    }

    vehicles[2].dir = 1; vehicles[3].dir = 1; vehicles[4].dir = 1; vehicles[7].dir = 1; vehicles[8].dir = 1;

    t0 = GetTime();
    for (i=0; i<NN; i++){
        pthread_create(&threads[i], NULL, VehicleAction, (void*)&vehicles[i]);
        usleep((useconds_t)(vehicles[i].inter_arrival_t*1000000));
    }
    
    for (i=0; i<NN; i++){
        pthread_join(threads[i], NULL);
    }

}
