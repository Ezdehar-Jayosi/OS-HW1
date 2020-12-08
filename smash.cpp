#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"
#include <stdlib.h>

JobsList* jobs;
std::vector<TimeOutEntry*> timeoutsList;
char* last_fg_cmd;
int GPID;
bool cm_nt_f;
char* smash_prompt;
bool alarm_flag;
bool forked;
int main(int argc, char* argv[]) {
  if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
    perror("smash error: failed to set ctrl-Z handler");
  }
  if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
    perror("smash error: failed to set ctrl-C handler");
  }
   struct sigaction signalAction;
    signalAction.sa_handler = alarmHandler;
    sigemptyset(&signalAction.sa_mask);
    signalAction.sa_flags = SA_RESTART; // SA_RESTART;
    sigaction(SIGALRM, &signalAction, NULL);
  
   jobs = new JobsList();
   smash_prompt = (char*)malloc(sizeof(char)*200);
   strcpy(smash_prompt,"smash> ");
   alarm_flag=false;
   cm_nt_f = false;
   GPID=-1;
   last_fg_cmd=(char*)malloc(sizeof(char)*280);
   SmallShell& smash = SmallShell::getInstance();
  while(true) {
     //printf("9.5\n");
    forked=false;
    
    printf("%s",smash_prompt);
      //    printf("10\n");

    std::string cmd_line;
    std::getline(std::cin, cmd_line);
        //  printf("11\n");

    smash.executeCommand(cmd_line.c_str());
  //  printf("9\n");
  }
  return 0;
}