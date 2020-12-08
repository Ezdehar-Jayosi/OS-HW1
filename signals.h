#ifndef SMASH__SIGNALS_H_
#define SMASH__SIGNALS_H_
#include "Commands.h"

extern JobsList* jobs;
extern char* last_fg_cmd;
extern int GPID;
extern std::vector<TimeOutEntry*> timeoutsList;
void ctrlZHandler(int sig_num);
void ctrlCHandler(int sig_num);
void alarmHandler(int sign_num);
#endif //SMASH__SIGNALS_H_
