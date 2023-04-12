//
// Created by ahmedosama on 4/12/23.
//
// We will assume that the new process is put in the tail of the queue
#include "headers.h"
#include "queue2.h"
int quantum = 1 ;
int main(int argc , char ** argv){

    struct Queue2 Processes ;
    Processes =  createQueue() ;
    while (true){
       struct Queue2 Current_Quantum_Processes ;  // A temp queue
       // Inserting all the available processes in the quantum queue
        while (!isEmpty(&Processes)) {
            struct Process Current_Process = Processes.front->data;
            dequeue(&Processes) ;
            Current_Process.remRunTime -= min(Current_Process.remRunTime , quantum);
            if(Current_Process.remRunTime){
                enqueue(& Processes , Current_Process);
            }
        }
        while (!isEmpty(&Current_Quantum_Processes)){
            dequeue(&Current_Quantum_Processes);
            Current_Process.remRunTime -=


        }
    }





}