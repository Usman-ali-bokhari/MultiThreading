#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <mutex>
#include <queue>
#include <semaphore.h>
#include "processes.h"

using namespace std;


double programCounter = 0.0; 
int contextSwitchCounter = 0;
int terminateFlag = 0;
int killCount = 0;
int runningCount = 0;
pthread_mutex_t cpu_lock, rrLock;
string cpuStates[4];
pthread_t* ids;
pthread_t extractProc, *CPU_Cores, progCount;
int choice, timeSlice;
string outputFile;
queue<PCB> ready;
queue<PCB> waiting;
PCB runningProc[4];
queue<PCB> new_Processes;
string ioProc = "< EMPTY >";
pthread_cond_t cond_var;

void terminateProc(PCB proc) {
    killCount++;
    runningCount--;
    if(killCount == numProcesses) {
        terminateFlag = 1;
    }
}


void wakeup(PCB proc){
    ioProc = "< EMPTY >";
    terminateProc(proc);
}

void yield(PCB proc){
    double init = programCounter;
    ioProc = proc.ProcName;
    while((programCounter - init) <= proc.ioTime){
        continue;
    }
    wakeup(proc);
}

void sortByArrivalTime(){
    for(int i = 0; i < numProcesses; i++){
        for(int j = 0; j < numProcesses - 1; j++){
            if(procList[j].arrivalTime > procList[j + 1].arrivalTime){
                PCB temp = procList[j];
                procList[j] = procList[j + 1];
                procList[j + 1] = temp;
            }
        }
    }

    for(int i = 0; i < numProcesses; i++) {
        new_Processes.push(procList[i]);
    }
}

void FCFS(PCB Process) {
    double init = programCounter;
    while((programCounter - init) <= Process.cpuTime){
        continue;
    }
    if(Process.ProcType == 'I') {
        yield(Process);
    }
}



void RoundRobin(PCB Process, int timeSlice, int id){
    double init = programCounter;
    while((programCounter - init) <= timeSlice && runningProc[id].runTime < Process.cpuTime) {
        continue;
    }
    if(runningProc[id].runTime >= runningProc[id].cpuTime){
        terminateProc(Process);
    }
    else{
        pthread_mutex_lock(&rrLock);
        ready.push(runningProc[id]);
        pthread_mutex_unlock(&rrLock);
    }
    if(Process.ProcType == 'I') {
        yield(Process);
    }
}


void PriorityBased(PCB Proc, int id){
    double init = programCounter;
    while((programCounter - init) <= Proc.cpuTime){
        if(Proc.priority < ready.front().priority){
            pthread_mutex_lock(&rrLock);
            ready.push(runningProc[id]);
            pthread_mutex_unlock(&rrLock);
            return;
        }
        continue;
    }
    if(Proc.ProcType == 'I') {
        yield(Proc);
    }
}

void contextSwitch(PCB process, int choice, int timeSlice, int id){
    if(choice == 1) {
        cpuStates[id] = process.ProcName;
        contextSwitchCounter++;
        runningCount++;
        FCFS(process);
        runningProc[id] = process;
        //sleep(1);
        contextSwitchCounter++;
        terminateProc(process);
    }

    if(choice == 2){
        cpuStates[id] = process.ProcName;
        runningProc[id] = process;
        contextSwitchCounter++;
        RoundRobin(process, timeSlice, id);
        contextSwitchCounter++;
    }

    if(choice == 3){
        cpuStates[id] = process.ProcName;
        runningProc[id] = process;
        contextSwitchCounter++;
        PriorityBased(process, id);
        contextSwitchCounter++;
    }
}



void Schedule(int choice, int timeSlice, int id) {
    pthread_mutex_lock(&cpu_lock);
    if(!ready.empty()) {
        PCB currProc = ready.front();
        //cout << "\n\nThread # " << pthread_self() << "  " << currProc.ProcName;
        ready.pop();
        pthread_mutex_unlock(&cpu_lock);
        contextSwitch(currProc, choice, timeSlice, id);
    }
    else{
        pthread_mutex_unlock(&cpu_lock);
        pthread_cond_wait(&cond_var, &cpu_lock);
        cpuStates[id] = "(IDLE)";
        runningProc[id] = idle;
    }
}

void sortReady() {
    PCB temp[numProcesses];
    int count = 0;
    while(!ready.empty()){
        temp[count] = ready.front();
        count++;
        ready.pop();
    }
    
    for(int i = 0; i < count; i++){
        for(int j = 0; j < count; j++){
            if(temp[j].priority < temp[j + 1].priority){
                PCB temp1 = temp[j + 1];
                temp[j + 1] = temp[j];
                temp[j] = temp1;
            }
        }
    }

    for(int i = 0; i < count; i++){
        ready.push(temp[i]);
    }
}


