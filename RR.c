//
// Created by ahmedosama on 4/12/23.
//
// We will assume that the new process is put in the tail of the queue
#include "headers.h"
#include "queue2.h"
int quantum = 1 ;
int main(int argc , char ** argv){

    struct Queue2 Processes ;
    struct Queue2 Finished_Processes ;
    Finished_Processes = createQueue();
    Processes =  createQueue() ;
    while (true){
       struct Queue2 Current_Quantum_Processes ;  // A temp queue
       // Inserting all the available processes in the quantum queue
        if (!isEmpty(&Processes)) {
            struct Process Current_Process = Processes.front->data;
            dequeue(&Processes) ;
            Current_Process.remRunTime -= min(Current_Process.remRunTime , quantum);
            if(Current_Process.startingTime == -1){ // If it's the first time to get scheduled
             Current_Process.startingTime =  getClk();
            }
            if(Current_Process.remRunTime){ // If it still didn't finish
                enqueue(& Processes , Current_Process);
            }else{
                Current_Process.finishTime = getClk();
                enqueue(& Finished_Processes , Current_Process);
            }
        }
    }





}