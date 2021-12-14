//
// Created by bo on 2021/12/14.
//

#ifndef AVPLAYER_BASIC_H
#define AVPLAYER_BASIC_H
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <exception>
#include <csignal>
#include <pthread.h>
#include <thread>

void customThreadCallback2(int t) ;
void createThread();
void createThreadsByPThead();
void *customThreadCallback(void *params);
//------------------------------------------------------------------------------------------
void signalHandler(int signum);
void registerSignal();
//------------------------------------------------------------------------------------------
template <class T>
class Bike {
public:
    void ride(T t);
};

//------------------------------------------------------------------------------------------
template <typename T>
T const& maxNum(T const& a, T const& b) {
    return  a > b? a:b;
}

//------------------------------------------------------------------------------------------
class MyException: public std::exception {
    char *what() const throw() {
        return "my exception";
    }
};

//------------------------------------------------------------------------------------------
void write();

void read();


//------------------------------------------------------------------------------------------
inline void funInline();

class Basic {
private:
    int length;
public:
    Basic(int l);
    friend void testFriendFunc(Basic basic);
};

class D {
public:
    D() {
        std::clog << "D()" << std::endl;
    }

    ~D() {
        std::clog << "~D()" << std::endl;
    }
    int a;
};

class C : virtual public D{
public:
    C() {
        std::clog << "C()" << std::endl;
    }

    ~C() {
        std::clog << "~C()" << std::endl;
    }
    int a;
};

class B : virtual public D{
public:
    B() {
        std::clog << "B()" << std::endl;
    }

    ~B() {
        std::clog << "~B()" << std::endl;
    }
    int a;
};

class A : C, B{
public:
    A() {
        std::clog << "A()" << std::endl;
    }

    ~A() {
        std::clog << "~A()" << std::endl;
    }
    int a;
};

#endif //AVPLAYER_BASIC_H
