//
//  main.cpp
//  ReaderWriterProblem
//
//  Created by Florian Frank on 27.06.16.
//  Copyright © 2016 Florian_Frank. All rights reserved.
//

#include <iostream>
#include <semaphore.h>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include <chrono>

//testvalue
int value;
//counter for writers
int rc = 0;
static int reader_nr=0;
//counter for counter id
static int writer_nr=0;
//mutex for cout statements
std::mutex outputMutex;
//mutex for rc value
std::mutex readerMutex;
//semaphore 
sem_t *readWriteSemp;
char isLockedBy;

void reader();
void writer();

int main(int argc, const char * argv[]) {
    readWriteSemp = sem_open("Semaphore", O_CREAT,O_RDWR,0);
    std::thread writerThread(writer);
    std::thread readerThread1(reader);
    std::thread readerThread2(reader);
    std::thread readerThread3(reader);
    writerThread.join();
    readerThread1.join();
    readerThread2.join();
    readerThread3.join();
    return 0;
}


void reader(){
    const int id =reader_nr++;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand()%10));
        outputMutex.lock();
        std::cout<<"Reader <"<<id<<"> requesting read access" << std::endl;
        outputMutex.unlock();
        readerMutex.lock();
        rc ++;
        if(isLockedBy=='w') {
            outputMutex.lock();
            std::cout<<"Reader <"<<id<<"> denied read access" << std::endl;
            outputMutex.unlock();
        }
        if(rc==1){
            sem_wait(readWriteSemp);
            isLockedBy='r';
        }
        readerMutex.unlock();
        outputMutex.lock();
        std::cout<<"Reader <"<<id<<"> was granted read access: " << value << std::endl;
        outputMutex.unlock();
        readerMutex.lock();
        rc --;
        outputMutex.lock();
        std::cout<<"Reader <"<<id<<"> finished reading" << std::endl;
        outputMutex.unlock();
        isLockedBy='0';
        if(rc==0) sem_post(readWriteSemp);
            readerMutex.unlock();
        
    }
}

void writer(){
    const int id = writer_nr++;
          while(true){
            std::this_thread::sleep_for(std::chrono::milliseconds(rand()%10));
           outputMutex.lock();
              std::cout<<"Writer <"<<id<<"> requesting write access" << std::endl;
              outputMutex.unlock();
              if(isLockedBy=='r'){
                  outputMutex.lock();
                  std::cout<<"Writer <"<<id<<"> denied write access" << std::endl;
                  outputMutex.unlock();
              }
              sem_wait(readWriteSemp);
              isLockedBy='w';
              outputMutex.lock();
              std::cout<<"Writer <"<<id<<"> was granted write access" << std::endl;
              outputMutex.unlock();
        value = rand();
              outputMutex.lock();
              std::cout<<"Writer <"<<id<<"> finished writing" << std::endl;
              outputMutex.unlock();
              isLockedBy= 'O';
        sem_post(readWriteSemp);
    }
}