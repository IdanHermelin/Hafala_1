#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <ctime>
#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class Command {
// TODO: Add your data members

 public:
  Command(const char* cmd_line);
  virtual ~Command();
  virtual void execute() = 0;
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line);
  virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {


   std::string cmd_line;
 public:
  ExternalCommand(const char* cmd_line);
  virtual ~ExternalCommand() {}
  void execute() override;
  bool isComplex();



};



class PipeCommand : public Command {
  // TODO: Add your data members
  std::string writeCommand;
  std::string readCommand;
  std::string sign;
 public:
  PipeCommand(const char* cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
    std::string redirectSign;
    std::string destFile;
    char* command;
 public:
  explicit RedirectionCommand(const char* cmd_line);
  virtual ~RedirectionCommand() {}
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
public:
  ChangeDirCommand(const char *cmd_line);
  virtual ~ChangeDirCommand() {}
  void execute() override;


private:
    std::string requestedDir;


};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line);
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};

class changePromptCommand : public BuiltInCommand{
public:
    changePromptCommand(const char* cmd_line);
    virtual ~changePromptCommand(){}
    void execute() override;

private:
    std::string plastPrompt;
};


class JobsList;
class QuitCommand : public BuiltInCommand {
// TODO: Add your data members
    bool isSpecified;
    std::string cmdLine;
public:
  QuitCommand(const char* cmd_line, JobsList* jobs);
  virtual ~QuitCommand() {}
  void execute() override;

};


class JobsList {
 public:

  class JobEntry {
   // TODO: Add your data members
  public:
   int job_index; // job-id
   bool isStopped;
   std::time_t entryTime;
   std::string cmd_line;
   pid_t job_pid;
   JobEntry();
   JobEntry(time_t entry_time,std::string cmd_line,pid_t job_pid);

  };
 // TODO: Add your data members
 public:
  JobsList();
  ~JobsList();
  void addJob(JobEntry* jobToAdd);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  std::vector<JobEntry>* getVec();
  static std::vector<JobEntry>* vectorOfJobs;
  static int max_index;
  friend class ExternalCommand;


  // TODO: Add extra methods or modify exisitng ones as needed
private:

};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  JobsCommand(const char* cmd_line, JobsList* jobs);
  virtual ~JobsCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 pid_t plastJobId;
 bool isPlastJobExist;
 std::string cmd_line;


 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members

    pid_t plastJobId;
    bool isPlastJobExist;
    std::string cmd_line;

 public:
  BackgroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~BackgroundCommand() {}
  void execute() override;
};

class TimeoutCommand : public BuiltInCommand {
/* Bonus */
// TODO: Add your data members
 public:
  explicit TimeoutCommand(const char* cmd_line);
  virtual ~TimeoutCommand() {}
  void execute() override;
};

class ChmodCommand : public BuiltInCommand {
    char* args[21];
    int newMode;
    std::string pathToFile;
  // TODO: Add your data members
 public:
  ChmodCommand(const char* cmd_line);
  virtual ~ChmodCommand() {}
  void execute() override;
};

class GetFileTypeCommand : public BuiltInCommand {
    std::string pathToFile;
  // TODO: Add your data members
 public:
  GetFileTypeCommand(const char* cmd_line);
  virtual ~GetFileTypeCommand() {}
  void execute() override;
};

class SetcoreCommand : public BuiltInCommand {
  // TODO: Add your data members
  char* args[21];
  int jobId;
  int coreToSet;
 public:
  SetcoreCommand(const char* cmd_line);
  virtual ~SetcoreCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members

    std::string cmd_line;
    int sigNum;
    int job_id;
 public:
  KillCommand(const char* cmd_line, JobsList* jobs);
  virtual ~KillCommand() {}
  void execute() override;
};

class SmallShell {
 private:
  // TODO: Add your data members
  SmallShell();
 public:
  Command *CreateCommand(const char* cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell();
    static bool isChpromptNeeded;
    static std::string toChangePrompt;
    static char* lastWorkingDirectory;
    static bool isLastDirectoryExist;
    static JobsList* listOfJobs;
    static bool toQuit;





    void executeCommand(const char* cmd_line);
  // TODO: add extra methods as needed
};





#endif //SMASH_COMMAND_H_
