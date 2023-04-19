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

SmallShell::SmallShell() {
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

  if (firstWord.compare("pwd") == 0) {
    return new GetCurrDirCommand(cmd_line);
  }
  else if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  else if (firstWord.compare("cd") == 0) {

      return new ChangeDirCommand(cmd_line);
  }
  else if (firstWord.compare("chprompt") == 0){
      return new changePromptCommand(cmd_line);
  }
  else if (firstWord.compare("jobs") == 0){
      return new JobsCommand(cmd_line,SmallShell::listOfJobs);
  }
  else if(firstWord.compare("fg")==0){
      return new ForegroundCommand(cmd_line,SmallShell::listOfJobs);
  }
  else if(firstWord.compare("bg") == 0){
      return new BackgroundCommand(cmd_line,SmallShell::listOfJobs);
  }
  else if (firstWord.compare("quit")){
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

ExternalCommand::ExternalCommand(const char *cmd_line) : Command(cmd_line)
{
    this->cmd_line = cmd_line;
}


void ExternalCommand::execute()
{
    pid_t pid = fork();
    if(pid == 0){
        string ScanCommandLine = this->cmd_line;
        bool isCommandLineOver = false;
        char* args[21];
        int index = 0;


        while(isCommandLineOver == false){
            size_t next = ScanCommandLine.find_first_of(" ");
            string toPush = ScanCommandLine.substr(0,next);
            string restCommandLine = ScanCommandLine.substr(next);
            const char* to_Push = toPush.c_str();
            args[index] = new char[toPush.length()+1];
            strcpy(args[index],to_Push);
            index++;
            size_t check = restCommandLine.find_first_not_of(" ");
            if(check == string::npos){
                isCommandLineOver = true;
            }
        }


    }

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
JobsCommand::JobsCommand(const char *cmd_line, JobsList *jobs): BuiltInCommand(cmd_line)
{
}

void JobsCommand::execute() {
    vector<JobsList::JobEntry> myListOfJobs = *SmallShell::listOfJobs->getVec();
    for (int i = 0;i < myListOfJobs.size();i++){
        cout << "[" << myListOfJobs[i].job_index <<"] " << myListOfJobs[i].cmd_line;
        cout << " : " << myListOfJobs[i].job_pid <<difftime(myListOfJobs[i].entryTime, time(nullptr))<< endl;
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

        SmallShell::listOfJobs->removeJobById(this->plastJobId);
        cout << this->cmd_line <<": " << plastPid << endl;
//        kill(plastPid,SIGCONT);
//        int status;
//        waitpid(plastPid, &status, 0);
    }
}
void JobsList::removeJobById(int jobId) {

    for (int i=0;i<this->vectorOfJobs->size();++i){
        if(this->vectorOfJobs[i].data()->job_index == jobId){
            this->vectorOfJobs->erase(vectorOfJobs->begin()+i);
            if (this->max_index == i){
                this->max_index = vectorOfJobs[i].data()->job_index;
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
    //checking other things:
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

    }

    else {
        for (int i = 0; i < SmallShell::listOfJobs->vectorOfJobs->size(); i++) {
            if (SmallShell::listOfJobs->vectorOfJobs[i].data()->isStopped == true) {
                lastStoppedJobId = SmallShell::listOfJobs->vectorOfJobs[i].data()->job_index;
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







