#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

#define timestamp std::chrono::high_resolution_clock::time_point
#define current_time() std::chrono::high_resolution_clock::now()
#define get_seconds(t2, t1) duration_cast<seconds>(t2 - t1).count()

default_random_engine generator;
poisson_distribution<int> distribution(4.1);

int lrCount = 0, rlCount = 0;
pthread_mutex_t mutex_lrCount, mutex_rlCount;

bool* kioskIsFree;
pthread_mutex_t mutex_kiosk;

int* securityBeltSize;
pthread_mutex_t mutex_belt;
pthread_mutex_t mutex_vipChannel;
pthread_mutex_t mutex_vipChannelAvailable;
pthread_mutex_t mutex_specialKiosk;
pthread_mutex_t mutex_boarding;

sem_t sem_availableKiosk;
sem_t sem_availableBelt;

timestamp initialTime;
int N_KIOSK, N_BELT, N_BELTCAP;
int T_CHECKIN, T_SECURITY, T_BOARDING, T_VIPWALK;

/* ************************* */
void checkIn(int id);
void visitSpecialKiosk(int id, bool isVip, int time, timestamp lastTime);
void useVIPChannel(int id, bool isVip, int time, timestamp lastTime);
/* ************************* */

/***** Helper Functions ******/
void printOut(int id, bool isVip, string description, int time) {
    string msg;
    if (isVip)  msg = "Passenger " + to_string(id) + " (VIP) " + description + " at time " + to_string(time) + "\n";
    else        msg = "Passenger " + to_string(id) + " " + description + " at time " + to_string(time) + "\n";
    cout << msg;
}

int findSecurityBelt() {
    for (int i = 0; i < N_BELT; i++) {
        if (securityBeltSize[i] < N_BELTCAP) {
            return i+1;
        }
    }
    return -1;
}

int findFreeKiosk() {
    for (int i = 0; i < N_KIOSK; i++) {
        if (kioskIsFree[i]) {
            return i+1;
        }
    }
    return -1;
}
/* ************************* */
/* ************************* */

void boardPlane(int id, bool isVip, int time, timestamp lastTime) {
    bool isCareless = (rand() % 2 == 0);
    if (isCareless) {
        printOut(id, isVip, "has lost boarding pass", time);
        visitSpecialKiosk(id, isVip, time, current_time());
    }
    else {
        printOut(id, isVip, "has started waiting to be boarded", time);

        pthread_mutex_lock(&mutex_boarding);
            time += get_seconds(current_time(), lastTime);

            printOut(id, isVip, "has started boarding the plane", time);
            sleep(T_BOARDING);
            time += T_BOARDING;
            printOut(id, isVip, "has boarded the plane", time);

        pthread_mutex_unlock(&mutex_boarding);
    }
}

void checkSecurity(int id, bool isVip, int time, timestamp lastTime) {

    timestamp waitingTime = current_time();
    time += get_seconds(waitingTime, lastTime);
    printOut(id, isVip, "has started waiting for security check", time);

    sem_wait(&sem_availableBelt);

        pthread_mutex_lock(&mutex_belt);
        int belt = findSecurityBelt();
        securityBeltSize[belt]++;
        pthread_mutex_unlock(&mutex_belt);

        time += get_seconds(current_time(), waitingTime);

        printOut(id, isVip, "has started the security check in belt " + to_string(belt), time);
        sleep(T_SECURITY);
        time += T_SECURITY;
        printOut(id, isVip, "has finished the security check", time);

        pthread_mutex_lock(&mutex_belt);
        securityBeltSize[belt]--;
        pthread_mutex_unlock(&mutex_belt);
    
    sem_post(&sem_availableBelt);
    boardPlane(id, isVip, time, current_time());
}

void visitSpecialKiosk(int id, bool isVip, int time, timestamp lastTime) {

    printOut(id, isVip, "is waiting to use the VIP channel to visit special kiosk", time);
    pthread_mutex_lock(&mutex_vipChannelAvailable);
    pthread_mutex_unlock(&mutex_vipChannelAvailable);

    pthread_mutex_lock(&mutex_rlCount);
        if (rlCount == 0) {
            pthread_mutex_lock(&mutex_vipChannel);
        }
        rlCount++;
    pthread_mutex_unlock(&mutex_rlCount);

    time += get_seconds(current_time(), lastTime);

    printOut(id, isVip, "got on the VIP channel to visit special kiosk", time);
    sleep(T_VIPWALK);
    time += T_VIPWALK;
    printOut(id, isVip, "got off the VIP channel and waiting to use special kiosk", time);

    pthread_mutex_lock(&mutex_rlCount);
        rlCount--;
        if (rlCount == 0) {
            pthread_mutex_unlock(&mutex_vipChannel);
        }
    pthread_mutex_unlock(&mutex_rlCount);

    timestamp waitingTime = current_time();
    pthread_mutex_lock(&mutex_specialKiosk);

    time += get_seconds(current_time(), waitingTime);

    printOut(id, isVip, "has started using the special kiosk", time);
    sleep(T_CHECKIN);
    time += T_CHECKIN;
    printOut(id, isVip, "has finished using the special kiosk", time);

    pthread_mutex_unlock(&mutex_specialKiosk);
    useVIPChannel(id, isVip, time, current_time());
}

void useVIPChannel(int id, bool isVip, int time, timestamp lastTime) {
    
    printOut(id, isVip, "is waiting to use the VIP channel to reach boarding", time);

    pthread_mutex_lock(&mutex_lrCount);
        if (lrCount == 0) {
            pthread_mutex_lock(&mutex_vipChannelAvailable);
            pthread_mutex_lock(&mutex_vipChannel);
        }
        lrCount++;
    pthread_mutex_unlock(&mutex_lrCount);

    time += get_seconds(current_time(), lastTime);

    printOut(id, isVip, "got on the VIP channel to reach boarding", time);
    sleep(T_VIPWALK);
    time += T_VIPWALK;
    printOut(id, isVip, "got off the VIP channel and reached boarding", time);

    pthread_mutex_lock(&mutex_lrCount);
        lrCount--;
        if (lrCount == 0) {
            pthread_mutex_unlock(&mutex_vipChannel);
            pthread_mutex_unlock(&mutex_vipChannelAvailable);
        }
    pthread_mutex_unlock(&mutex_lrCount);
    boardPlane(id, isVip, time, current_time());
}

void checkIn(int id, bool isVip, int time, timestamp lastTime) {
    sem_wait(&sem_availableKiosk);
            
        pthread_mutex_lock(&mutex_kiosk);
        int kiosk = findFreeKiosk();
        kioskIsFree[kiosk] = false;
        pthread_mutex_unlock(&mutex_kiosk);

        time += get_seconds(current_time(), lastTime);

        printOut(id, isVip, "has started self-check in at kiosk " + to_string(kiosk), time);
        sleep(T_CHECKIN);
        time += T_CHECKIN;
        printOut(id, isVip, "has finished self-check in at kiosk " + to_string(kiosk), time);

        pthread_mutex_lock(&mutex_kiosk);
        kioskIsFree[kiosk] = true;
        pthread_mutex_unlock(&mutex_kiosk);

    sem_post(&sem_availableKiosk);

    if (isVip)  useVIPChannel(id, isVip, time, current_time());
    else        checkSecurity(id, isVip, time, current_time());
}

void* arriveAirport(void* arg) {
    int id = *((int*)arg);
    delete (int*)arg;
    arg = nullptr;

    bool isVip = (rand() % 2 == 0);

    timestamp t = current_time();
    int time = get_seconds(t, initialTime);
    printOut(id, isVip, "has arrived at the airport", time);

    checkIn(id, isVip, time, current_time());
    return NULL;
}


int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    cin >> N_KIOSK >> N_BELT >> N_BELTCAP;
    cin >> T_CHECKIN >> T_SECURITY >> T_BOARDING >> T_VIPWALK;

    kioskIsFree = new bool[N_KIOSK];
    fill(kioskIsFree, kioskIsFree + N_KIOSK, true);

    securityBeltSize = new int[N_BELT];
    fill(securityBeltSize, securityBeltSize + N_BELT, 0);

    /*  Mutex and Sem Initialization */
    pthread_mutex_init(&mutex_belt, NULL);
    pthread_mutex_init(&mutex_kiosk, NULL);
    pthread_mutex_init(&mutex_vipChannel, NULL);
    pthread_mutex_init(&mutex_vipChannelAvailable, NULL);
    pthread_mutex_init(&mutex_specialKiosk, NULL);
    pthread_mutex_init(&mutex_boarding, NULL);
    pthread_mutex_init(&mutex_lrCount, NULL);
    pthread_mutex_init(&mutex_rlCount, NULL);

    sem_init(&sem_availableKiosk, 0, N_KIOSK);
    sem_init(&sem_availableBelt,  0, N_BELT * N_BELTCAP);
    /*********************************/

    #define INTERVAL_TIME   3   //  Interval between different batches of passenger in seconds
    #define PASSENGER_BATCH 3   //  Total number of passenger batch

    int ret;                        //  To read the return value from each `pthread` methods
    int passengerCount = 0;         //  Total count of passengers for uniquely identifying
    initialTime = current_time();   //  Initial timestamp to determine program startup time (GLOBAL)

    for (int i=0; i < PASSENGER_BATCH; i++) {
        int batchCount = distribution(generator);
        pthread_t passenger[batchCount];

        for (int j=0; j < batchCount; j++) {
            int* id = new int(passengerCount + 1);
            ret = pthread_create(&passenger[j], NULL, arriveAirport, (void*)id);
            if (ret != 0)   cout << "Error: pthread_create() failed\n";
            passengerCount++;
        }
        
        for (int j=0; j < batchCount; j++) {
            ret = pthread_join(passenger[j], NULL);
            if (ret != 0)   cout << "Error: pthead_join() failed\n";
        }
        sleep(INTERVAL_TIME);
    }

    /*      Mutex and Sem Destroy      */
    pthread_mutex_destroy(&mutex_belt);
    pthread_mutex_destroy(&mutex_kiosk);
    pthread_mutex_destroy(&mutex_vipChannel);
    pthread_mutex_destroy(&mutex_vipChannelAvailable);
    pthread_mutex_destroy(&mutex_specialKiosk);
    pthread_mutex_destroy(&mutex_boarding);
    pthread_mutex_destroy(&mutex_lrCount);
    pthread_mutex_destroy(&mutex_rlCount);

    sem_destroy(&sem_availableBelt);
    sem_destroy(&sem_availableBelt);
    /**********************************/

    return 0;
}