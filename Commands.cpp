#include <unistd.h>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"


using namespace std;

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif
/*
int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  stringstream check1(cmd_line);
  string intermediate;
  int i = 0;
  while(getline(check1, intermediate, ' '))
  {
    args[i] = (char*)malloc(intermediate.length()+1);
    memset(args[i], 0, intermediate.length()+1);
    strcpy(args[i], intermediate.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
  const string whitespace = " \t\n";
  const string str(cmd_line);
  return str[str.find_last_not_of(whitespace)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
  const string whitespace = " \t\n";
  const string str(cmd_line);
  // find last character other than spaces
  size_t idx = str.find_last_not_of(whitespace);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(whitespace, idx-1) + 1] = 0;
}
*/
// TODO: Add your implementation for classes in Commands.h 

SmallShell::SmallShell() {
// TODO: add your implementation
  //HistoryLog = new CommandsHistory();
   plastPwd=(char*)malloc(sizeof(char)*200);
   strcpy(plastPwd,"");
}

SmallShell::~SmallShell() {
// TODO: add your implementation
delete HistoryLog;
free(plastPwd);

for (auto it = timeoutsList.begin(); it != timeoutsList.end(); ++it) //NEW
  {
      free((*it)->cmd);
      (*it)->cmd=NULL;
    TimeOutEntry* timeout_entry = *it;
    delete timeout_entry;
  }  
 
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
	// For example:
	char* delimiters = " \t\n";   
	char* cmd_line_cpy= (char*)malloc(sizeof(char)*(strlen(cmd_line)+1));
    strcpy(cmd_line_cpy,cmd_line);
    char* cmd = strtok(cmd_line_cpy, delimiters);
    if(cmd==NULL) return nullptr;
    string cmd_s = string(cmd);
    string cmd_pd = string(cmd_line);
   if (cmd_pd.find("|") < cmd_pd.size()) {//NEW COMMANDS 
        free(cmd_line_cpy);
        return new PipeCommand(cmd_line);
  }else if (cmd_pd.find(">") < cmd_pd.size()) {//NEW COMMANDS 
        free(cmd_line_cpy);
        return new RedirectionCommand(cmd_line);
    }
  else if (strcmp(cmd,"pwd") == 0) {//DONE WORKING
  free(cmd_line_cpy);
    return new GetCurrDirCommand(cmd_line);
  }
  else if(strcmp(cmd,"cd") == 0){//DONE CHECKED WORKING
  free(cmd_line_cpy);
     return new ChangeDirCommand(cmd_line,&plastPwd);
  }
 else if(strcmp(cmd,"jobs") == 0){//CHECKED WITH EXTERNAL COMMANDS THAT ARE NOT BG
  free(cmd_line_cpy);
  
  return new JobsCommand(cmd_line, jobs);
  } else if(strcmp(cmd,"kill") == 0){//INITIALLY WORKING
  free(cmd_line_cpy);
  
     return new  KillCommand(cmd_line, jobs);
  }else if(strcmp(cmd,"showpid") == 0){//DONE WORKING
  free(cmd_line_cpy);
  
    return new  ShowPidCommand(cmd_line);
  }else if(strcmp(cmd,"fg") == 0){
      free(cmd_line_cpy);
     //printf("new fg command\n");
    return new ForegroundCommand(cmd_line, jobs);
  }else if((strcmp(cmd,"bg") == 0)){
      free(cmd_line_cpy);
     return new BackgroundCommand(cmd_line, jobs);
  }else if((strcmp(cmd,"quit") == 0)){
      free(cmd_line_cpy);
     return new QuitCommand(cmd_line,jobs);
  }else if((strcmp(cmd,"cp") == 0)){
      free(cmd_line_cpy);
     return new CopyCommand(cmd_line);
  }else if (strcmp(cmd,"chprompt") == 0) {//NEW COMMANDS 
  free(cmd_line_cpy);
    return new ChpromptCommand(cmd_line);
  }else if (strcmp(cmd,"timeout") == 0) {//NEW COMMANDS 
        free(cmd_line_cpy);
    return new TimeOutCommand(cmd_line);
  }else if (strcmp(cmd,"ls") == 0) {//NEW COMMANDS 
        free(cmd_line_cpy);
    return new LSCommand(cmd_line);
  }else {
	 free(cmd_line_cpy);
     //printf("%s--------------\n",cmd_line);
    return new ExternalCommand(cmd_line,jobs);//CHECKED DONE BUT NEEDS MORE TESTING
  }
 
  return nullptr;
}

void SmallShell::executeCommand(const char *cmd_line) {

  Command* cmd = CreateCommand(cmd_line);
 // HistoryLog->addRecord(cmd_line);
  if(cmd){
  cmd->execute();
  // printf("g3\n");
   delete cmd;
 //  printf("g4\n");
  }
 // free( cmd);
  
  // Please note that you must fork smash process for some commands (e.g., external commands....)
}