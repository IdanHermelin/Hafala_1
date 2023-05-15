#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"



int main(int argc, char* argv[]) {
    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }

    //TODO: setup sig alarm handler
    /*
    struct sigaction sigAction;
    sigAction.sa_handler= alarm_handler;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = SA_RESTART;
    if (sigaction(SIGALRM, &sigAction, NULL) == -1) {
        perror("smash error: failed to set sigalarm handler");
    }
    */



    SmallShell& smash = SmallShell::getInstance();
    while(true&&SmallShell::toQuit== false) {
        if(SmallShell::isChpromptNeeded == true){
            std::cout << SmallShell::toChangePrompt << "> " ;
        }
        else{
            std::cout << "smash> ";
        }
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        smash.executeCommand(cmd_line.c_str());
    }
    return 0;
}
