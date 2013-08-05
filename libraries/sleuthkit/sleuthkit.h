#ifndef SLEUTHKIT_H 
#define SLEUTHKIT_H

#include <main/interfaces.h>
#include <sqlite3.h>
#include <tsk/framework/framework.h>
#include "TskSystemPropertiesImpl.h"
#include "TskImgDBSqlite.h"
#include "TskDBBlackboard.h"
#include "TskSchedulerQueue.h"
#include "TskFileManagerImpl.h"
#include "TskImageFileTsk.h"
#include <QtPlugin>
#include <QObject>
#include <QString>
#include <QStringList>

class SleuthKitPlugin : public QObject, public SleuthKitInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "wombat.SleuthKitInterface" FILE "sleuthkit.json")
    Q_INTERFACES(SleuthKitInterface)
    
public:
    //SleuthKit Interface Functions
    void SetupSystemProperties(QString configFilePath);
    void SetupSystemLog(QString logFilePath);
    void SetupImageDatabase(QString imgDBPath);
    void SetupSystemBlackboard();
    void SetupSystemSchedulerQueue();
    void SetupSystemFileManager();
    void OpenEvidence(QString evidencePath);

    //QStringList evidenceActions() const;
    //QStringList evidenceActionIcons() const;

    //void addEvidence(int currentCaseID);
    //void remEvidence(int currentCaseID);

private:
    int currentcaseid;
    TskSystemPropertiesImpl* systemproperties;
    std::auto_ptr<Log> log;
    std::auto_ptr<TskImgDB> imgdb;
    TskImageFileTsk imagefiletsk;
    TskSchedulerQueue scheduler;

};

#endif // SLEUTHKIT_H
