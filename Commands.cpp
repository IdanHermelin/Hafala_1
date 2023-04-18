#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
//#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"

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
Command::Command(const char *cmd_line) {

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
        cout << " : " << getpid() <<difftime(myListOfJobs[i].entryTime, time(nullptr))<< endl;
    }

}


std::vector<JobsList::JobEntry> *JobsList::getVec() {
    return this->vectorOfJobs;
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


BuiltInCommand::BuiltInCommand(const char *cmd_line): Command(cmd_line)
{

}

bool SmallShell::isChpromptNeeded;
std::string SmallShell::toChangePrompt;
char* SmallShell::lastWorkingDirectory;
bool SmallShell::isLastDirectoryExist;
JobsList* SmallShell::listOfJobs;


