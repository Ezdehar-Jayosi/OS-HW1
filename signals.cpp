#include <iostream>
#include <signal.h>
#include "signals.h"

using namespace std;

void ctrlZHandler(int sig_num) {
	// TODO: Add your implementation
	printf("smash: got ctrl-Z\n");
    //printf("GPID IS %d , last_fg_cmd is %s",GPID,last_fg_cmd);
	if(GPID>=0 && !waitpid(GPID,NULL,WNOHANG)&&(strcmp(last_fg_cmd,"")!=0)){
			//printf("we are stopping the cmd\n");
	   jobs->addJob(last_fg_cmd,true,GPID);
	   if(kill(GPID,SIGSTOP)<0){
	      perror("smash error: kill failed");
	   }
	  printf("smash: process %d was stopped\n",GPID);
	  GPID=-1;
	  strcpy(last_fg_cmd,"");
	}
}

void ctrlCHandler(int sig_num) {
  // TODO: Add your implementation
  printf("smash: got ctrl-C\n");
	if(GPID>=0 && !waitpid(GPID,NULL,WNOHANG)&&(strcmp(last_fg_cmd,"")!=0)){
	   if(kill(GPID,SIGKILL)<0){
		   perror("smash error: kill failed");
	   }
	  printf("smash: process %d was killed\n",GPID);
	  GPID=-1;
	  strcpy(last_fg_cmd,"");
	}
}

void alarmHandler(int sign_num){
   printf("smash: got an alarm\n");
    if(timeoutsList.empty()) return;
    for (auto it = timeoutsList.begin(); it != timeoutsList.end();)
    {
        int dif = difftime(time(nullptr), (*it)->timestamp + (*it)->duration);
         if(dif >= 0){
            kill((*it)->pid, SIGKILL);
            printf("smash: timeout %s timed out!\n", (*it)->cmd);
            jobs->removeJobById((*it)->pid);
            free((*it)->cmd);
            (*it)->cmd=NULL;
            it = timeoutsList.erase(it);
            alarm_flag=false;
            if (timeoutsList.empty())
            {
                break;
            }
            if (it != timeoutsList.begin())
                it--;
            }
            else
            {
                 if(!timeoutsList.empty()) {
                     free((*it)->cmd);
                    (*it)->cmd=NULL;
                    timeoutsList.erase(it);
                 }
            }
       
          ++it;
   }
GPID=-1;
	  strcpy(last_fg_cmd,"");
}
