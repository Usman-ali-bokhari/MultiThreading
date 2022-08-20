#ifndef processes_h
#define processes_h

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <fstream>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <queue>


using namespace std;

string filename;
int CPU_Count;
typedef struct PCB{
    int ProcID;
    char* ProcName;
    int priority;
    float arrivalTime;
    char ProcType;
    float cpuTime;
    float ioTime;
    float runTime;
}processes;

PCB idle;

int numProcesses = -1;
processes* procList;

void printHeader() {
    printf("Time\t  Running \tReady\t  Waiting\t  ");
    for(int i = 0; i < CPU_Count; i++){
        printf("CPU %d\t      ", i);
    }
    printf("< I/O Queue >\n\n====\t  ======= \t=====\t  =======\t  ");
    for(int i = 0; i < CPU_Count; i++){
        printf("=====\t      ");
    }
    printf("=============\n\n\n");
}

void countProcesses(){
    ifstream infile;
    infile.open(filename);
    string temp;
    while(!infile.eof()){
        getline(infile, temp);
        numProcesses++;
    }
    infile.close();
}

void storeProcesses() {
    string line;
    int counter = -2;
    ifstream infile;
    infile.open(filename);
    while (!infile.eof()) {
        counter++;
        getline(infile, line);
        if (counter == -1) {
            continue;
        }
        int pos = 0;
        for (int i = 0; i < line.length(); i++) {
            if (line[i] == ' ' || line[i] == 9) {
                continue;
            }
            int countSegment = 0;
            int tempIndex = i;
            string temp_ref;
            while (line[tempIndex] != 9 && line[tempIndex] != ' ') {
                char a = line[tempIndex];
                temp_ref += line[tempIndex];
                tempIndex++;
                countSegment++;
            }
            i += countSegment;
            if (pos == 0) {
                procList[counter].ProcID = counter + 1;
                procList[counter].ProcName = (char*)malloc(temp_ref.length() * sizeof(char));
                strcpy(procList[counter].ProcName, temp_ref.c_str());
                pos++;
                continue;
            }

            if (pos == 1) {
                procList[counter].priority = stoi(temp_ref);
                pos++;
                continue;
            }

            if(pos == 2) {
                procList[counter].arrivalTime = stof(temp_ref);
                pos++;
                continue;
            }

            if(pos == 3) {
                procList[counter].ProcType = temp_ref[0];
                pos++;
                continue;
            }

            if(pos == 4) {
                procList[counter].cpuTime = stof(temp_ref);
                pos++;
                continue;
            }

            if(pos == 5) {
                procList[counter].ioTime = stof(temp_ref);
                pos++;
                continue;
            }
        }
    }

    srand(time(NULL));

    int flag = 0;
    for(int i = 0; i < numProcesses; i++){
        if(procList[i].cpuTime == -1.0){
            procList[i].cpuTime = (float)rand()/((float)RAND_MAX/10.0) + 1;
            continue;
        }
    }


    for(int i = 0; i < numProcesses; i++){
        if(procList[i].ioTime == -2.0){
            continue;
        }
        flag = 1;
        break;
    }

    if(flag == 0){
        for(int i = 0 ; i < numProcesses; i++){
            if(procList[i].ProcType = 'I'){
                procList[i].ioTime = (float)rand()/((float)RAND_MAX/3.0) + 1;
            }      
            else{
                procList[i].ioTime = -1.0;
            }
        }
    }
}

void initializePCB(){
    for(int i = 0; i < numProcesses; i++){
        procList[i].arrivalTime = 0.0;
        procList[i].cpuTime = -1.0;
        procList[i].priority = 0;
        procList[i].ioTime = -2.0;
        procList[i].ProcID = -1;
        procList[i].ProcType = 'C';
        procList[i].runTime = 0.0;
    }

    string name = "(IDLE)";
    idle.ProcName = (char*)malloc(sizeof(name));
    strcpy(idle.ProcName, name.c_str());
    idle.cpuTime = 0.1;
    idle.priority = 0;
    idle.ioTime = -1.0;
    idle.ProcID - -1;
    idle.ProcType = 'C';
}

void showProcesses(){
    printf("========================================================================================================================\n\n");
    printf("                                                Process List\n\n========================================================================================================================\n\n");
    printf("Process ID\tProcess Name\tPriority\tArrival Time\tType\tCPU Time\tIO Time\n\n");
    for(int i = 0; i < numProcesses; i++){
        cout << setprecision(2) << procList[i].ProcID <<"              " << procList[i].ProcName << setw(20 - strlen(procList[i].ProcName)) << procList[i].priority << "                "  << procList[i].arrivalTime << "          "<<procList[i].ProcType << "           "<< procList[i].cpuTime << "               ";
        if(procList[i].cpuTime > 10){
            cout << procList[i].ioTime << endl;
        }
        else{
            cout << " " << procList[i].ioTime << endl;
        }
    }
    printf("\n\n========================================================================================================================\n\n");
    cout << endl;
    
}
void InitProcesses(){
    countProcesses();
    procList = new PCB[10];
    initializePCB();
    storeProcesses();
}

#endif