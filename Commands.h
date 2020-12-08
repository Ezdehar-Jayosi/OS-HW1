#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <string>
#include <list>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <iomanip>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
//#include "smash_def.h"
#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)
#define HISTORY_MAX_RECORDS (50)
#define MAX_LINE_SIZE (80)
class SmallShell;
using namespace std;
class JobsList;
extern bool forked;
extern char* last_fg_cmd;
extern int GPID;
extern bool cm_nt_f;
extern char* smash_prompt;
extern bool alarm_flag;
extern JobsList* jobs;
extern char** plastPwd;
//using std;
class Command {
// TODO: Add your data members
 char** args;
 int num_arg=0;
   char* cmd;
   char* cmd_line;
   std::string cmd_2;
   char* cmd_line2;
 public:

    Command(const char* cmd_line){
	int len = strlen(cmd_line);
	cmd = (char*)malloc(sizeof(char)*(len+1));
	cmd_line2 = (char*)malloc(sizeof(char)*(len+1));
	strcpy(this->cmd_line2,cmd_line);
	args = (char**)malloc(sizeof(char*)*COMMAND_MAX_ARGS);
	char* delimiters = " \t\n";   //WHAT SHOULD BE THE DELIMITERS " " OR " \t\n"
//	char* cmd_line_cpy= strdup(cmd_line);
    cmd = strtok(this->cmd_line2, delimiters);
    args[0] = (char*)malloc(sizeof(char)*COMMAND_ARGS_MAX_LENGTH);;
	if(cmd){ strcpy(args[0], cmd);
            cmd_2.append(args[0]);
    }
	//num_arg=0;
	for (int i = 1; i < COMMAND_MAX_ARGS; i++) {
		args[i] = (char*)malloc(sizeof(char)*COMMAND_ARGS_MAX_LENGTH);
        char* arg=strtok(NULL, delimiters);
        if(arg) strcpy(args[i],arg);
		if (arg == NULL) {
            args[i] = NULL;
        }else{
        num_arg++;
        cmd_2.append(args[i]);
       
        }
	}
  
  this->cmd_line = (char*)malloc(sizeof(char)*(len+1));
    strcpy(this->cmd_line,cmd_line);
  //  printf("cmd_2 is %s\n",cmd_2.c_str());
    //args[i]=NULL;
  //  free(cmd_line_cpy);
  //printf("num of args %d\n",num_arg);
  }
  virtual ~Command(){
     // free(cmd);
      //free(cmd_line);
	/* free(cmd);
	  free(cmd_line);
      
	  cmd=NULL;
      cmd_line=NULL;
	  for(int i=0;i<num_arg;i++){
	      free(args[i]);
		  args[i]=NULL;
	  }*/
  }
  virtual void execute() = 0;
  //virtual void prepare();
  virtual void cleanup(){
      free(cmd);
	  free(cmd_line);
      free(cmd_line2);
	  cmd=NULL;
	  for(int i=0;i<=num_arg;i++){
          if(args[i]){
	      free(args[i]);
		  args[i]=NULL;}
          
	  }
      
  }
  int getNumArgs(){
	  return num_arg;
  }
  char** getArgs(){
      return args;
  }
  char* getCmd(){
    return cmd;
  }
  char* getCmdLine(){
    return cmd_line;
  }
  string getCmd2(){
     return cmd_2;
  }
  
  void removeLast(){
	  if(num_arg >= 1){
	  if(strcmp(args[num_arg],"&")==0){
         // printf("hwe\n");
	    args[num_arg]=NULL;
	  }else{
     (args[1])[strlen(args[1])-1]='\0';
	 }
	  }else{
	  
	  (args[0])[strlen(args[0])-1]='\0';
	  }
  }
  // TODO: Add your extra methods if needed
};
class JobsList;
class TimeOutEntry{
    public:
    char* cmd;
    pid_t pid;
    time_t timestamp;
    int duration;
    bool bg=false;
};
extern std::vector<TimeOutEntry*> timeoutsList;
 class JobEntry {
	public:
   int job_Id;
   pid_t process_id;
   bool finished;
   bool stopped;
   time_t time_inserted;
   char* command;      //should we save the whole command with args? or the just the "command" itselt
   JobEntry(int jobid,pid_t process_id,bool finish,bool stop,time_t time,char* cmd){
	   this->process_id=process_id;
	   job_Id=jobid;
	   finished=finish;
	   stopped=stop;
	   time_inserted=time;
	   command=(char*)malloc(sizeof(char)*280);
	   strcpy(command,cmd);
   }
   ~JobEntry(){
      free(command);
   }
   
  };
class JobsList {
 public:
 // TODO: Add your data members 
 std::list<JobEntry*> JobList;
 std::list<JobEntry*> StoppedJobsList;
 int job_id;
 public:
  JobsList():JobList(),StoppedJobsList(){
    job_id=1;
  }
  ~JobsList(){
	  
	   for (std::list<JobEntry*>::iterator it=JobList.begin(); it != JobList.end(); ++it){
		    //delete &*it;
		   it = JobList.erase(it);
	   }
	   for (std::list<JobEntry*>::iterator it=StoppedJobsList.begin(); it != StoppedJobsList.end(); ++it){
		   //delete &*it;
		   it = StoppedJobsList.erase(it);
	    }
  }
  void addJob(Command* cmd, bool isStopped ,pid_t process_id){ 
	  time_t time_inserted=time(NULL);
      JobEntry* newJob = new JobEntry(job_id,process_id,false,isStopped,time_inserted, cmd->getCmdLine()); 
	  JobList.push_back(newJob);
	  if(isStopped){
	   JobEntry* newSJob = new JobEntry(job_id,process_id,false,isStopped,time_inserted, cmd->getCmdLine()); 
	   StoppedJobsList.push_back(newSJob);
	  }
	  job_id++;
  }void addJob(char* cmd, bool isStopped ,pid_t process_id){ 
	  time_t time_inserted=time(NULL);
	   for (std::list<JobEntry*>::iterator it=JobList.begin(); it != JobList.end(); ++it){
		if((*it)->process_id == process_id){
			if((*it)->stopped==false){
			   (*it)->stopped=true;
			   JobEntry* newSJob = new JobEntry((*it)->job_Id,process_id,false,true,time_inserted, cmd);
	            StoppedJobsList.push_back(newSJob);
			}
			 (*it)->time_inserted=time_inserted;
			 (*it)->finished=false;
			
			 return;
		 }
       }
	
      JobEntry* newJob = new JobEntry(job_id,process_id,false,isStopped,time_inserted, cmd); 
	  JobList.push_back(newJob);
	  if(isStopped){
	  JobEntry* newSJob = new JobEntry(job_id,process_id,false,isStopped,time_inserted, cmd);
	  StoppedJobsList.push_back(newSJob);
	  }
	  job_id++;
  }
  void printJobsList(){
	  //TODO CHECK IF JOB HAS TERMINATED -> DID IT , BUT NEEDS TO CHECK IF ITS WORKING RIGHT;
	  
	removeFinishedJobs();
	if(JobList.size()==0) return;
    for (std::list<JobEntry*>::iterator it=JobList.begin(); it != JobList.end(); ++it){
		time_t time_now=time(NULL);
	    if((*it)->stopped){
		   
		std::cout << "["<<(*it)->job_Id<<"]" << " " << (*it)->command << " :" <<" "<< (*it)->process_id<<" "<< ((int) difftime(time(NULL), (*it)->time_inserted)) <<" "<<"secs"<< " (stopped)"<<  std::endl;
		
		}else{
		
		std::cout << "["<<(*it)->job_Id<<"]" << " " << (*it)->command << " :" <<" "<< (*it)->process_id<<" "<<  ((int) difftime(time(NULL), (*it)->time_inserted)) <<" "<< "secs"<<  std::endl;
		
		}			
	
	}
  }
  void killAllJobs(){
	   removeFinishedJobs();
	  printf("smash: sending SIGKILL signal to %d jobs:\n",JobList.size());
	 for (std::list<JobEntry*>::iterator it=JobList.begin(); it != JobList.end(); ++it){
		 printf("%d: %s\n",(*it)->process_id,(*it)->command);
		if(kill((*it)->process_id,SIGKILL)!=0){
	  perror("smash error: kill failed");
	   return;
	 }
     }  
	  
  }
  void removeFinishedJobs(){
	  for (std::list<JobEntry*>::iterator it=JobList.begin(); it != JobList.end(); ++it){
		if(waitpid((*it)->process_id, NULL, WNOHANG)>0){ //check if deleting and iterating over a list is safe,check if the waitpid interrupt works fine
			 (*it)->finished=true;
		}	
	}
	for (std::list<JobEntry*>::iterator it=StoppedJobsList.begin(); it != StoppedJobsList.end(); ++it){
		if(waitpid((*it)->process_id, NULL, WNOHANG)>0){ //check if deleting and iterating over a list is safe,check if the waitpid interrupt works fine
			 (*it)->finished=true;
		}	
	}
    for (std::list<JobEntry*>::iterator it=JobList.begin(); it != JobList.end(); ++it){
		if((*it)->finished){ //check if deleting and iterating over a list is safe,check if the waitpid interrupt works fine
			 it=JobList.erase(it);
		}
	}		
	
    for (std::list<JobEntry*>::iterator it=StoppedJobsList.begin(); it != StoppedJobsList.end(); ++it){
		if((*it)->finished){ //check if deleting and iterating over a list is safe,check if the waitpid interrupt works fine
			 it=StoppedJobsList.erase(it);
		}
	}
     
	  job_id = (JobList.size()==0)? 1 : (JobList.back()->job_Id+1);
  }
  
  JobEntry* getJobById(int jobId){
	//  printf("getJobbyid function \n");
	if(jobId>job_id) return NULL;
   for (std::list<JobEntry*>::iterator it=JobList.begin(); it != JobList.end(); ++it){
						  //printf("looking \n");
		if((*it)->job_Id == jobId){
				//  printf("found_it \n");
			 return (*it);
		 }
	}
	return NULL;
  }
  void removeJobById(int jobId){
   for (std::list<JobEntry*>::iterator it=JobList.begin(); it != JobList.end(); ++it){
		if((*it)->job_Id == jobId){
			if((*it)->stopped){
			removeStoppedJob(jobId);
			}
			 //delete &*it;
			 JobList.erase(it);
			 job_id = (JobList.back()->job_Id+1);
			 return;
		 }
	}
  }
  JobEntry* getLastJob(int* lastJobId){
	  if(JobList.size()==0){
	    return NULL;
	  }
     *lastJobId = JobList.back()->job_Id;
	 return JobList.back();
  }
  JobEntry* getLastStoppedJob(int *jobId){
     
	  if(StoppedJobsList.size()==0){
         //  printf("here\n");
	    return NULL;
	  }
        //printf("here1\n");
     *jobId = StoppedJobsList.back()->job_Id;
      // printf("here2\n");
	 return StoppedJobsList.back();
  }
  // TODO: Add extra methods or modify exisitng ones as needed
  void removeStoppedJob(int jobId){
	  pid_t process_id;
   
	for (std::list<JobEntry*>::iterator it=JobList.begin(); it != JobList.end(); ++it){
		if((*it)->job_Id == jobId){
			(*it)->stopped=false;
			 process_id=(*it)->process_id;			
		 }
       }
 for (std::list<JobEntry*>::iterator it=StoppedJobsList.begin(); it != StoppedJobsList.end(); ++it){
		 if((*it)->process_id == process_id){
			 //delete &*it;
			 it = StoppedJobsList.erase(it);
			 return;
		 }			 
	} 	   
}

};


class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line):Command(cmd_line){}
  virtual ~BuiltInCommand(){}
 // void execute(SmallShell* smash){}
};

class ExternalCommand : public Command {
	JobsList* jobs;
	bool bg=false,comp=false;
    char* complex_command[COMMAND_MAX_ARGS];
 public:
  ExternalCommand(const char* cmd_line,JobsList* jobs):Command(cmd_line){
	  //  printf(" (1)\n");
      cm_nt_f=false;
	   this->jobs=jobs;
	   	//printf(" (2)\n");
        string str = string(cmd_line);
      // if(strstr(cmd_line,"*")|| strstr(cmd_line,"?")|| strstr(cmd_line,"_")){
        if(cmd_line[strlen(cmd_line) - 1]== '&'){
		//printf(" (5)\n");
		   //removeLast();
         //  printf("gggg\n");
	      bg=true;
	   } 

        if((str.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890& ") != std::string::npos && str.find("&")==std::string::npos)||(alarm_flag)){
        comp=true;
      //  printf(" (5)\n");
		 complex_command[0] = "/bin/bash";
		 complex_command[1] = "-c";
		 complex_command[2]=(char*)malloc(sizeof(char)*strlen(cmd_line));
        //printf("cmd_line is %s\n",cmd_line);
        strcpy(complex_command[2], cmd_line);
         if(alarm_flag && bg){
             
             complex_command[2][strlen(cmd_line)-1]=0;
         }
		 
		 complex_command[3] = NULL;
		 return;
	   }
        if(cmd_line[strlen(cmd_line) - 1]== '&'){
		//printf(" (5)\n");
		   removeLast();
         //  printf("gggg\n");
	      bg=true;
	   } 
	   if(cmd_line[strlen(cmd_line) - 1] != '&'){
		 //printf(" (6)\n");
	      strcpy(last_fg_cmd,cmd_line);
		  //printf(" (6.5)\n");
	   }
	   // printf("(cmd_line[strlen(cmd_line) - 1] is %c\n",(cmd_line[strlen(cmd_line) - 1]));
       //printf("num of args is%d\n",getNumArgs());
      //// printf(" (5m\n");

	  
  }
  virtual ~ExternalCommand() {
      free(complex_command[2]);
  }
  void execute() override{
	   int pID;
        
	  	  //printf(" (7)\n");
         
	switch (pID = fork()) {
	case -1:
		// Add your code here (error)
		perror("failed to create a process");
        cm_nt_f=true;
		exit(1);
		break;
	case 0:
		// Child Process
         ///just added NEW
       
		if(!comp){
             //forked=true;
             setpgid (0, 0);
		
			   //printf(" (8)\n");
              //printf(" getArgs()[0] is %s and getArgs()[1] is %s\n",getArgs()[0],getArgs()[1]);
		execvp(getArgs()[0], getArgs());
       // printf("1\n");
		printf("bash: %s: command not found\n",getCmd());
        cm_nt_f=true;
			// printf(" (9)\n");
		exit(5);
        break;
		}else if (comp){
		     setpgid (0, 0);
           // printf("2\n");
			execvp(complex_command[0], complex_command);
			printf("bash: %s: command not found\n",complex_command[0]);
           // printf("dddd\n");
            cm_nt_f=true;
			exit(5);
			break;
		}
		
	default:
        if(alarm_flag)
     	{
            TimeOutEntry* timeout_entry= timeoutsList.back();
            timeout_entry->pid = pID;
            timeout_entry->cmd= (char*)malloc(sizeof(char)*(strlen(getCmdLine())+1));
            strcpy(timeout_entry->cmd,getCmdLine());
            if(bg) timeout_entry->bg=true;
          //  timeoutsList.push_back(timeout_entry);
      		alarm_flag = false;
    	}
		
	    if(!bg){
			GPID = pID;
            int status;
            strcpy(last_fg_cmd,getCmdLine());
			waitpid(pID, &status, WUNTRACED);
           // printf("status 1 is %d\n",status);
            
            if ( WIFEXITED(status) ) {
                int es = WEXITSTATUS(status);
                //printf("es is %d\n",es);
                if(es >0 && forked) exit(1);
                
            }
               
			break;
		}else if (bg){
		   jobs->addJob(getCmdLine(),false,pID); 
         GPID=-1;
        strcpy(last_fg_cmd,"");
		}
		
	}
  //  printf(" external ends here\n");
    jobs->removeFinishedJobs();
  
  }
};

class CommandsHistory {
	                 //ADDED
 protected:
  class CommandHistoryEntry {
	  // TODO: Add your data members
	  public:
	  int seq_num;
	  char* cmd_line;
  };
 // TODO: Add your data members
 int num_of_logs,seq;
 char* prev_cmd;
 CommandHistoryEntry** HistoryLog; //OR BETTER USE A CONTAINER LIKE A VECTOR? 
 public:
  CommandsHistory(){
	 num_of_logs=0;
	 seq=1;
	 prev_cmd=(char*)malloc(sizeof(char)*280);
	 strcpy(prev_cmd,"");
	 HistoryLog=(CommandHistoryEntry**)malloc(sizeof(CommandHistoryEntry*)*50);
	 for(int i=0;i<50;i++){
	   HistoryLog[i] = (CommandHistoryEntry*)malloc(sizeof(CommandHistoryEntry));
	   HistoryLog[i]->seq_num=-1;
	   HistoryLog[i]->cmd_line=(char*)malloc(sizeof(char)*280);
	   
	 }
  }
  ~CommandsHistory() {
	  for(int i=0;i<50;i++){
	   free(HistoryLog[i]->cmd_line);
	   free(HistoryLog[i]);
	 }
	 free(HistoryLog);
  }
  void addRecord(const char* cmd_line){
/*if(strcmp(prev_cmd,cmd_line)!=0){
	  HistoryLog[0]->seq_num=seq;
	  strcpy(HistoryLog[0]->cmd_line,cmd_line);
	  strcpy(prev_cmd,cmd_line);
	  num_of_logs++;
	 }else{*/
	    if(strcmp(prev_cmd,cmd_line)==0){
		    HistoryLog[(num_of_logs%50)-1]->seq_num++;
		}else{
		   HistoryLog[(num_of_logs%50)]->seq_num=seq;
		   strcpy(HistoryLog[(num_of_logs%50)]->cmd_line,cmd_line);
		   strcpy(prev_cmd,cmd_line);
		   num_of_logs++;
		}
	 
	 seq++;
  }
  void printHistory(){
     int i=0,last_index;
	 if(num_of_logs>49){
	   i=(num_of_logs%50);
	 }
	 last_index=(num_of_logs<50)?num_of_logs:50;
    for(int j=i;j<last_index;j++){
	  std::cout << std::right << std::setw(5) <<HistoryLog[j]->seq_num << "  "<<HistoryLog[j]->cmd_line <<  std::endl;
	} 
	for(int j=0;j<i;j++){
	  std::cout << std::right << std::setw(5) <<  HistoryLog[j]->seq_num  <<"  "<< HistoryLog[j]->cmd_line <<  std::endl;
	}
  }
};

class HistoryCommand : public BuiltInCommand {
 // TODO: Add your data members
 CommandsHistory* historyptr;
 public:
  HistoryCommand(const char* cmd_line, CommandsHistory* history):BuiltInCommand(cmd_line){
   historyptr= history;
   ///historyptr->addRecord(cmd_line);
  }
  virtual ~HistoryCommand() {
  }
  void execute() override{
     historyptr->printHistory();
	 jobs->removeFinishedJobs();
  }
};

class SmallShell {
 private:
  // TODO: Add your data members
  CommandsHistory* HistoryLog;
  char* plastPwd;
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
  void executeCommand(const char* cmd_line);
  // TODO: add extra methods as needed
};

class PipeCommand : public Command {
  // TODO: Add your data members
  int split_index;
  bool isAmp=false;
  bool bg=false;
 public:
  PipeCommand(const char* cmd_line):Command(cmd_line){
   //  printf("1\n");
    if(cmd_line[strlen(cmd_line) - 1] == '&'){
            bg=true;
            removeLast();
      }
     //char* cmd_line_cpy= strdup(getCmdLine());
   //  printf("2\n");
     string cmd_pd = string(getCmdLine());
    // printf("3\n");
     split_index = (cmd_pd).find("|");
//printf("4\n");
    if((cmd_pd).find("|&") != std::string::npos){
        isAmp = true;
        split_index+=2; ///changed
    }
     
  //  printf("5\n");
  }
  virtual ~PipeCommand() {}
  void execute() override{
      jobs->removeFinishedJobs();

    string cmd_line = string(getCmdLine());
  //  printf("9\n");
    string cmd_1 = cmd_line.substr(0,split_index-1);
    string cmd_2 = cmd_line.substr(split_index+1,cmd_line.size()- split_index -1);
  //  printf("10\n");

 // printf("11\n");
     int pid_2 = fork();
     forked = true;
    // printf("12\n");
    if(pid_2 < 0) perror("smash error: fork failed");
    if(pid_2 == 0){
       // printf("13\n");
         setpgid(0,0);
          int fds[2];
          if (pipe(fds) == -1) {
            perror("smash error: pipe failed");
            return;
        }
      //  printf("15\n");
      //STDOUT_FILENO
          int pid_1 = fork();
        //  printf("16\n");
        
        if(pid_1 < 0) perror("smash error: fork failed");
        if(pid_1 == 0){
            setpgid(0,0);
           // printf("17\n");
            if( isAmp){
                if(dup2(fds[1],STDERR_FILENO)==-1){
                    perror("smash error: dup2 failed");
                    return;
                }
            }else{
                
              //  printf("17.5\n");
               if(dup2(fds[1],STDOUT_FILENO)==-1){
                    perror("smash error: dup2 failed");
                    return;
                } 
            }
           // printf("18\n");
           close(fds[1]);
            close(fds[0]);
            
          //  printf("19\n");
            SmallShell::getInstance().executeCommand(cmd_1.c_str());
      
            if(cm_nt_f) exit(1);
          exit(0);
    }else{  //parent
        int pid_3 = fork();
       // printf("21\n");
        if(pid_3 < 0) {
            perror("smash error: fork failed");
            return;
        }else if(pid_3==0){
            setpgid(0,0);
          //  printf("22\n");
      
      
             if(dup2(fds[0],STDIN_FILENO)==-1){
                    perror("smash error: dup2 failed");
                    return;
            }
          
            close(fds[0]);
            close(fds[1]);
             SmallShell::getInstance().executeCommand(cmd_2.c_str());
            exit(0);
        }else{
            close(fds[0]);
            close(fds[1]);
         
            int status2, status1;
           
             int res1 = waitpid(pid_1, &status1, WUNTRACED);
              if ( WIFEXITED(status1) ) {
                int es = WEXITSTATUS(status1);
                if(es==1) {
                    
                   // printf("ffff\n");
                    exit(1);
                }
            }
            int res2 = waitpid(pid_3, &status2, WUNTRACED);
          
            if (res2 == -1 || res1 == -1) {
                    perror("smash error: waitpid failed");
                    return;

            }
           exit(0);
           
           
        
        }
    }
  } else{
      
       // printf("28\n");
    if (!cmd_line.empty() && bg) {
        // printf("30\n");
        jobs->addJob(getCmdLine(),false, pid_2);
       // printf("30\n");
    } else {
         GPID = pid_2;
         strcpy(last_fg_cmd,getCmdLine());
        // printf("31\n");
        if(waitpid(pid_2, nullptr, WUNTRACED)==-1){
         perror("smash error: waitpid failed");
                    return;
        }
       // printf("31\n");
     //    GPID = -1;
       //last_fg_cmd = "";
    }
    }
    
  }
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 int index=-1;
 bool two_red=false, isAmp=false;
 char* from_file=NULL;
 char* to_file=NULL;
 string cmdLine;
 public:
  explicit RedirectionCommand(const char* cmd_line): Command(cmd_line){
   //   printf("getCmdLine() is %s\n",cmd_line);
     cmdLine = getCmd2();
     string cmdFrom = string(cmd_line);
     int index_from;
   // printf("2\n");
     index = cmdLine.find('>');
     index_from = cmdFrom.find('>');
    // printf("3\n");
     if(cmdLine.at(index+1)=='>') {
       //  printf("3\n");
         two_red = true;
         index++;
         index_from++;
     }
     from_file= (char*)malloc(sizeof(char)*200);
     to_file= (char*)malloc(sizeof(char)*200);
    ///printf("4\n");
  //  printf("c_str is %s\n",(cmdLine.substr(0,index)).c_str());
  int to_file_size;
   string sub_str;
  if(two_red){
      string sub_str=(cmdFrom.substr(0,index_from-1));
      strcpy(from_file,sub_str.c_str());
      if(cmdFrom.at(cmdFrom.size()-1)=='&'){
         from_file[index_from-2]='&';
         from_file[index_from-1]=0;
         strcpy(to_file, (cmdLine.substr(index+1,cmdLine.size()-index-2)).c_str());

      }else{
                strcpy(to_file, (cmdLine.substr(index+1,cmdLine.size()-index-1)).c_str());

      }
      
          //printf("sub_str is %s from_file is %s\n",sub_str.c_str(),from_file);

   //  to_file_size = (cmdLine.substr(index+1,cmdLine.size()-index-1)).size();
    //  printf(" ---------- from_file is %s\n",from_file);
  }else{
      string sub_str=(cmdFrom.substr(0,index_from));
      strcpy(from_file,sub_str.c_str());
        if(cmdFrom.at(cmdFrom.size()-1)=='&') {
            from_file[index_from-1]='&';
            from_file[index_from]=0;
            strcpy(to_file, (cmdLine.substr(index+1,cmdLine.size()-index-2)).c_str());
        }else{
            strcpy(to_file, (cmdLine.substr(index+1,cmdLine.size()-index-1)).c_str());
        }
    
 //   printf("sub_str is %s from_file is %s\n",sub_str.c_str(),from_file);
      
     to_file_size = (cmdLine.substr(index+1,cmdLine.size()-index-1)).size();
   // printf(" ---------- from_file is %s ------------------ cmdlINE is %s\n",from_file,cmdLine.c_str());
  }
 //printf("form file is %s | to file is %s\n",from_file,to_file);
     //printf("5\n");
    // strcpy(to_file,getArgs()[getNumArgs()]);
    //printf("6\n");
    
   
     if(to_file_size==0 && to_file[to_file_size - 1]=='&'){
     //   printf("7\n");
        to_file[to_file_size-1] = 0;
      //  printf("8\n");
     }
    //printf("9\n");
      
  }
  virtual ~RedirectionCommand() {
      from_file=NULL;
      to_file=NULL;
  }
  void execute() override{
      //printf("1\n");
      jobs->removeFinishedJobs();
     //printf("2\n");
       //std::ifstream in{from_file};
        //printf("4f\n");
   //int pid = fork();
   
   int fdt;
 //  if(pid<0) perror("smash error: fork failed");
  // else if(pid==0){
        //printf("6f\n");
      /* int stdout_copy = dup(1);
       printf("6.5\n");
        close(stdout_copy);*/
       /// fclose(stdout);
        //printf("7f\n");
        if(two_red){
            //printf("8f\n");
          /*  std::ofstream out;
            out.open(to_file, std::ios::app);
            std::cout.rdbuf(out.rdbuf());*/
            fdt=open(to_file, O_WRONLY | O_APPEND |O_CREAT , 0666);
            if (fdt==-1){
                if(to_file) free(to_file);
                if(from_file)free(from_file);
                perror("smash error: open failed");
                return;
            }
           // printf("to file is %s\n",to_file);
            //printf("9f\n");
        }else{
          //  printf("10f\n");
            fdt =open(to_file, O_WRONLY | O_CREAT| O_TRUNC , 0666);
            if(fdt==-1){
                if(to_file) free(to_file);
                if(from_file)free(from_file);
                perror("smash error: open failed");
                return;
            }
         //   printf("11f\n");
        }
      //  printf("from file is %s\n",from_file);
        int stdout_copy = dup(1);
        //printf("4.5\n");
         dup2(fdt, 1);   
         close(fdt);
      // printf("12f\n");
   //     printf("from file is %s\n",from_file)
    
      // printf("from_file is %s | to_file is %s\n",from_file,to_file);
        SmallShell::getInstance().executeCommand(from_file);
     
            close(1);
       //  printf("10\n");
        dup2(stdout_copy, 1);
         //printf("11\n"); 
         close(stdout_copy);
        //printf("13f\n");
       if(to_file) free(to_file);
                if(from_file)free(from_file);
        //jobs->addJob(getCmdLine(),false, pid);
      // exit(0);
/*   }else{
        printf("6\n");
       if (!cmdLine.empty() && isAmp) {
            printf("7\n");
        jobs->addJob(getCmdLine(),false, pid);
       printf("8\n");
       /* } else {
           printf("9\n");
           // GPID = pid;
           //strcpy(last_fg_cmd,getCmdLine());
         
           if(waitpid(pid, NULL, WUNTRACED)==-1){
                perror("smash error: waitpid failed\n");
           
           }
         //   printf("12\n");
            // GPID = -1;
            //last_fg_cmd = "";
        }
              
    }
    */
  //  std::cout.rdbuf(std::cout.rdbuf());
    //printf("13\n");
  // exit(0);
  }
  //void prepare() override;
  //void cleanup() override;
};


class ChangeDirCommand : public BuiltInCommand { //ADDED
// TODO: Add your data members 
   char* plastpwd;
   char* path;
   char** plastPwd;
 public:
  ChangeDirCommand(const char* cmd_line, char** plastPwd):BuiltInCommand(cmd_line){
	  plastpwd = (char*)malloc(sizeof(char)*200);
	  path = (char*)malloc(sizeof(char)*280);
    if(strcmp(*plastPwd,"")!=0){
	strcpy(plastpwd,(*plastPwd));
	}else{
	strcpy(plastpwd,"");
	}
	this->plastPwd=plastPwd;
  }
  virtual ~ChangeDirCommand() {
	  free(plastpwd);
	  free(path);
  }
  void execute() override{
	  		///  printf("num of args is %d arg1 is %s arg2 is %s\n",getNumArgs(),getArgs()[0],getArgs()[1]);

	  if(getNumArgs()>1){
	    printf("smash error: cd: too many arguments\n");
		return;
	  }	  
	  getcwd(path, MAX_LINE_SIZE);
	  if(strcmp(getArgs()[1],"-")==0){
		  if(strcmp(plastpwd,"")==0){
		  printf("smash error: cd: OLDPWD not set\n");
		  return;
		  }
	      if(chdir(plastpwd)==-1) {
		     perror("smash error: chdir failed");
		  }
	  }else {
	       if(chdir(getArgs()[1])==-1){
		      perror("smash error: chdir failed");
		   }
	  }
	  strcpy(*plastPwd,path);
	 // printf("plastpwd new is %s\n",*plastPwd);
  }
};

class GetCurrDirCommand : public BuiltInCommand {
	char* path;
 public:
  GetCurrDirCommand(const char* cmd_line):BuiltInCommand(cmd_line){
		  path = (char*)malloc(sizeof(char)*280);
    getcwd(path, MAX_LINE_SIZE);  //SHOULD WE CHECK IF GETCWD() FAILS?
  }
  virtual ~GetCurrDirCommand() {}
  void execute() override{
     printf("%s\n", path);
    jobs->removeFinishedJobs();
  }
};

class ShowPidCommand : public BuiltInCommand {
	pid_t smash_pid;
 public:
  ShowPidCommand(const char* cmd_line):BuiltInCommand(cmd_line){
     smash_pid=getpid();
  }
  virtual ~ShowPidCommand() {}
  void execute() override{
  std::cout << "smash pid is " << smash_pid<< std::endl;
  jobs->removeFinishedJobs();
  }
};

class ChpromptCommand: public BuiltInCommand{
    
    public:
    ChpromptCommand(const char* cmd_line): BuiltInCommand(cmd_line){};
    virtual ~ChpromptCommand() {}
    void execute() override{
        //printf("1\n");
    int args_num = getNumArgs();
    char* end_of_prompt = "> ";
    // printf("2\n");
    if(args_num == 0)     //chprompt only
    {
       //  printf("3\n");
        strcpy(smash_prompt,"smash");
       // printf("4\n");
       
    }
    else
    {
        //printf("5\n");
      //std::string str(getArgs()[1]);
      strcpy(smash_prompt,getArgs()[1]);
      // printf("6\n");
     // smash_prompt = getArgs()[1]; 
    }
    // strcpy(smash_prompt,"smash");
    // printf("7\n");
     strcat(smash_prompt,end_of_prompt);
    /////       (*smash_prompt) = (*smash_prompt) + (*end_of_prompt);
     
    }
    
};

class TimeOutCommand: public BuiltInCommand{
    bool timeout=false;
    bool bg=false;
    public:
    TimeOutCommand(const char* cmd_line): BuiltInCommand(cmd_line){
      timeout=true;
    }
    virtual ~TimeOutCommand() {}
    void execute() override{
      //  printf("g1\n");
    if(getNumArgs() < 2) {
        alarm_flag=false;	  
        std::cout << "smash error: kill: invalid arguments"<< std::endl;	
        return;
    }
      //  printf("g2\n");
       //char* timeout_duration= getArgs()[1];
       char* duration_temp_str=(char*)malloc(strlen(getArgs()[1])+1); //changed
       strcpy(duration_temp_str,getArgs()[1]);
       int duration = atoi(duration_temp_str);
       free(duration_temp_str);
       if(duration<=0) { 
            alarm_flag=false;	   
            std::cout << "smash error: kill: invalid arguments"<< std::endl;	
            return;
       }
       
       char* cmd = (char*)malloc(200*(sizeof(char)));
        if(getCmdLine()[strlen(getCmdLine()) - 1]== '&'){
	      bg=true;
	   }
        int i = 3;
        strcpy(cmd, getArgs()[2]);
       // printf("1cmd is %s\n",cmd);
       // cmd[strlen(cmd)-1]=0;
        while((!bg && i != getNumArgs()+1 ) || (bg &&  i != getNumArgs() +1))
        { 
            
            
            if((!bg && i != getNumArgs()+1 ) || (bg &&  i != getNumArgs()+1)) 
            {
                strcat(cmd , " ");
                strcat(cmd, getArgs()[i]);
               // printf("2cmd is %s\n",cmd);
            }
            i++;
        }
        alarm(duration);   	
        alarm_flag = true;
        TimeOutEntry* timeout_entry = new TimeOutEntry();
        timeout_entry->duration = duration;
        timeout_entry->timestamp = time(NULL);
        timeoutsList.push_back(timeout_entry);
       // printf("cmd is %s\n",cmd);
        SmallShell::getInstance().executeCommand(cmd);
        alarm_flag=false;
    }
    
};

class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
bool canKill=false;
JobsList* joblist;
public:
  QuitCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){
	 this->joblist=jobs;
	 if(getNumArgs()!=0 && (!strcmp(getArgs()[1],"kill"))){
		  canKill= true;
	 }
  }
  virtual ~QuitCommand() {}
  void execute() override{
    if(canKill){
      joblist->killAllJobs();
	}
  //  printf("fff\n");
	free(last_fg_cmd);
    //printf("ggg\n");
	delete jobs;
    //printf("kkk\n");
	exit(0);
  }
};


class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 JobsList* jobsList;
 public:
  JobsCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){
    jobsList=jobs;  
  }
  virtual ~JobsCommand() {}
  void execute() override{
     jobsList->printJobsList();
  }
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
   JobEntry* job;
   int sig_number;
   int job_id;
   bool flag=false;
 public:
  KillCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){
	  //job_id = atoi(getArgs()[2]);
       
	  char* temp;
	   if(getNumArgs()!=2){
	    flag = true;
		return;
	  }
	  job_id=strtol(getArgs()[2],&temp, 0);
	  if(*temp != '\0'){
		flag=true;
       }
	  //jobs->removeFinishedJobs();
	 this->job = jobs->getJobById(job_id);
     sig_number = (-1) * atoi(getArgs()[1]);

  }
  virtual ~KillCommand() {}
  void execute() override{
  if(flag){
	   std::cout << "smash error: kill: invalid arguments"<< std::endl;
	  return;
  }
	 /* if(sig_number < 1 || sig_number > 32){
		 std::cout << "smash error: kill: invalid arguments"<< std::endl;
		 return;
	  }*/
	  if(!job){
		  std::cout << "smash error: kill: job-id " << job_id << " does not exist" << std::endl;
		  return;
		}
     if(kill(job->process_id,sig_number)!=0){
	  perror("smash error: kill failed");
	   return;
	 }
	 std::cout << "signal number "<< sig_number<< " was sent to pid "<< job->process_id<< std::endl;
     jobs->removeFinishedJobs();
  }
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 int JobId;
 JobEntry* Job;
 bool flag=false;
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){
	     //printf("fg constuctor\n");

	  jobs->removeFinishedJobs();
	     //printf("1\n");

  if(getNumArgs()>1){
	 	    printf("2\n");
            printf("cmd_line is %s\n",cmd_line);
    printf("smash error: fg: invalid arguments\n");
	flag=true;
	return;
  }
  if(getNumArgs()==0){
  	   //  printf("3\n");
  Job = jobs->getLastJob(&JobId);
  	   //  printf("4\n");
  if(!Job){
    printf("smash error: fg: jobs list is empty\n");
	flag=true;
	return;
  }
  }else{
	  	 //    printf("5\n");
	  char* temp;
	  int job_id=strtol(getArgs()[1],&temp, 0);
	  	  //   printf("6\n");
	  if(*temp != '\0' ){
	    std::cout << "smash error: fg: invalid arguments"<< std::endl;
		flag=true;
		return;
       }
	   	   //  printf("7\n");
  Job = jobs->getJobById(job_id);
  	    // printf("8\n");
  if(!Job || job_id<=0){
  printf("smash error: fg: job-id %d does not exist\n",atoi(getArgs()[1]));
  flag=true;
  return;
  }
  	  //   printf("10\n");
  JobId = Job->job_Id;
  }
  	  //   printf("11\n");
  strcpy(last_fg_cmd,Job->command);
  	 //    printf("12\n");
  GPID = Job->process_id;
  	  //   printf("14\n");
  Job->finished=true;
  //printf("job id is %d\n",JobId);
  
  }
  virtual ~ForegroundCommand() {}
  void execute() override{
	  if(flag) return;
	   /// printf("hihihihihihi\n");
	//  printf("we are executing fg on %s\n",Job->command);
    if ((kill(GPID, SIGCONT)) < 0) {
			perror("smash error: kill failed");
		} else {
			printf("%s : %d\n", Job->command,GPID);
		}
  //   printf("gpidd is %d\n",GPID);
  if (waitpid(GPID, NULL, WUNTRACED) < 0) {
		perror("smash error: waitpid failed\n");
	} 
  }
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 JobEntry* job;
 int job_id;
 JobsList* jobs;
 public:
  BackgroundCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){
	 this->jobs=jobs;
	 jobs->removeFinishedJobs();
  }
  virtual ~BackgroundCommand() {}
  void execute() override{
	   if(getNumArgs()>1){
	   std::cout << "smash error: bg: invalid arguments" << std::endl;
	   return;
	 }
     if(getNumArgs()==0){
      //   printf("&&\n");
	  job = jobs->getLastStoppedJob(&job_id);
      //printf("%%\n");
	  if(!job){
	    std::cout << "smash error: bg: there is no stopped jobs to resume" << std::endl;
		return;
	 }}
	   if(getNumArgs()==1){
		    char* temp;
	  int job_id=strtol(getArgs()[1],&temp, 0);
	  	  //   printf("6\n");
	  if(*temp != '\0' ){
          std::cout << "smash error: bg: invalid arguments" << std::endl;
		return;
       }	   
	  job = jobs->getJobById(job_id);
      if(!job || job_id <=0){
	  std::cout << "smash error: bg: job-id " << job_id <<" does not exist" << std::endl;
	  		return;
	  }
	  if(!job->stopped){
	    std::cout << "smash error: bg: job-id " << job_id << " is already running in the background" << std::endl;
				return;
	  }
	 }
    jobs->removeStoppedJob(job->job_Id);
	 if (kill(job->process_id,SIGCONT) < 0) {
			perror("smash error: kill failed");
		} else {
			printf("%s : %d\n", job->command,job->process_id);
		}
  
  }
};


class CopyCommand : public BuiltInCommand {
	char buffer[1024];//buffer size?
    int files[2];
    ssize_t count;
 public:
  CopyCommand(const char* cmd_line):BuiltInCommand(cmd_line){
  }
  virtual ~CopyCommand() {
  }
  void execute() override{

	 std::ifstream in{getArgs()[1]};
	 if (!in) {
         perror("smash error: open failed");
	   return;
    }
    std::remove(getArgs()[2]);
    std::ofstream out{getArgs()[2]};
    if (!out) {
         perror("smash error: open failed");
	   return;
    }
    static constexpr std::size_t buffsize{1024};
    char buffer[buffsize];
    while (in.read(buffer, buffsize)) {
        out.write(buffer, buffsize);
    }
    out.write(buffer, in.gcount());
	printf("smash: %s was copied to %s\n",getArgs()[1],getArgs()[2]);
	jobs->removeFinishedJobs();
  }
};

class LSCommand : public BuiltInCommand {
    struct dirent **filelist; 
    int res;
 public:
  LSCommand(const char* cmd_line):BuiltInCommand(cmd_line){
       res=scandir(".",&filelist,NULL,alphasort);
       if(res<0) perror("smash error: scandir failed");
  }
  virtual ~LSCommand() {
      free(filelist);
  }
  void execute() override{
    if(res>=0){
        int indx=0;
        while(indx<res){
            printf("%s\n",filelist[indx]->d_name);
            free(filelist[indx]);
            indx++;
        }
    }
    
    
  }
};

#endif //SMASH_COMMAND_H_
