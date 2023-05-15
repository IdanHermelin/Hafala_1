#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <sys/stat.h>
#include <iomanip>
#include "Commands.h"
#include "signals.h"
#include <signal.h>
#include <ctime>
#include <fstream>
#include <fcntl.h>
#include <sched.h>


using namespace std;


const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

void fillArgsArray(const char* cmdLine,char* args[21]);
string _ltrim(const std::string& s) // returns the string without any initial spaces
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);

}

string _trim(const std::string& s)
{
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(string(cmd_line)).c_str());
    for(std::string s; iss >> s; ) {
        args[i] = (char*)malloc(s.length()+1);
        memset(args[i], 0, s.length()+1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    return i;

    FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
    const string str(cmd_line);
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    // if all characters are spaces then return
    if (idx == string::npos) {
        return;
    }
    // if the command line does not end with & then return
    if (cmd_line[idx] != '&'){
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = ' ';
    // truncate the command line string up to the last non-space character
    cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h
class JobsList;
SmallShell::SmallShell() {

    SmallShell::listOfJobs = new JobsList();
// TODO: add your implementation
}

SmallShell::~SmallShell() {
// TODO: add your implementation
}
Command::Command(const char *cmd_line)
{

}
Command::~Command() noexcept {

}
/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
    // For example:

    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));


    if (cmd_s.find(">") != string::npos||cmd_s.find(">>") != string::npos){

        return new RedirectionCommand(cmd_line);

    }
    else if(cmd_s.find("|") != string::npos){
        return new PipeCommand(cmd_line);
    }


    else if (firstWord.compare("pwd") == 0) {
        if (_isBackgroundComamnd(cmd_line)){
            char* toSend = const_cast<char*>(cmd_line);
            _removeBackgroundSign(toSend);
            return new GetCurrDirCommand(toSend);
        }
        return new GetCurrDirCommand(cmd_line);
    }
    else if (firstWord.compare("showpid") == 0) {
        if (_isBackgroundComamnd(cmd_line)){
            char* toSend = const_cast<char*>(cmd_line);
            _removeBackgroundSign(toSend);
            return new ShowPidCommand(toSend);
        }
        return new ShowPidCommand(cmd_line);
    }
    else if (firstWord.compare("cd") == 0) {
        if (_isBackgroundComamnd(cmd_line)){
            char* toSend = const_cast<char*>(cmd_line);
            _removeBackgroundSign(toSend);
            return new ChangeDirCommand(toSend);
        }
        return new ChangeDirCommand(cmd_line);
    }
    else if (firstWord.compare("chprompt") == 0){
        if (_isBackgroundComamnd(cmd_line)){
            char* toSend = const_cast<char*>(cmd_line);
            _removeBackgroundSign(toSend);
            return new changePromptCommand(toSend);
        }
        return new changePromptCommand(cmd_line);
    }
    if(firstWord.compare("kill") == 0){


        return new KillCommand(cmd_line,SmallShell::listOfJobs);
    }


    else if (firstWord.compare("jobs") == 0){\
        if (_isBackgroundComamnd(cmd_line)){
            char* toSend = const_cast<char*>(cmd_line);
            _removeBackgroundSign(toSend);
            return new JobsCommand(toSend,SmallShell::listOfJobs);
        }
        return new JobsCommand(cmd_line,SmallShell::listOfJobs);
    }
    else if(firstWord.compare("fg")==0){
        if (_isBackgroundComamnd(cmd_line)){
            char* toSend = const_cast<char*>(cmd_line);
            _removeBackgroundSign(toSend);
            return new ForegroundCommand(toSend,SmallShell::listOfJobs);
        }
        return new ForegroundCommand(cmd_line,SmallShell::listOfJobs);
    }
    else if(firstWord.compare("bg") == 0){
        if (_isBackgroundComamnd(cmd_line)){
            char* toSend = const_cast<char*>(cmd_line);
            _removeBackgroundSign(toSend);
            return new BackgroundCommand(toSend,SmallShell::listOfJobs);
        }
        return new BackgroundCommand(cmd_line,SmallShell::listOfJobs);
    }
    else if (firstWord.compare("quit") == 0){
        if (_isBackgroundComamnd(cmd_line)){
            char* toSend = const_cast<char*>(cmd_line);
            _removeBackgroundSign(toSend);
            SmallShell::toQuit = true;
            return new QuitCommand(toSend,SmallShell::listOfJobs);
        }
        SmallShell::toQuit = true;
        return new QuitCommand(cmd_line,SmallShell::listOfJobs);
    }
    else if(firstWord.compare("getfileinfo") == 0){
        return new GetFileTypeCommand(cmd_line);
    }
    else if(firstWord.compare("chmod") == 0){
        return new ChmodCommand(cmd_line);
    }
    else if(firstWord.compare("timeout") == 0){
        return new TimeoutCommand(cmd_line);
    }




    else {

        return new ExternalCommand(cmd_line);
    }


//  else {
//    return new ExternalCommand(cmd_line);
//  }

    return nullptr;
}
void fillArgsArray(const char* cmdLine,char* args[21]){
    string ScanCommandLine = cmdLine;
    bool isCommandLineOver = false;
    int index = 0;


    while (isCommandLineOver == false) {
        size_t next = ScanCommandLine.find_first_of(WHITESPACE);
        if (next == string::npos) {
            isCommandLineOver = true;
        }
        string toPush, restCommandLine;
        if (isCommandLineOver == false) {
            toPush = ScanCommandLine.substr(0, next);
        } else {
            toPush = ScanCommandLine;
        }


        if (isCommandLineOver == false) {
            restCommandLine = ScanCommandLine.substr(next + 1);
        }
        const char *to_Push = toPush.c_str();
        args[index] = new char[toPush.length() + 1];
        strcpy(args[index], to_Push);
        index++;

        if (isCommandLineOver == false) {
            ScanCommandLine = restCommandLine;
        }

    }
    args[index++] = nullptr;
}

ChmodCommand::ChmodCommand(const char *cmd_line): BuiltInCommand(cmd_line)
{
    this->cmd_line = cmd_line;

}

void ChmodCommand::execute() {
    if (_parseCommandLine(this->cmd_line.c_str(),this->args)!=3){
        cerr << "smash error: chmod: invalid aruments" << endl;
        return;
    }
    try {
        this->newMode = stoi(this->args[1]);
    }
    catch(const std::invalid_argument& e){
        cerr << "smash error: chmod: invalid aruments" << endl;
        return;
    }

    this->pathToFile = this->args[2];

    int check = chmod(this->pathToFile.c_str(),this->newMode);
    if (check != 0){
        cerr << "smash error: chmod: invalid aruments" << endl;
    }

}

SetcoreCommand::SetcoreCommand(const char *cmd_line): BuiltInCommand(cmd_line)
{
    int result = _parseCommandLine(cmd_line,this->args);
    if (result != 3){
        cerr<< "smash error: setcore: invalid arguments" << endl;
    }
    try {
        this->jobId = stoi(this->args[1]);
    }
    catch(const std::invalid_argument& e){
        cerr << "smash error: setcore: invalid aruments" << endl;
    }
    try {
        this->coreToSet = stoi(this->args[2]);
    }
    catch(const std::invalid_argument& e){
        cerr << "smash error: setcore: invalid aruments" << endl;
    }
}

void SetcoreCommand::execute()
{
    JobsList::JobEntry* jobToSetCore = SmallShell::listOfJobs->getJobById(this->jobId);
    if(jobToSetCore == nullptr){
        cerr << "smash error: setcore: job-id <job-id> does not exist" << endl;
    }

    cpu_set_t cpuToSet;
    CPU_ZERO(&cpuToSet);
    CPU_SET(this->coreToSet, &cpuToSet);
    int result = sched_setaffinity(jobToSetCore->job_pid, sizeof(cpu_set_t), &cpuToSet);
    if (result != 0){
        cerr << "smash error: setcore: invalid core number" << endl;

    }
}



GetFileTypeCommand::GetFileTypeCommand(const char *cmd_line): BuiltInCommand(cmd_line)
{
    string cmd_s = _trim(cmd_line);
    size_t check = cmd_s.find_first_of(WHITESPACE);
    this->pathToFile = _trim(cmd_s.substr(check));

}

void GetFileTypeCommand::execute() {
    struct stat status;
    const char* filename = this->pathToFile.c_str();
    FILE *file = fopen(filename,"rb");
    if (file == nullptr){
        cerr << "smash error: gettype: invalid aruments" << endl;
    }
    else{
        if(stat(filename, &status) != 0){
            cerr << "smash error: gettype: invalid aruments" << endl;
        }
        fseek(file, 0, SEEK_END);
        long size = ftell(file);

        if (S_ISREG(status.st_mode)) {
            //temp.txt’s type is “regular file” and takes up 420 bytes
            cout << this->pathToFile << " type is “regular file” and takes up " << size<< " bytes"<<endl;
        } else if (S_ISDIR(status.st_mode)) {
            cout << this->pathToFile << " type is “directory” and takes up " << size<< " bytes" << endl;
        } else if (S_ISCHR(status.st_mode)) {
            cout << this->pathToFile << " type is “character device” and takes up " << size<< " bytes" << endl;
        } else if (S_ISBLK(status.st_mode)) {
            cout << this->pathToFile << " type is “block device” and takes up " << size<< " bytes" << endl;
        } else if (S_ISFIFO(status.st_mode)) {
            cout << this->pathToFile << " type is “FIFO” and takes up " << size << " bytes" << endl;
        }
        else if (S_ISLNK(status.st_mode)) {
            cout << this->pathToFile << " type is “symbolic link” and takes up " << size<< " bytes" << endl;
        }
        else if (S_ISSOCK(status.st_mode)) {
            cout << this->pathToFile << " type is “socket” and takes up " << size<< " bytes" << endl;
        }
    }
}

RedirectionCommand::RedirectionCommand(const char *cmd_line) : Command(cmd_line)
{
    char *args[21];
    _parseCommandLine(cmd_line,args);
    string cmd_s = _trim(cmd_line);
    bool flag = false;
    if(cmd_s.find(">>")!= string::npos){
        flag = true;
        size_t index = cmd_s.find_first_of(">>");
        cmd_s.insert(index,1,' ');
        cmd_s.insert(index+3,1,' ');
    }
    if(flag == false && cmd_s.find('>')!=string::npos){
        size_t index = cmd_s.find_first_of(">");
        cmd_s.insert(index,1,' ');
        cmd_s.insert(index+2,1,' ');
    }

    _parseCommandLine(cmd_s.c_str(),args);
    this->fullCommand = new char[strlen(cmd_line)+1];
    strcpy(this->fullCommand,cmd_line);
    int index = 0;
    string beforeSign;
    while(strchr(args[index],'>') == nullptr && strstr(args[index],">>") == nullptr){
        beforeSign.append(args[index]);
        beforeSign.append(" ");
        index++;
    }
    this->command = new char[beforeSign.length()+1];
    strcpy(this->command,beforeSign.c_str());
    this->redirectSign = args[index];

    index++;
    if(args[index]!= nullptr){
        this->destFile = args[index];
    }
    this->command = new char[beforeSign.length()+1];
    strcpy(this->command, beforeSign.c_str());



}

void RedirectionCommand::execute() {


    string cmd_s = _trim(this->command);
    ExternalCommand* cmd = new ExternalCommand(this->command);



    if(this->redirectSign == ">"){

        int original_stdout_fd = dup(STDOUT_FILENO);
        if(original_stdout_fd <0){
            perror("smash error: dup failed");
        }
        ofstream file;
        file.open(this->destFile, std::ios::trunc);

        if(!file.is_open()){
            perror("smash error: open failed");
            return;
        }
        if(cmd_s.compare("showpid") == 0){
            file << "smash pid is " << getpid() << endl;
            return;
        }
        if (cmd_s.compare("pwd") == 0) {

            char workingDirectory[1024];
            getcwd(workingDirectory, sizeof(workingDirectory));
            file << workingDirectory << endl;
            return;
        }
        if (cmd_s.compare("jobs") == 0){

            vector<JobsList::JobEntry>* myVec =  SmallShell::listOfJobs->vectorOfJobs;
            for (int i = 0;i < myVec->size();i++){
                int job_index = (*myVec)[i].job_index;
                string cmdLine = (*myVec)[i].cmd_line;
                pid_t pid = (*myVec)[i].job_pid;
                file << "[" << job_index <<"] " << cmdLine;
                file << " : " << pid << " ";
                file << difftime(time(nullptr),(*myVec)[i].entryTime) << " secs";
                if ((*myVec)[i].isStopped == true){
                    file << " (stopped)";
                }
                file << endl;
            }
            return;
        }

        string firstWord = cmd_s.substr(0,cmd_s.find_first_of(WHITESPACE));
        if(firstWord.compare("kill") == 0){
            KillCommand* killCmd = new KillCommand(this->command,SmallShell::listOfJobs);
            if (killCmd->getIsValid() == true){
                system(this->fullCommand);
                return;
            }
            return;
        }
        system(this->fullCommand);

    }
    if (this->redirectSign == ">>"){

        int original_stdout_fd = dup(STDOUT_FILENO);
        if(original_stdout_fd <0){
            perror("smash error: dup failed");
        }
        //ofstream file(this->destFile, std::ios::app);
        ofstream file;
        file.open(this->destFile, std::ios::app);
        if(!file.is_open()){
            perror("smash error: open failed");
            return;
        }
        //chmod(this->destFile.c_str(), 0655);


        if(cmd_s.compare("showpid") == 0){
            file << "smash pid is " << getpid() << endl;
            return;
        }
        if (cmd_s.compare("pwd") == 0) {

            char workingDirectory[1024];
            getcwd(workingDirectory, sizeof(workingDirectory));
            file << workingDirectory << endl;
            return;
        }
        if (cmd_s.compare("jobs") == 0){
            vector<JobsList::JobEntry>* myVec =  SmallShell::listOfJobs->vectorOfJobs;
            for (int i = 0;i < myVec->size();i++){
                int job_index = (*myVec)[i].job_index;
                string cmdLine = (*myVec)[i].cmd_line;
                pid_t pid = (*myVec)[i].job_pid;
                file << "[" << job_index <<"] " << cmdLine;
                file << " : " << pid << " ";
                file << difftime(time(nullptr),(*myVec)[i].entryTime) << " secs";
                if ((*myVec)[i].isStopped == true){
                    file << " (stopped)";
                }
                file << endl;
            }
            return;
        }

        system(this->fullCommand);



    }
}



PipeCommand::PipeCommand(const char *cmd_line): Command(cmd_line)
{
    char* args[21];
    _parseCommandLine(cmd_line,args);
    int index = 0;
    string beforeSign;
    string afterSign;
    while(strcmp(args[index],"|") != 0 && strcmp(args[index],"|&")!=0){
        this->writeCommand.append(args[index]);
        this->writeCommand.append(" ");
        index++;
    }
    this->writeCommand = _trim(this->writeCommand);
    this->sign = args[index];
    index++;
    while(args[index]!= nullptr){
        this->readCommand.append(args[index]);
        this->readCommand.append(" ");
        index++;
    }
    this->readCommand = _trim(this->readCommand);


}


void PipeCommand::execute() {
    int fd[2];
    int result = pipe(fd);
    if(result < 0){
        perror("smash error: pipe failed");
    }
    ExternalCommand* readCmd = new ExternalCommand(this->readCommand.c_str());
    ExternalCommand* writeCmd = new ExternalCommand(this->writeCommand.c_str());

    if (this->writeCommand.compare("showpid") == 0 || this->writeCommand.compare("pwd") == 0) {
        int original_stdin_fd = dup(STDIN_FILENO);
        if(original_stdin_fd <0){
            perror("smash error: dup failed");
        }
        int result;
        pid_t pid1 = fork();
        if (pid1 == 0){
            setpgrp();
            close(fd[0]);
            if (this->sign.compare("|") == 0) {
                result = dup2(fd[1], STDOUT_FILENO);
                if(result < 0){
                    perror("smash error: dup2 failed");
                }
            }
            else if (this->sign.compare("|&") == 0) {
                result = dup2(fd[1], STDERR_FILENO);
                if(result < 0){
                    perror("smash error: dup2 failed");
                }
            }

            if (this->writeCommand.compare("showpid") == 0) {
                cout << "smash pid is " << getpid() << endl;
            }
            if (this->writeCommand.compare("pwd") == 0) {
                char workingDirectory[1024];
                getcwd(workingDirectory, sizeof(workingDirectory));
                cout << workingDirectory << endl;
            }
            exit(0);

        }
        else{
            int status;
            waitpid(pid1,&status,WUNTRACED);
            close(fd[1]);
            result = dup2(fd[0], STDIN_FILENO);
            if(result <0){
                perror("smash error: dup2 failed");
            }
            if (this->readCommand.compare("showpid") == 0 || this->readCommand.compare("pwd") == 0){


                pid_t pid2 = fork();
                if (pid2 == 0){
                    setpgrp();
                    if (this->readCommand.compare("showpid") == 0) {
                        cout << "smash pid is " << getpid() << endl;
                    }
                    if (this->readCommand.compare("pwd") == 0) {

                        char workingDirectory[1024];
                        getcwd(workingDirectory, sizeof(workingDirectory));
                        cout <<workingDirectory << endl;
                    }
                    exit(0);
                }
                else{
                    int status;
                    waitpid(pid2, &status, WUNTRACED);
                    result = dup2(original_stdin_fd, STDIN_FILENO);
                    if(result <0){
                        perror("smash error: dup2 failed");
                    }
                    close(fd[0]);
                    close(original_stdin_fd);
                }
            }

            else {


                readCmd->execute();
                result = dup2(original_stdin_fd, STDIN_FILENO);
                if(result <0){
                    perror("smash error: dup2 failed");
                }
                close(fd[0]);
                close(original_stdin_fd);

            }

        }

    }

    else {

        pid_t pid1 = fork();
        if (pid1 == 0) {
            setpgrp();
            close(fd[0]);
            if (this->sign.compare("|") == 0) {
                result = dup2(fd[1], STDOUT_FILENO);
                if(result <0){
                    perror("smash error: dup2 failed");
                }
            } else if (this->sign.compare("|&") == 0) {
                result = dup2(fd[1], STDERR_FILENO);
                if(result <0){
                    perror("smash error: dup2 failed");
                }
            }
            close(fd[1]);
            writeCmd->execute();
            exit(0);
        }
        else{
            int original_stdin_fd = dup(STDIN_FILENO);
            if(original_stdin_fd <0){
                perror("smash error: dup failed");
            }
            int status;
            waitpid(pid1, &status, WUNTRACED);
            close(fd[1]);
            result = dup2(fd[0], STDIN_FILENO);
            if(result <0){
                perror("smash error: dup2 failed");
            }


            if (this->readCommand.compare("showpid") == 0 || this->readCommand.compare("pwd") == 0){


                pid_t pid2 = fork();
                if (pid2 == 0){
                    setpgrp();
                    if (this->readCommand.compare("showpid") == 0) {
                        cout << "smash pid is " << getpid() << endl;
                    }
                    if (this->readCommand.compare("pwd") == 0) {

                        char workingDirectory[1024];
                        getcwd(workingDirectory, sizeof(workingDirectory));
                        cout <<workingDirectory << endl;
                    }
                    exit(0);
                }
                else{
                    int status;
                    waitpid(pid2, &status, WUNTRACED);
                    result = dup2(original_stdin_fd, STDIN_FILENO);
                    if(result <0){
                        perror("smash error: dup2 failed");
                    }
                    close(fd[0]);
                    close(original_stdin_fd);
                }
            }

            else {


                readCmd->execute();
                result = dup2(original_stdin_fd, STDIN_FILENO);
                if(result <0){
                    perror("smash error: dup2 failed");
                }
                close(fd[0]);
                close(original_stdin_fd);

            }
        }
    }
}




QuitCommand::QuitCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line)
{
    this->cmdLine = _trim(cmd_line);
    string cmd_s = _trim(string(cmd_line));
    size_t middle = cmd_s.find_first_of(WHITESPACE);
    if (middle == string::npos){
        this->isSpecified = false;
        return;
    }
    string afterQuitLine = cmd_s.substr(middle);
    afterQuitLine = _ltrim(afterQuitLine);
    if(afterQuitLine.compare("kill") == 0) {
        this->isSpecified = true;
    }
    else{
        this->isSpecified = false;
    }
}


void QuitCommand::execute() {


    if (this->isSpecified == true) {
        cout << "smash: sending SIGKILL signal to " << SmallShell::listOfJobs->vectorOfJobs->size() << " jobs:" << endl;
        for (int i = 0; i < SmallShell::listOfJobs->vectorOfJobs->size(); i++) {
            cout <<(*SmallShell::listOfJobs->vectorOfJobs)[i].job_pid << ": " << (*SmallShell::listOfJobs->vectorOfJobs)[i].cmd_line << endl;
        }
        for (int i = 0; i < SmallShell::listOfJobs->vectorOfJobs->size(); i++) {
            int result = kill((*SmallShell::listOfJobs->vectorOfJobs)[i].job_pid, SIGKILL);
            if(result!=0){
                perror("smash error: kill failed");
            }
        }
    }
}

KillCommand::KillCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line)
{

    this->cmd_line = cmd_line;
    this->isValid = isArgsCorrect();



}
bool KillCommand::getIsValid(){
    return this->isValid;
}

bool KillCommand::isArgsCorrect() {


    int num_args = _parseCommandLine(this->cmd_line.c_str(), this->args);
    if(num_args<=2 || num_args>=4){ // The kill function needs to get up to 2 parameters and the kill command
        cerr << "smash error: kill: invalid arguments" << endl;
        return false;
    }


    if (this->args[1][0] != '-'){
        cerr << "smash error: kill: invalid arguments" << endl;
        return false;
    }
    try{
        this->sigNum = stoi(this->args[1]);
    }
    catch(const std::invalid_argument& e){
        cerr << "smash error: kill: invalid arguments" << endl;
        return false;
    }
    this->sigNum = -1 * this->sigNum ;
    try{
        this->job_id_to_send = stoi(this->args[2]);
    }
    catch(const std::invalid_argument& e){
        cerr << "smash error: kill: invalid arguments" << endl;
        return false;
    }

    bool found_job = false;
    vector<JobsList::JobEntry>* myVec =  SmallShell::listOfJobs->vectorOfJobs;
    for (int i = 0;i < myVec->size();i++){
        int job_index = (*myVec)[i].job_index;
        if(job_index ==this->job_id_to_send) {
            //Save the pid to send:
            this->pidToSend = (*myVec)[i].job_pid;
            found_job = true;
            break;
        }
    }
    if(found_job == false){
        cerr << "smash error: kill: job-id " << this->job_id_to_send << " does not exist" << std::endl;
        return false;
    }
    return true;

}



void KillCommand::execute() {

    if (this->isValid == false){
        return;
    }

    int result = kill(this->pidToSend,this->sigNum);
    if (result == 0) {

        std::cout << "signal number " << this->sigNum << " was sent to pid " << this->pidToSend << std::endl;
    }
    else {
        perror("smash error: kill");
    }
}


void SmallShell::executeCommand(const char *cmd_line) {
    // TODO: Add your implementation here
    SmallShell::listOfJobs->removeFinishedJobs();
    Command* cmd = CreateCommand(cmd_line);
    cmd->execute();
//  Please note that you must fork smash process for some commands (e.g., external commands....)
}


ExternalCommand::ExternalCommand(const char *cmdLine) : Command(cmdLine)
{
    this->cmd_line = cmdLine;
    //this->originalCmdLine = cmdLine;


}

bool ExternalCommand::isComplex() {
    if(this->cmd_line.find("*")!=string::npos||this->cmd_line.find("?")!=string::npos){
        return true;
    }
    return false;
}

void ExternalCommand::execute()
{


    std::time_t entry_time = time(nullptr);
    const char* cmdLineToSendConst = this->cmd_line.c_str();
    bool isBgCmd = _isBackgroundComamnd(cmdLineToSendConst);

    if (this->isComplex()){
        pid_t pid = fork();
        if (pid == 0){
            setpgrp();
            char *cmdLineToSend = const_cast<char*>(cmdLineToSendConst);
            char *args[] = {"/bin/bash","-c",cmdLineToSend, nullptr};
            execvp(args[0],args);
            exit(0);
        }
        else{
            if (!isBgCmd){ //foreground
                int status;
                JobsList::JobEntry cur_job = JobsList::JobEntry(entry_time,cmd_line,pid);
                SmallShell::ForegroundJob = &cur_job; ///to child
                waitpid(pid, &status, WUNTRACED);
                SmallShell::ForegroundJob = nullptr; ///to null
            }
            else{
                JobsList::JobEntry jobToAdd(entry_time,cmd_line,pid);
                SmallShell::listOfJobs->addJob(&jobToAdd,false);
            }
        }
    }


    if(!this->isComplex()){
        pid_t pid = fork();
        if(pid > 0){
            if (!isBgCmd){ //foreground
                int status;
                JobsList::JobEntry cur_job = JobsList::JobEntry(entry_time,cmd_line,pid);
                SmallShell::ForegroundJob = &cur_job; ///to child
                waitpid(pid, &status, WUNTRACED);
                SmallShell::ForegroundJob = nullptr; ///to null
            }
            else
            {
                setpgrp();
                JobsList::JobEntry jobToAdd(entry_time,cmd_line,pid);
                SmallShell::listOfJobs->JobsList::addJob(&jobToAdd,false);
            }
        }

        if(pid == 0) {
            char *args2[21];
            char* cmdLine = new char[this->cmd_line.length()+1];
            strcpy(cmdLine,this->cmd_line.c_str());
            _removeBackgroundSign(cmdLine);
            _parseCommandLine(cmdLine,args2);
            execvp(args2[0], args2);
            exit(0);

        }

    }
}

void JobsList::addJob(JobEntry *jobToAdd, bool isStopped) {
    if(isStopped){
        int num_before = 0, index=0;
        while((*SmallShell::listOfJobs->vectorOfJobs)[index].job_index <jobToAdd->job_index){
            num_before++;
            index++;
        }
        SmallShell::listOfJobs->vectorOfJobs->insert(this->vectorOfJobs->begin() + num_before, *jobToAdd);
        if (jobToAdd->job_index == this->max_index+1){
            this->max_index++;
        }
    }
    else{
        jobToAdd->isInJobsList = true;
        this->max_index++;
        jobToAdd->job_index = this->max_index;
        this->vectorOfJobs->insert(this->vectorOfJobs->cend(),*jobToAdd);
    }
}

JobsList::JobEntry::JobEntry(time_t entry_time, std::string cmd_line, pid_t job_pid)
{
    this->isInJobsList = false;
    this->isStopped = false;
    this->cmd_line = cmd_line;
    this->entryTime = entry_time;
    this->job_pid = job_pid;

}


JobsList::JobsList()
{
    this->max_index = 0;
    this->vectorOfJobs = new vector<JobEntry>;

}


ChangeDirCommand::ChangeDirCommand(const char *cmd_line): BuiltInCommand(cmd_line)
{
    this->isValid = true;
    char* args[21];
    int numOfArgs = _parseCommandLine(cmd_line,args);
    if (numOfArgs != 2){
        this->isValid = false;
        cerr << "smash error: cd: too many arguments" <<endl;
    }
    this->requestedDir = args[1];
}
void ChangeDirCommand::execute() {
    if(this->isValid == false){
        return;
    }

    if(this->requestedDir.compare("-")!=0){
        char cwd[1024];
        getcwd(cwd,sizeof(cwd));
        char* plastPwd = new char[this->requestedDir.length() + 1];
        strcpy(plastPwd, this->requestedDir.c_str());

        if (chdir(plastPwd)!=0) {

            perror("smash error: chdir failed");
        }
        else{

            SmallShell::lastWorkingDirectory = new char[strlen(cwd)+1];
            strcpy(SmallShell::lastWorkingDirectory,cwd);
            SmallShell::isLastDirectoryExist = true;
        }
    }
    else{
        if(!SmallShell::isLastDirectoryExist){
            cerr << ("smash error: cd: OLDPWD not set") << endl;
        }
        else{
            char cwd[1024];
            getcwd(cwd,sizeof(cwd));
            chdir(SmallShell::lastWorkingDirectory);
            SmallShell::lastWorkingDirectory = new char[strlen(cwd)+1];
            strcpy(SmallShell::lastWorkingDirectory,cwd);
            SmallShell::isLastDirectoryExist = true;
        }
    }

}
JobsCommand::JobsCommand(const char *cmd_line, JobsList *jobs): BuiltInCommand(cmd_line) {

}

void JobsCommand::execute() {

    vector<JobsList::JobEntry>* myVec =  SmallShell::listOfJobs->vectorOfJobs;
    for (int i = 0;i < myVec->size();i++){
        int job_index = (*myVec)[i].job_index;
        string cmdLine = (*myVec)[i].cmd_line;
        pid_t pid = (*myVec)[i].job_pid;
        cout << "[" << job_index <<"] " << cmdLine;
        cout << " : " << pid << " ";
        cout << difftime(time(nullptr),(*myVec)[i].entryTime) << " secs";
        if ((*myVec)[i].isStopped == true){
            cout << " (stopped)";
        }
        cout << endl;
    }
}


GetCurrDirCommand::GetCurrDirCommand(const char *cmd_line): BuiltInCommand(cmd_line)
{

}
void GetCurrDirCommand::execute() {

    char workingDirectory[1024];
    getcwd(workingDirectory, sizeof(workingDirectory));
    cout << workingDirectory << endl;
}

ShowPidCommand::ShowPidCommand(const char *cmd_line): BuiltInCommand(cmd_line)
{

}

void ShowPidCommand::execute() {
    pid_t pid = getpid();
    cout << "smash pid is " << pid << endl;
}


changePromptCommand::changePromptCommand(const char *cmd_line): BuiltInCommand(cmd_line)
{
    char *args[21];
    int numOfArgs =  _parseCommandLine(cmd_line,args);
//    string cmd_s = _trim(string(cmd_line));
//    size_t middle = cmd_s.find_first_of(WHITESPACE);
//    string afterCHLine = cmd_s.substr(middle+1);
//    afterCHLine = _ltrim(afterCHLine);
    if (numOfArgs == 1){
        this->plastPrompt = args[0];
    }
    else{
        this->plastPrompt = args[1];
    }

}
void changePromptCommand::execute()
{
    if(this->plastPrompt.compare("chprompt") == 0){
        SmallShell::isChpromptNeeded = false;
    }
    else{
        SmallShell::isChpromptNeeded = true;
        SmallShell::toChangePrompt = this->plastPrompt;
    }

}

std::vector<JobsList::JobEntry> *JobsList::getVec() {
    return this->vectorOfJobs;
}

ForegroundCommand::ForegroundCommand(const char *cmd_line, JobsList *jobs):BuiltInCommand(cmd_line)
{
    this->isValid = true;
    char* args[21];
    int numOfArgs = _parseCommandLine(cmd_line,args);
    if (numOfArgs > 2){
        cerr << "smash error: fg: invalid arguments" <<endl;
        this->isValid = false;
        return;
    }
    if (numOfArgs == 2){
        try{
            this->plastJobId =std::stoi(args[1]);
        }
        catch (const std::invalid_argument& e){
            this->isValid = false;
            cerr << "smash error: fg: invalid arguments" <<endl;
            return;
        }
        isPlastJobExist = true;
    }

    else{
        plastJobId = 0;
        isPlastJobExist = false;
        if(jobs->max_index== 0){
            this->isValid = false;
            cerr << "smash error: fg: jobs list is empty" << endl;
            return;
        }
    }
}


void ForegroundCommand::execute()
{
    if(this->isValid == false){
        return;
    }
    std::time_t entry_time = time(nullptr);
    pid_t plastPid = -1;
    vector<JobsList::JobEntry>* myVector = SmallShell::listOfJobs->vectorOfJobs;
    if (!this->isPlastJobExist && myVector->size()>0){
        this->plastJobId = SmallShell::listOfJobs->max_index;
    }
    for (int i=0;i<myVector->size();i++){
        if((*myVector)[i].job_index == this->plastJobId) {
            plastPid = (*myVector)[i].job_pid;
        }
    }
    if (plastPid == -1){
        cerr << "smash error: fg: job-id " <<this->plastJobId<< " does not exist" << endl;
    }

    else{
        string ToPrint = SmallShell::listOfJobs->getJobById(this->plastJobId)->cmd_line;
        SmallShell::listOfJobs->removeJobById(this->plastJobId);
        cout << ToPrint  <<" : " << plastPid << endl;
        int result = kill(plastPid,SIGSTOP);
        if(result!=0){
            perror("smash error: kill failed");
        }
        result = kill(plastPid,SIGCONT);
        if(result!=0){
            perror("smash error: kill failed");
        }
        JobsList::JobEntry cur_job = JobsList::JobEntry(entry_time,ToPrint,plastPid);
        cur_job.isInJobsList = true;
        cur_job.job_index = plastJobId;
        SmallShell::ForegroundJob = &cur_job; ///to child
        int status;
        waitpid(plastPid, &status, WUNTRACED);
        SmallShell::ForegroundJob = nullptr; ///to null
    }
}

void JobsList::removeFinishedJobs() {
    int status;
    for(int i=0;i<JobsList::vectorOfJobs->size();++i) {
        if(waitpid((*JobsList::vectorOfJobs)[i].job_pid, &status, WNOHANG)>0){
            SmallShell::listOfJobs->removeJobById((*JobsList::vectorOfJobs)[i].job_index);
            i--;
        }
    }
    int max=0;
    for(int i=0;i<JobsList::vectorOfJobs->size();++i){
        if((*JobsList::vectorOfJobs)[i].job_index > max){
            max=(*JobsList::vectorOfJobs)[i].job_index;
        }
    }
    JobsList::max_index = max;
}

void JobsList::removeJobById(int jobId) {

    for (int i=0;i<JobsList::vectorOfJobs->size();++i){
        if((*JobsList::vectorOfJobs)[i].job_index == jobId){
            JobsList::vectorOfJobs->erase(JobsList::vectorOfJobs->cbegin() + i);
            if (JobsList::max_index == jobId){
                if(i == 0){
                    JobsList::max_index = 0;
                }
                else{
                    JobsList::max_index = (*vectorOfJobs)[i-1].job_index;
                }
            }
        }
    }


}

JobsList::JobEntry *JobsList::getJobById(int jobId) {
    vector<JobsList::JobEntry> myVector = *JobsList::vectorOfJobs;
    for( int i=0; i<myVector.size();i++ ) {
        if(myVector[i].job_index == jobId){
            return &myVector[i];
        }
    }
    return nullptr;
}

JobsList::JobEntry *JobsList::getLastJob(int *lastJobId) {
    if(JobsList::max_index==0){
        return nullptr;
    }
    vector<JobsList::JobEntry> myVector = *JobsList::vectorOfJobs;
    return &myVector[JobsList::max_index-1];
}




BackgroundCommand::BackgroundCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line){
    //check the syntax of the command:
    this->isValid = true;
    char* args[21];
    int numOfArgs = _parseCommandLine(cmd_line,args);
    if (numOfArgs > 2){
        this->isValid = false;
        cerr << "smash error: bg: invalid arguments" <<endl;
        return;
    }
    if(numOfArgs == 2){ //there is a job-id
        try{
            this->plastJobId =std::stoi(args[1]);
        }
        catch (const std::invalid_argument& e){
            this->isValid = false;
            cerr << "smash error: bg: invalid arguments" <<endl;
            return;
        }
        isPlastJobExist = true;
    }
    else{
        plastJobId = 0;
        isPlastJobExist = false;
    }
}



void BackgroundCommand::execute(){
    if(this->isValid == false){
        return;
    }
    int lastStoppedJobId = -1;
    int index = 0;
    if (this->isPlastJobExist == true) {
        if (SmallShell::listOfJobs->getJobById(this->plastJobId) == nullptr) {

            cerr << "smash error: bg: job-id " << this->plastJobId << " does not exist" << endl;
            return;
        } else {
            while (((*SmallShell::listOfJobs->vectorOfJobs)[index].job_index < this->plastJobId)){
                index++;
            }
            bool isStp = (*SmallShell::listOfJobs->vectorOfJobs)[index].isStopped;
            if (isStp == false) {

                cerr << "smash error: bg: job-id " << this->plastJobId << " is already running in the background" << endl;
                return;
            }
        }
        (*SmallShell::listOfJobs->vectorOfJobs)[index].isStopped = false;
        cout << (*SmallShell::listOfJobs->vectorOfJobs)[index].cmd_line << " : " << (*SmallShell::listOfJobs->vectorOfJobs)[index].job_pid << endl;
        pid_t pidToSend = (*SmallShell::listOfJobs->vectorOfJobs)[index].job_pid;
        int result = kill(pidToSend,SIGCONT);
        if(result!=0){
            perror("smash error: kill failed");
        }

    }

    else {
        for (int i = 0; i < SmallShell::listOfJobs->vectorOfJobs->size(); i++) {
            if ((*SmallShell::listOfJobs->vectorOfJobs)[i].isStopped == true) {
                lastStoppedJobId = (*SmallShell::listOfJobs->vectorOfJobs)[i].job_index;
                index = i;
            }
        }
        if (lastStoppedJobId == -1) {

            cerr << "smash error: bg: there is no stopped jobs to resume" << endl;
        }
        else{
            (*SmallShell::listOfJobs->vectorOfJobs)[index].isStopped = false;
            cout << (*SmallShell::listOfJobs->vectorOfJobs)[index].cmd_line << " : " << (*SmallShell::listOfJobs->vectorOfJobs)[index].job_pid << endl;
            pid_t pidToSend = (*SmallShell::listOfJobs->vectorOfJobs)[index].job_pid;
            int result = kill(pidToSend,SIGCONT);
            if(result!=0){
                perror("smash error: kill failed");
            }

        }
    }
}









BuiltInCommand::BuiltInCommand(const char *cmd_line): Command(cmd_line)
{

}



bool SmallShell::isChpromptNeeded;
std::string SmallShell::toChangePrompt;
char* SmallShell::lastWorkingDirectory;
bool SmallShell::isLastDirectoryExist;
JobsList* SmallShell::listOfJobs;
std::vector<JobsList::JobEntry>* JobsList::vectorOfJobs;
int  JobsList::max_index=0;



bool SmallShell::toQuit;
JobsList::JobEntry* SmallShell::ForegroundJob;
TimeoutCommand* SmallShell::TimeOutJob;



TimeoutCommand::TimeoutCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {
    char* args[21];
    int numOfArgs = _parseCommandLine(cmd_line,args);
    SmallShell::TimeOutJob->duration = stoi(args[0]);
    int index=0;
    while(args[index]!=nullptr){
        this->cmd_line.append(args[index]);
        this->cmd_line.append(" ");
        index++;
    }
}

void TimeoutCommand::execute() {
    //SmallShell::TimeOutJob->cmd_line = cmd_line;
    // Command* command = SmallShell::createCommand(cmd_line.c_str());

    //   alarm(duration);

}
