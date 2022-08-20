#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <mutex>
#include <semaphore.h>
#include "processes.h"
#include "scheduler.h"
#include <climits>
#include <chrono>
#include <fstream>
#include <thread>

using namespace std;


int tempCount;


void implement_start(){
    pthread_mutex_init(&cpu_lock, NULL);
    pthread_mutex_init(&rrLock, NULL);
    pthread_cond_init(&cond_var, NULL);
    InitProcesses();
    sortByArrivalTime();
    showProcesses();
    printHeader();
    CPU_Cores = (pthread_t*)malloc(CPU_Count * sizeof(pthread_t));
    ids = (pthread_t*)malloc(CPU_Count * sizeof(pthread_t));
}

void* programCount(void* arg) {
    fstream MyFile;
    MyFile.open(outputFile,ios::out|ios::trunc);
    while(!terminateFlag){
       
       cout << setprecision(5) << programCounter << "           " << ready.size() << "           " << waiting.size();
       MyFile << setprecision(5) << programCounter << "           " << ready.size() << "           " << waiting.size();
 
       for(int i = 0; i < CPU_Count; i++){
           cout << "           " << cpuStates[i];
           MyFile << "           " << cpuStates[i];
           runningProc[i].runTime += 0.1;
       }
       cout << "  " << ioProc << "\n\n";
       MyFile << "  " << ioProc << "\n\n";
       programCounter = programCounter + 0.1;
       this_thread::sleep_for(chrono::milliseconds(50));
    }
    pthread_exit(NULL);
}

void* CPU(void* arg){ 
    int id = tempCount;
    tempCount++;
    while(!terminateFlag){
        Schedule(choice, timeSlice, id);
    }
    pthread_exit(NULL);
}

void* extractProcess(void* arg) {
    while(!new_Processes.empty()) { 
       // cout << "Checking";
        if(new_Processes.front().arrivalTime <= programCounter && !new_Processes.empty()) {
            //cout << "Thread # " << pthread_self() << "  " << new_Processes.front().ProcName << "    " << programCounter << endl;
            ready.push(new_Processes.front());
            pthread_cond_broadcast(&cond_var);
            if(choice == 3){
                sortReady();
            }
            //cout << "\n" << ready.front().ProcName;
            new_Processes.pop();
            //sleep(1);
            continue;
        }
        
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv){
    system("clear");
    filename = argv[1];
    if(argc == 6){
        CPU_Count = stoi(argv[2]);
        choice = 2;
        timeSlice = stoi(argv[4]);
        outputFile = argv[5];
    }
    else if(argc == 5){
        CPU_Count = stoi(argv[2]);
        if(argv[3][0] == 'f'){
            choice = 1;
        }
        else{
            choice = 3;
        }
        outputFile = argv[4];
        timeSlice = -1;

    }

    /* cout << "CPU COUNT: " << CPU_Count << endl;
    cout << "Time Slice: " << timeSlice << endl;
    cout << "PUTSF " << outputFile << endl; */
    implement_start();

    pthread_create(&progCount, NULL, programCount, NULL);   
    pthread_create(&extractProc, NULL, extractProcess, NULL);
    for(int i = 0; i < CPU_Count; i++){
        pthread_create(&CPU_Cores[i], NULL, CPU, NULL);
    }  
    pthread_join(progCount, NULL);
    cout << "\n\nContext Switches: " << contextSwitchCounter << endl;
    cout << "Total Execution Time: " << programCounter << endl; 
}