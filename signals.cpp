#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
	// TODO: Add your implementation
    cout << "smash: got ctrl-Z" <<endl;
    if(SmallShell::ForegroundJob != nullptr) {
        SmallShell::ForegroundJob->isStopped = true;
        SmallShell::listOfJobs->addJob(SmallShell::ForegroundJob, SmallShell::ForegroundJob->isStopped);
        kill(SmallShell::ForegroundJob->job_pid,SIGSTOP);
        cout << "smash: process " << SmallShell::ForegroundJob->job_pid << " was stopped" <<endl;
        SmallShell::ForegroundJob = nullptr; //kill the process
        cout<< "finished ctrl-z function" <<endl;
    }
}

void ctrlCHandler(int sig_num) {
  // TODO: Add your implementation
    cout << "smash: got ctrl-C"<<endl;
    if(SmallShell::ForegroundJob != nullptr){
        kill(SmallShell::ForegroundJob->job_pid,SIGKILL);
        cout << "smash: process " << SmallShell::ForegroundJob->job_pid << " was killed" <<endl;
        SmallShell::ForegroundJob = nullptr; //kill the process
    }
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

