#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "bits/stdc++.h"
using namespace std;

default_random_engine generator;
poisson_distribution<int> distribution(4.1);

int N_KIOSK, N_BELT, N_BELTCAP;
int T_CHECKIN, T_SECURITY, T_BOARDING, T_VIPWALK;

int lrCount = 0, rlCount = 0;
pthread_mutex_t lr_mutex, rl_mutex;

sem_t sem_availableKiosk;
sem_t sem_availableBelt;

pthread_mutex_t mutex_kiosk;
pthread_mutex_t mutex_belt;
pthread_mutex_t mutex_vipChannel;
pthread_mutex_t mutex_specialKiosk;
pthread_mutex_t mutex_boarding;

bool* kioskIsFree;
int* securityBeltSize;

/* ************************* */
void checkIn(int id);
void visitSpecialKiosk(int id, bool isVip);
void useVIPWalk(int id, bool isVip);
/* ************************* */


int findSecurityBelt() {
    for (int i = 0; i < N_BELT; i++) {
        if (securityBeltSize[i] < N_BELTCAP) {
            return i;
        }
    }
    return -1;
}

int findFreeKiosk() {
    for (int i = 0; i < N_KIOSK; i++) {
        if (kioskIsFree[i]) {
            return i;
        }
    }
    return -1;
}

void boardPlane(int id, bool isVip) {
    bool isCareless = (rand() % 2 == 0);
    if (isCareless) {
        string msg;
        if (isVip)  msg = "Passenger " + to_string(id) + " (VIP) has lost boarding pass.\n";
        else        msg = "Passenger " + to_string(id) + " has lost boarding pass.\n";
        cout << msg;
        visitSpecialKiosk(id, isVip);
    }
    else {
        pthread_mutex_lock(&mutex_boarding);
            sleep(T_BOARDING);
            string msg;
            if (isVip)  msg = "Passenger " + to_string(id) + " (VIP) has boarded the plane.\n";
            else        msg = "Passenger " + to_string(id) + " has boarded the plane.\n";
            cout << msg;
        pthread_mutex_unlock(&mutex_boarding);
    }
}

void checkSecurity(int id, bool isVip) {
    sem_wait(&sem_availableBelt);

        pthread_mutex_lock(&mutex_belt);
        int belt = findSecurityBelt();
        securityBeltSize[belt]++;
        pthread_mutex_unlock(&mutex_belt);

        sleep(T_SECURITY);
        string msg = "Passenger " + to_string(id) + " is passing security at belt " + to_string(belt) + ".\n";
        cout << msg;

        pthread_mutex_lock(&mutex_belt);
        securityBeltSize[belt]--;
        pthread_mutex_unlock(&mutex_belt);
    
    sem_post(&sem_availableBelt);
    boardPlane(id, isVip);
}

void visitSpecialKiosk(int id, bool isVip) {

    string msg;
    if (isVip)  msg = "Passenger " + to_string(id) + " (VIP) is waiting to access VIP channel.\n";
    else        msg = "Passenger " + to_string(id) + " is waiting to access VIP channel.\n";
    cout << msg;

    pthread_mutex_lock(&mutex_vipChannel);
        sleep(T_VIPWALK);
        if (isVip)  msg = "Passenger " + to_string(id) + " (VIP) has used the VIP channel.\n";
        else        msg = "Passenger " + to_string(id) + " has used the VIP channel.\n";
        cout << msg;
    pthread_mutex_unlock(&mutex_vipChannel);

    pthread_mutex_lock(&mutex_specialKiosk);
        sleep(T_CHECKIN);
        if (isVip)  msg = "Passenger " + to_string(id) + " (VIP) is checking in at special kiosk\n";
        else        msg = "Passenger " + to_string(id) + " is checking in at special kiosk\n";
        cout << msg;
    pthread_mutex_unlock(&mutex_specialKiosk);

    useVIPWalk(id, isVip);
}

void useVIPWalk(int id, bool isVip) {
    
    string msg;
    if (isVip)  msg = "Passenger " + to_string(id) + " (VIP) has reached the VIP channel.\n";
    else        msg = "Passenger " + to_string(id) + " has reached the VIP channel.\n";
    cout << msg;

    pthread_mutex_lock(&lr_mutex);
        if (lrCount == 0) {
            pthread_mutex_lock(&mutex_vipChannel);
        }
        lrCount++;
    pthread_mutex_unlock(&lr_mutex);

    sleep(T_VIPWALK);

    if (isVip)  msg = "Passenger " + to_string(id) + " (VIP) has used the VIP channel to reach boarding.\n";
    else        msg = "Passenger " + to_string(id) + " has used the VIP channel to reach boarding.\n";
    cout << msg;

    pthread_mutex_lock(&lr_mutex);
        lrCount--;
        if (lrCount == 0) {
            pthread_mutex_unlock(&mutex_vipChannel);
        }
    pthread_mutex_unlock(&lr_mutex);

    boardPlane(id, isVip);
}

void checkIn(int id, bool isVip) {
    sem_wait(&sem_availableKiosk);
            
        pthread_mutex_lock(&mutex_kiosk);
        int kiosk = findFreeKiosk();
        kioskIsFree[kiosk] = false;
        pthread_mutex_unlock(&mutex_kiosk);

        sleep(T_CHECKIN);
        string msg;
        if (isVip)  msg = "Passenger " + to_string(id) + " (VIP) is checking in at kiosk " + to_string(kiosk) + "\n";
        else        msg = "Passenger " + to_string(id) + " is checking in at kiosk " + to_string(kiosk) + "\n";
        cout << msg;
        
        pthread_mutex_lock(&mutex_kiosk);
        kioskIsFree[kiosk] = true;
        pthread_mutex_unlock(&mutex_kiosk);

    sem_post(&sem_availableKiosk);

    if (isVip)  useVIPWalk(id, isVip);
    else        checkSecurity(id, isVip);
}

void* arriveAirport(void* arg) {
    int id = *((int*)arg);
    delete (int*)arg;
    arg = nullptr;

    bool isVip = (rand() % 2 == 0);
    string msg;
    if (isVip)  msg = "Passenger " + to_string(id) + " (VIP) has arrived at the airport\n";
    else        msg = "Passenger " + to_string(id) + " has arrived at the airport\n";
    cout << msg;
    checkIn(id, isVip);
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
    /*********************************/

    pthread_mutex_init(&mutex_belt, NULL);
    pthread_mutex_init(&mutex_kiosk, NULL);
    pthread_mutex_init(&mutex_vipChannel, NULL);
    pthread_mutex_init(&mutex_specialKiosk, NULL);
    pthread_mutex_init(&mutex_boarding, NULL);

    pthread_mutex_init(&lr_mutex, NULL);
    pthread_mutex_init(&rl_mutex, NULL);

    sem_init(&sem_availableKiosk, 0, N_KIOSK);
    sem_init(&sem_availableBelt,  0, N_BELT * N_BELTCAP);

    int ret;
    pthread_t customer[10];
    for (int i = 0; i < 10; i++) {
        int* id = new int(i);
        ret = pthread_create(&customer[i], NULL, arriveAirport, (void*)id);

        if (ret != 0) {
            cout << "Error: pthread_create()\n";
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(customer[i], NULL);
    }

    return 0;
}
