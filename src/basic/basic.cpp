//
// Created by bo on 2021/12/14.
//

#include <unistd.h>
#include "basic.h"



Basic::Basic(int l) : length(l){

}

void testFriendFunc(Basic basic) {
    std::clog << "length is " << basic.length << std::endl;
}

void funInline() {

}

void write() {
    std::ofstream ofstream;
    ofstream.open("resources/aaa.dat", std::ios::out);

    char buf[100];
    std::cout << "please input:" << std::endl;
    std::cin.getline(buf, 100);
    ofstream << buf << std::endl;

    ofstream.close();
}

void read() {
    std::ifstream ifstream;
    ifstream.open("resources/aaa.dat", std::ios::in);

    char buf[100];
    ifstream >> buf;
    ifstream.close();

    std::cout << "content:" << buf << std::endl;

}

void signalHandler(int signum) {
    std::clog << "signum" << std::endl;
    exit(signum);
}

void registerSignal() {
    signal(SIGINT, signalHandler);
    int i = 0;
    while (++i) {
        std::clog << "going to sleep" << std::endl;
        if ( i == 3) {
            raise(SIGINT);
        }
        sleep(1);
    }
}

void * customThreadCallback(void * parameters) {
    int thread_id = *(int *)parameters;
    std::clog << "thread created. id is :" << thread_id << std::endl;

    pthread_exit(NULL);
}

static const uint8_t NUMOFTHREAD = 5;

void createThreadsByPThead() {
    pthread_t threads[NUMOFTHREAD];
    int rc = 0;
    int *temp = new int[NUMOFTHREAD];

    pthread_attr_t attr;
    void *status;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < NUMOFTHREAD; ++i) {

        temp[i] = i + 0xFF;
        std::clog << "thread creat. id is :" << temp[i] << std::endl;
        rc = pthread_create(&threads[i], NULL , customThreadCallback, (void *)&temp[i]);

        if (rc) {
            std::cerr << "create thread failed." << rc << std::endl;
            exit(-1);
        }
    }
    pthread_attr_destroy(&attr);
    for (int i = 0; i < NUMOFTHREAD; ++i) {
        rc = pthread_join(threads[i], &status);
        if (rc) {
            std::cerr << "join failed cause of" << rc << std::endl;
            exit(-1);
        }
    }
    std::clog << "main program exit" << std::endl;
    //pthread_exit(NULL);
}

void customThreadCallback2(int t) {
    std::clog << "custom id------>" << t << std::endl;
}

void createThread() {
    int a = 10;
    std::thread t1(customThreadCallback2, a);
    t1.join();
}