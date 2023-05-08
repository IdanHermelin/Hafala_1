#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;
bool _isBackgroundComamnd(const char* cmd_line);
void ctrlZHandler(int sig_num) {
    // TODO: Add your implementation
    cout << "smash: got ctrl-Z" <<endl;
    if(SmallShell::ForegroundJob != nullptr) {
        bool isJob;
        if (_isBackgroundComamnd(SmallShell::ForegroundJob->cmd_line.c_str())==true || SmallShell::ForegroundJob->isInJobsList){
            isJob = true;
        }

        SmallShell::ForegroundJob->isStopped = true;
        SmallShell::listOfJobs->addJob(SmallShell::ForegroundJob, isJob);
        kill(SmallShell::ForegroundJob->job_pid,SIGSTOP);
        cout << "smash: process " << SmallShell::ForegroundJob->job_pid << " was stopped" <<endl;
        SmallShell::ForegroundJob = nullptr; //kill the process
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

