#ifndef TSKLOG_H
#define TSKLOG_H

#include <time.h>
#include <main/wombatvariable.h>
#include <tsk/framework/services/Log.h>
#include <tsk/framework/framework_i.h>
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <fstream>

class TskLog : public Log
{
public:

    TskLog(WombatVariable *logVariable);
    virtual ~TskLog();

    void log(Channel msgType, const std::string &logMsg);

protected:
    std::string logpath;
    std::string dbpath;
    std::ofstream outstream;
    int caseid;
    int imageid;
    int analysistype;
    WombatVariable *logvariable;

};
#endif
