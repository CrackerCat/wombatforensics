#ifndef SLEUTHKIT_H 
#define SLEUTHKIT_H

#include <main/interfaces.h>
#include <sqlite3.h>
#include <tsk/framework/framework.h>
#include <time.h>
#include "TskSystemPropertiesImpl.h"
#include "TskImgDBSqlite.h"
#include "TskDBBlackboard.h"
#include "TskSchedulerQueue.h"
#include "TskFileManagerImpl.h"
#include "TskImageFileTsk.h"
#include "TskFileTsk.h"
#include <QtPlugin>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QXmlStreamWriter>
#include <QStandardItemModel>
#include <QByteArray>
#include <QDataStream>

class SleuthKitPlugin : public QObject, public SleuthKitInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "wombat.SleuthKitInterface" FILE "sleuthkit.json")
    Q_INTERFACES(SleuthKitInterface)
    
public:
    //SleuthKit Interface Functions
    void SetupSystemProperties(QString settingsPath, QString configFilePath);
    void SetupSystemLog(QString dataPath, QString logFilePath);
    void SetupImageDatabase(QString imgDBPath, QString evidenceFilePath);
    void SetupSystemBlackboard();
    void SetupSystemSchedulerQueue();
    void SetupSystemFileManager();
    void OpenEvidence(QString evidencePath);
    QStandardItemModel* GetCurrentImageDirectoryTree(void);
    QString GetFileContents(int fileID);
    QString GetFileTxtContents(int fileID);

private:
    int currentcaseid;
    TskSystemPropertiesImpl* systemproperties;
    TskFileManagerImpl* fileManager;
    std::auto_ptr<Log> log;
    std::auto_ptr<TskImgDB> imgdb;
    TskSchedulerQueue scheduler;
};

#endif // SLEUTHKIT_H
