#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include "signals.h"
#include <signal.h>
#include <ctime>
#include <fstream>
#include <fcntl.h>
#include <experimental/filesystem>

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

    if (cmd_s.find(">")||cmd_s.find(">>")){
        return new RedirectionCommand(cmd_line);
    }
    else if(cmd_s.find("|")){
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
GetFileTypeCommand::GetFileTypeCommand(const char *cmd_line): BuiltInCommand(cmd_line)
{
    string cmd_s = _trim(cmd_line);
    size_t check = cmd_s.find_first_of(WHITESPACE);
    this->pathToFile = _trim(cmd_s.substr(check));

}

void GetFileTypeCommand::execute() {


    if (!std::experimental::filesystem::exists(this->pathToFile)) {
        std::cerr << "smash error: gettype: invalid aruments" << std::endl;
    }
    FILE* file = fopen(this->pathToFile.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);

    if (std::experimental::filesystem::is_regular_file(this->pathToFile)) {
        std::cout << this->pathToFile << "type is “regular file” and takes up " << fileSize <<" bytes"<< std::endl;
    }
    else if (std::experimental::filesystem::is_directory(this->pathToFile)) {
        std::cout << this->pathToFile << "type is “directory” and takes up " << fileSize <<" bytes"<< std::endl;
    }
    else if(std::experimental::filesystem::is_character_file(this->pathToFile))
    {
        std::cout << this->pathToFile << "type is “character device” and takes up " << fileSize<<" bytes"<< std::endl;
    }
    else if(std::experimental::filesystem::is_block_file(this->pathToFile))
    {
        std::cout << this->pathToFile << "type is “block device” and takes up " << fileSize<<" bytes"<< std::endl;
    }
    else if(std::experimental::filesystem::is_fifo(this->pathToFile))
    {
        std::cout << this->pathToFile << "type is “FIFO” and takes up " << fileSize <<" bytes"<< std::endl;
    }
//    else if()
//    {
//        std::cout << this->pathToFile << "type is “character device” and takes up " << std::filesystem::file_size(this->pathToFile)<<" bytes"<< std::endl;
//    }
    else if (std::experimental::filesystem::is_symlink(this->pathToFile))
    {
        std::cout << this->pathToFile << "type is “symbolic link” and takes up "<< fileSize << " bytes" << std::endl;
    }
}

RedirectionCommand::RedirectionCommand(const char *cmd_line) : Command(cmd_line)
{
    string cmd_s = _trim(cmd_line);
    if(cmd_s.find(">>")){
        this->redirectSign = ">>";
    }
    if(cmd_s.find(">")){
        this->redirectSign = ">";
    }
    size_t check = cmd_s.find_last_of(">");
    string afterSign = cmd_s.substr(check+1);
    string beforeSign;
    if (this->redirectSign == ">"){
        beforeSign = cmd_s.substr(0,check-1);
    }
    if (this->redirectSign == ">>"){
        string beforeSign = cmd_s.substr(0,check-2);
    }

    this->destFile = _trim(afterSign);
    string cmdBeforeSign = _trim(beforeSign);
    this->command = new char[cmdBeforeSign.length()+1];
    strcpy(this->command, cmdBeforeSign.c_str());
}

void RedirectionCommand::execute() {

    string ScanCommandLine = this->command;
    int fileDescriptor;
    if(this->redirectSign == ">"){
        fileDescriptor = open(this->destFile.c_str(),std::ios::trunc);
    }
    if(this->redirectSign == ">>"){
        fileDescriptor = open(this->destFile.c_str(),std::ios::app);
    }

    if (fork() == 0) {

        dup2(fileDescriptor, 1);
        char *args[21];
        fillArgsArray(this->command,args);
        execvp(args[0], args);
    }
}

PipeCommand::PipeCommand(const char *cmd_line): Command(cmd_line)
{
    string cmd_s = _trim(cmd_line);
    size_t check = cmd_s.find_first_of("&");
    if (check != string::npos){
        this->sign = "|&";
    }
    else {
        this->sign = "|";
        check = cmd_s.find_first_of("|");
    }
    this->writeCommand = _trim(cmd_s.substr(0,check-1));
    this->readCommand = _trim(cmd_s.substr(check+1));
}

void PipeCommand::execute() {
    int fd[2];
    pipe(fd);
    if(fork() == 0){
        if(this->sign.compare("|")){
            dup2(fd[1],STDOUT_FILENO);
        }
        if(this->sign.compare("|&")){
            dup2(fd[1],STDERR_FILENO);
        }
        close(fd[0]);
        close(fd[1]);
        char* args[21];
        fillArgsArray(this->writeCommand.c_str(),args);
        execvp(args[0],args);
    }
    if(fork() == 0){
        dup2(fd[0],STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        char *args2[21];
        fillArgsArray(this->readCommand.c_str(),args2);
        execvp(args2[0],args2);
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
            cout <<(*SmallShell::listOfJobs->vectorOfJobs)[i].job_pid << " : " << (*SmallShell::listOfJobs->vectorOfJobs)[i].cmd_line << endl;
        }
        for (int i = 0; i < SmallShell::listOfJobs->vectorOfJobs->size(); i++) {
            int result = kill((*SmallShell::listOfJobs->vectorOfJobs)[i].job_pid, SIGKILL);
        }
    }
}

KillCommand::KillCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line)
{
    this->cmd_line = _trim(cmd_line);
    size_t check = this->cmd_line.find_first_of(WHITESPACE);
    if (check == string::npos){
        cerr << "smash error: kill: invalid arguments" << endl;
    }
    string lineAfterKill = this->cmd_line.substr(check);
    check = lineAfterKill.find_first_of(WHITESPACE);
    if (check == string::npos){
        cerr << "smash error: kill: invalid arguments" << endl;
    }
    this->sigNum = std::stoi(lineAfterKill);
    lineAfterKill = lineAfterKill.substr(check);
    check = lineAfterKill.find_first_of(WHITESPACE);
    if (check == string::npos){
        cerr << "smash error: kill: invalid arguments" << endl;
    }
    this->job_id = std::stoi(lineAfterKill);
    check = lineAfterKill.find_first_not_of(WHITESPACE);
    if (check != string::npos){
        cerr << "smash error: kill: invalid arguments" << endl;
    }

}

void KillCommand::execute() {

}

void SmallShell::executeCommand(const char *cmd_line) {
  // TODO: Add your implementation here

   Command* cmd = CreateCommand(cmd_line);
   cmd->execute();
//  Please note that you must fork smash process for some commands (e.g., external commands....)
}
//ExternalCommand::ExternalCommand(const char *cmd_line): Command(cmd_line)
//{
//
//}

ExternalCommand::ExternalCommand(const char *cmdLine) : Command(cmdLine)
{
    this->cmd_line = _trim(cmdLine);

}

bool ExternalCommand::isComplex() {
    if(this->cmd_line.find("*")||this->cmd_line.find("?")){
        return true;
    }
    return true;
}

//void ExternalCommand::execute() {
//    std::time_t entry_time = time(nullptr);
//    pid_t pid = getpid();
//    JobsList::JobEntry jobToAdd(entry_time,cmd_line,pid);
//    SmallShell::listOfJobs->JobsList::addJob(&jobToAdd);
//}

void ExternalCommand::execute()
{

    std::time_t entry_time = time(nullptr);
    const char* cmdLineToSendConst = this->cmd_line.c_str();
    bool isBgCmd = _isBackgroundComamnd(cmdLineToSendConst);

    if (this->isComplex() == true){
        pid_t pid = fork();
        if (pid == 0){

            char *cmdLineToSend = const_cast<char*>(cmdLineToSendConst);
            char *args[] = {"/bin/bash","-c",cmdLineToSend, nullptr};
            execvp(args[0],args);
        }
        else{
            if (isBgCmd == false){
                int status;
                waitpid(pid, &status, 0);
            }
            else{
                JobsList::JobEntry jobToAdd(entry_time,cmd_line,pid);
                SmallShell::listOfJobs->addJob(&jobToAdd);
            }
        }
    }

    if(this->isComplex() == false){
        pid_t pid = fork();
        if(pid == 0) {
            const char* ScanCommandLine = this->cmd_line.c_str();
            char *args[21];
            fillArgsArray(ScanCommandLine,args);
            execvp(args[0], args);

        }
        if(pid > 0){
            if (isBgCmd == false){
                int status;
                waitpid(pid, &status, 0);
            }
            else
            {
                JobsList::JobEntry jobToAdd(entry_time,cmd_line,pid);
                SmallShell::listOfJobs->JobsList::addJob(&jobToAdd);
            }
        }
    }
}

void JobsList::addJob(JobEntry *jobToAdd) {
    this->max_index++;
    jobToAdd->job_index = this->max_index;
    this->vectorOfJobs->insert(this->vectorOfJobs->cend(),*jobToAdd);
}

JobsList::JobEntry::JobEntry(time_t entry_time, std::string cmd_line, pid_t job_pid)
{
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

    string cmd_s = _trim(string(cmd_line));
    size_t middle = cmd_s.find_first_of(WHITESPACE);
    string afterCDLine = cmd_s.substr(middle+1);
    afterCDLine = _trim(afterCDLine);
    size_t isMoreThan2Argues = afterCDLine.find_first_of(" ");
    if (isMoreThan2Argues != string::npos){
        cerr << "smash error: cd: too many arguments" <<endl;
    }
    this->requestedDir = afterCDLine.substr(0, afterCDLine.find_first_of(WHITESPACE));

}
void ChangeDirCommand::execute() {

    if(this->requestedDir.compare("-")!=0){
        char cwd[1024];
        getcwd(cwd,sizeof(cwd));
        char* plastPwd = new char[this->requestedDir.length() + 1];
        strcpy(plastPwd, this->requestedDir.c_str());

        if (chdir(plastPwd)!=0) {

            perror("smash error: cd failed");
        }
        else{

            SmallShell::lastWorkingDirectory = new char[strlen(cwd)+1];
            strcpy(SmallShell::lastWorkingDirectory,cwd);
            SmallShell::isLastDirectoryExist = true;
        }
    }
    else{
        if(!SmallShell::isLastDirectoryExist){
            perror("smash error: cd: OLDPWD not set");
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
        cout << difftime(time(nullptr),(*myVec)[i].entryTime) << " secs" << endl;
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
    string cmd_s = _trim(string(cmd_line));
    size_t middle = cmd_s.find_first_of(WHITESPACE);
    string afterCHLine = cmd_s.substr(middle+1);
    afterCHLine = _ltrim(afterCHLine);
    this->plastPrompt = afterCHLine.substr(0, afterCHLine.find_first_of(" \n"));

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
    //check the syntax of the command:
    string cmd_s = _trim(string(cmd_line));
    this->cmd_line = cmd_s;
    size_t index = cmd_s.find_first_of(WHITESPACE);
    string afterFGLine = cmd_s.substr(index+1);
    afterFGLine = _trim(afterFGLine);
    size_t isMoreThan2Argues = afterFGLine.find_first_of(" ");
    if (isMoreThan2Argues != string::npos){
        cerr << "smash error: fg: invalid arguments" <<endl;
    }

    if(afterFGLine!=" "){ //there is a job-id
        plastJobId =std::stoi(afterFGLine);
        isPlastJobExist = true;
    }
    else{
        plastJobId = 0;
        isPlastJobExist = false;
        if(jobs->max_index== 0){
            cerr << "smash error: fg: jobs list is empty" << endl;
        }
    }
}


void ForegroundCommand::execute()
{
    pid_t plastPid = -1;
    vector<JobsList::JobEntry> myVector = *JobsList::vectorOfJobs;
    if (this->isPlastJobExist == false){
            plastPid = myVector[myVector.size()-1].job_pid;
    }
    for (int i=0;i<myVector.size();i++){
        if(myVector[i].job_index == this->plastJobId) {
            plastPid = myVector[i].job_pid;
        }
    }
    if (plastPid == -1){
        cerr << "smash error: fg: job-id " <<this->plastJobId<< " does not exist" << endl;
    }

    else{
        string ToPrint = SmallShell::listOfJobs->getJobById(plastPid)->cmd_line;

        SmallShell::listOfJobs->removeJobById(this->plastJobId);
        cout << ToPrint  <<" : " << plastPid << endl;
        kill(plastPid,SIGSTOP);
        kill(plastPid,SIGCONT);
        int status;
        waitpid(plastPid, &status, 0);
    }
}
void JobsList::removeJobById(int jobId) {

    for (int i=0;i<this->vectorOfJobs->size();++i){
        if((*this->vectorOfJobs)[i].job_index == jobId){
            this->vectorOfJobs->erase(this->vectorOfJobs->cbegin() + i);
            if (this->max_index == jobId){
                if(i == 0){
                    this->max_index = 0;
                }
                else{
                    this->max_index = (*vectorOfJobs)[i-1].job_index;
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
    string cmd_s = _trim(string(cmd_line));
    this->cmd_line = cmd_s;
    size_t index = cmd_s.find_first_of(WHITESPACE);
    string afterBGLine = cmd_s.substr(index+1);
    afterBGLine = _trim(afterBGLine);
    size_t isMoreThan2Argues = afterBGLine.find_first_of(" ");
    if (isMoreThan2Argues != string::npos){
        cerr << "smash error: fg: invalid arguments" <<endl;
    }
    if(afterBGLine!=" "){ //there is a job-id
        plastJobId =std::stoi(afterBGLine);
        isPlastJobExist = true;
    }
    else{
        plastJobId = 0;
        isPlastJobExist = false;
    }
///////////////need to complete this
}



void BackgroundCommand::execute(){
    int lastStoppedJobId = -1;

    if (this->isPlastJobExist == true) {
        if (SmallShell::listOfJobs->getJobById(this->plastJobId) == nullptr) {
            cerr << "smash error: bg: job-id " << this->plastJobId << " does not exist" << endl;
        } else if (SmallShell::listOfJobs->getJobById(this->plastJobId)->isStopped == false) {
            cerr << "smash error: bg: job-id " << this->plastJobId << " is already running in the background" << endl;
        }
        SmallShell::listOfJobs->getJobById(this->plastJobId)->isStopped = false;
        cout << this->cmd_line << " : " << this->plastJobId << endl;
        pid_t pidToSend = SmallShell::listOfJobs->getJobById(this->plastJobId)->job_pid;
        int result = kill(pidToSend,SIGCONT);

    }

    else {
        for (int i = 0; i < SmallShell::listOfJobs->vectorOfJobs->size(); i++) {
            if ((*SmallShell::listOfJobs->vectorOfJobs)[i].isStopped == true) {
                lastStoppedJobId = (*SmallShell::listOfJobs->vectorOfJobs)[i].job_index;
            }
        }
        if (lastStoppedJobId == -1) {
            cerr << "smash error: bg: there is no stopped jobs to resume" << endl;
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






