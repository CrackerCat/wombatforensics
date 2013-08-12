#include "sleuthkit.h"

void SleuthKitPlugin::SetupSystemProperties(QString settingsPath, QString configFilePath)
{
    QString tmpPath = settingsPath;
    tmpPath += "/";
    tmpPath += configFilePath;
    QFile tmpFile(tmpPath);
    fprintf(stderr, "TmpPath: %s\n", tmpPath.toStdString().c_str());
    if(!tmpFile.exists()) // if tsk-config.xml does not exist, create and write it here
    {
        if(tmpFile.open(QFile::WriteOnly | QFile::Text))
        {   
            QXmlStreamWriter xml(&tmpFile);
            xml.setAutoFormatting(true);
            xml.writeStartDocument();
            xml.writeStartElement("TSK_FRAMEWORK_CONFIG");
            xml.writeStartElement("CONFIG_DIR");
            xml.writeCharacters(settingsPath);
            xml.writeEndElement();
            xml.writeStartElement("MODULE_DIR");
            xml.writeCharacters("/usr/local/lib");
            xml.writeEndElement();
            xml.writeStartElement("MODULE_CONFIG_DIR");
            xml.writeCharacters("/usr/local/share/tsk");
            xml.writeEndElement();
            xml.writeStartElement("PIPELINE_CONFIG_FILE");
            xml.writeCharacters("#CONFIG_DIR#/tsk-pipe.xml");
            xml.writeEndElement();
            xml.writeEndElement();
            xml.writeEndDocument();
        }
        else
        {
            fprintf(stderr, "Could not open file to write\n");
        }
        tmpFile.close();
    }
    try
    {
        systemproperties = new TskSystemPropertiesImpl();
        systemproperties->initialize(tmpPath.toStdString());
        TskServices::Instance().setSystemProperties(*systemproperties);
        fprintf(stderr, "Configuration File Loading was successful!\n");
    }
    catch(TskException &ex)
    {
        fprintf(stderr, "Loading Config File config file: %s\n", ex.message().c_str());
    }
    tmpPath = settingsPath;
    tmpPath += "/tsk-pipe.xml";
    QFile pipeFile(tmpPath);
    fprintf(stderr, "PipPath: %s\n", tmpPath.toStdString().c_str());
    if(!pipeFile.exists()) // if tsk-pipe.xml does not exist, create and write it here
    {
        if(pipeFile.open(QFile::WriteOnly | QFile::Text))
        {
            QXmlStreamWriter pxml(&pipeFile);
            pxml.setAutoFormatting(true);
            pxml.writeStartDocument();
            pxml.writeStartElement("PIPELINE_CONFIG");
            pxml.writeStartElement("PIPELINE");
            pxml.writeAttribute("type", "FileAnalysis");
            pxml.writeStartElement("MODULE");
            pxml.writeAttribute("order", "1");
            pxml.writeAttribute("type", "plugin");
            pxml.writeAttribute("location", "tskHashCalcModule");
            pxml.writeEndElement();
            pxml.writeStartElement("MODULE");
            pxml.writeAttribute("order", "2");
            pxml.writeAttribute("type", "plugin");
            pxml.writeAttribute("location", "tskFileTypeSigModule");
            pxml.writeEndElement();
            pxml.writeEndElement();
            pxml.writeEndElement();
            pxml.writeEndDocument();
        }
        else
            fprintf(stderr, "Could not open file for writing\n");
        pipeFile.close();
    }
    try
    {
        SetSystemProperty(TskSystemProperties::PIPELINE_CONFIG_FILE, tmpPath.toStdString());
    }
    catch(TskException &ex)
    {
        fprintf(stderr, "couldn't load pipeline: %s\n", ex.message().c_str());
    }
    try
    {
        SetSystemProperty(TskSystemProperties::OUT_DIR, "/home/pasquale/WombatForensics/tmpfiles");
    }
    catch(TskException &ex)
    {
        fprintf(stderr, "Setting out dir failed: %s\n", ex.message().c_str());
    }
}
void SleuthKitPlugin::SetupSystemLog(QString dataPath, QString logFilePath)
{
    QString tmpPath = dataPath;
    tmpPath += logFilePath;
    try
    {
        log = std::auto_ptr<Log>(new Log());
        log->open(tmpPath.toStdString().c_str());
        TskServices::Instance().setLog(*log);
        fprintf(stderr, "Loading Log File was successful!\n");
    }
    catch(TskException &ex)
    {
        fprintf(stderr, "Loading Log File: %s\n", ex.message().c_str());
    }
}
QString SleuthKitPlugin::SetupImageDatabase(QString imgDBPath, QString evidenceFilePath)
{
    QString evidenceFileName = evidenceFilePath.split("/").last();
    evidenceFileName += ".db";

    try
    {
        imgdb = std::auto_ptr<TskImgDB>(new TskImgDBSqlite(imgDBPath.toStdString().c_str(), evidenceFileName.toStdString().c_str()));
        if(imgdb->initialize() != 0)
            fprintf(stderr, "Error initializing db\n");
        else
        {
            fprintf(stderr, "DB was Initialized Successfully!\n");
        }
        TskServices::Instance().setImgDB(*imgdb);
        fprintf(stderr, "Loading ImageDB was Successful!\n");
    }
    catch(TskException &ex)
    {
        fprintf(stderr, "Loading ImageDB: %s\n", ex.message().c_str());
    }
    return evidenceFilePath;
}

void SleuthKitPlugin::OpenImageDatabase(QString imgDBPath, QString evidenceFilePath)
{
    QString evidenceFileName = evidenceFilePath.split("/").last();
    evidenceFileName += ".db";

    try
    {
        imgdb = std::auto_ptr<TskImgDB>(new TskImgDBSqlite(imgDBPath.toStdString().c_str(), evidenceFileName.toStdString().c_str()));
        if(imgdb->open() != 0)
            fprintf(stderr, "Error opening db\n");
        else
        {
            fprintf(stderr, "DB was Opened Successfully!\n");
        }
        TskServices::Instance().setImgDB(*imgdb);
        fprintf(stderr, "Loading ImageDB was Successful!\n");
    }
    catch(TskException &ex)
    {
        fprintf(stderr, "Loading ImageDB: %s\n", ex.message().c_str());
    }
}

void SleuthKitPlugin::SetupSystemBlackboard()
{
    try
    {
        TskServices::Instance().setBlackboard((TskBlackboard &)TskDBBlackboard::instance());
        fprintf(stderr, "Loading Blackboard was successful!\n");
    }
    catch(TskException &ex)
    {
        fprintf(stderr, "Loading Blackboard: %s\n", ex.message().c_str());
    }
}
void SleuthKitPlugin::SetupSystemSchedulerQueue()
{
    try
    {
        TskServices::Instance().setScheduler(scheduler);
        fprintf(stderr, "Loading Scheduler was successful!\n");
    }
    catch(TskException &ex)
    {
        fprintf(stderr, "Loading Scheduler: %s\n", ex.message().c_str());
    }
}
void SleuthKitPlugin::SetupSystemFileManager()
{
    try
    {
        TskServices::Instance().setFileManager(fileManager->instance());
        fprintf(stderr, "Loading File Manager was successful!\n");
    }
    catch(TskException &ex)
    {
        fprintf(stderr, "Loading FileManager: %s\n", ex.message().c_str());
    }
}
void SleuthKitPlugin::OpenEvidence(QString evidencePath)
{
    TskPipelineManager pipelineMgr;
    TskPipeline *filePipeline;
    try
    {
        filePipeline = pipelineMgr.createPipeline(TskPipelineManager::FILE_ANALYSIS_PIPELINE);
    }
    catch(const TskException &ex)
    {
        fprintf(stderr, "Error creating file analysis pipeline: %s\n", ex.message().c_str());
    }

    TskImageFileTsk imagefiletsk;
    try
    {
        imagefiletsk.open(evidencePath.toStdString());
        TskServices::Instance().setImageFile(imagefiletsk);
        fprintf(stderr, "Opening Image File was successful!\n");
    }
    catch(TskException &ex)
    {
        fprintf(stderr, "Opening Evidence: %s\n", ex.message().c_str());
    }
    try
    {
        imagefiletsk.extractFiles();
        fprintf(stderr, "Extracting Evidence was successful\n");
    }
    catch(TskException &ex)
    {
        fprintf(stderr, "Extracting Evidence: %s\n", ex.message().c_str());
    }
    TskSchedulerQueue::task_struct *task;
    while((task = scheduler.nextTask()) != NULL)
    {
        try
        {
            if(task->task == Scheduler::FileAnalysis && filePipeline && !filePipeline->isEmpty())
            {
                filePipeline->run(task->id);
            }
            else
            {
                fprintf(stderr, "Skipping task: %s\n", task->task);
            }
            //delete task;
        }
        catch(TskException &ex)
        {
            fprintf(stderr, "TskException: %s\n", ex.message().c_str());
        }
    }
    delete task;
    if(filePipeline && !filePipeline->isEmpty())
    {
        filePipeline->logModuleExecutionTimes();
    }
}

QStandardItem* SleuthKitPlugin::GetCurrentImageDirectoryTree(QString imageDbPath, QString imageName)
{
    //std::vector<uint64_t> fileidVector;
    std::vector<TskFileRecord> fileRecordVector;
    std::list<TskVolumeInfoRecord> volRecordList;
    std::list<TskFsInfoRecord> fsInfoRecordList;
    QString fullPath = imageName + "/";
    QString currentVolPath = "";
    QString currentFsPath = "";
    //fileidVector = imgdb->getFileIds();
    TskFileRecord tmpRecord;
    TskVolumeInfoRecord volRecord;
    TskFsInfoRecord fsInfoRecord;
    QStandardItem *fsNode;
    QStandardItem *volNode;
    QStandardItem *imageNode = new QStandardItem(imageName);
    int ret;
    uint64_t tmpId;
    // also need to get the partitions and volumes as nodes.
    ret = imgdb->getVolumeInfo(volRecordList);
    foreach(volRecord, volRecordList) // populates all vol's and fs's.  need to do a better loop where i add files to current vol/fs as they go.
    {
        // if volflag = 0, get description
        // if volflag = 1, list as unallocated
        fprintf(stderr, "Vol Description: %s - VolFlags: %d\n", volRecord.description.c_str(), volRecord.flags);
        if(volRecord.flags >= 0 && volRecord.flags <= 2)
        {
            if(volRecord.flags == 0)
            {
                volNode = new QStandardItem(QString::fromUtf8(volRecord.description.c_str()));
                currentVolPath = QString::fromUtf8(volRecord.description.c_str()) + "/";
            }
            else if(volRecord.flags == 1)
            {
                volNode = new QStandardItem("unallocated space");
                currentVolPath = "unallocated space/";
            }
            else if(volRecord.flags == 2)
            {
                volNode = new QStandardItem(QString::fromUtf8(volRecord.description.c_str()));
                currentVolPath = QString::fromUtf8(volRecord.description.c_str());
                currentVolPath += "/";
            }
            else
            {
                // don't display anything
            }
            // for each volrecord, get fsinfo list
            imageNode->appendRow(volNode);
            ret = imgdb->getFsInfo(fsInfoRecordList);
            foreach(fsInfoRecord, fsInfoRecordList)
            {
                if(fsInfoRecord.vol_id == volRecord.vol_id)
                {
                    QString fsType = "";
                    // NEED TO DO SWITCH/CASE HERE TO GET FSTYPE
                    if(fsInfoRecord.fs_type == 1)
                        fsType = "NTFS";
                    else if(fsInfoRecord.fs_type == 3)
                        fsType = "FAT12";
                    else if(fsInfoRecord.fs_type == 4)
                        fsType = "FAT16";
                    else if(fsInfoRecord.fs_type == 5)
                        fsType = "FAT32";
                    else if(fsInfoRecord.fs_type == 7)
                        fsType = "UFS1";
                    else if(fsInfoRecord.fs_type == 8)
                        fsType = "UFS1B";
                    else if(fsInfoRecord.fs_type == 9)
                        fsType = "UFS2";
                    else if(fsInfoRecord.fs_type == 11)
                        fsType = "EXT2";
                    else if(fsInfoRecord.fs_type == 12)
                        fsType = "EXT3";
                    else if(fsInfoRecord.fs_type == 14)
                        fsType = "SWAP";
                    else if(fsInfoRecord.fs_type == 16)
                        fsType = "RAW";
                    else if(fsInfoRecord.fs_type == 18)
                        fsType = "ISO9660";
                    else if(fsInfoRecord.fs_type == 20)
                        fsType = "HFS";
                    else if(fsInfoRecord.fs_type == 22)
                        fsType = "EXT4";
                    else if(fsInfoRecord.fs_type == 23)
                        fsType = "YAFFS2";
                    else if(fsInfoRecord.fs_type == 25)
                        fsType = "UNSUPPORTED";
                    else
                        fsType = QString::number(fsInfoRecord.fs_type);
                    fsNode = new QStandardItem(fsType);
                    currentFsPath = fsType + "/";
                    volNode->appendRow(fsNode);
                    //BEGIN FILE ADD CODE
                    std::vector<uint64_t> fileidVector;
                    //Create custom function to access this...
                    sqlite3* tmpImgDB;
                    QString tmpImgDbPath = imageDbPath + imageName + ".db";
                    if(sqlite3_open(tmpImgDbPath.toStdString().c_str(), &tmpImgDB) == SQLITE_OK)
                    {
                        sqlite3_stmt* stmt;
                        if(sqlite3_prepare_v2(tmpImgDB, "SELECT file_id FROM fs_files WHERE fs_id = ? ORDER BY file_id", -1, &stmt, 0) == SQLITE_OK)
                        {
                            if(sqlite3_bind_int(stmt, 1, fsInfoRecord.fs_id) == SQLITE_OK)
                            {
                                while(sqlite3_step(stmt) == SQLITE_ROW)
                                {
                                    uint64_t fileId = (uint64_t)sqlite3_column_int(stmt, 0);
                                    fileidVector.push_back(fileId);
                                }
                                sqlite3_finalize(stmt);
                            }
                            else
                            {
                                //std::wstringstream infoMessage;
                                //infoMessage << L"Get FsFileIds Failed: " << sqlite3_errmsg(imgdb);
                                //LOGERROR(infoMessage.c_str());
    
                            }
                        }
                        else
                        {
                            //std::wstringstream infoMessage;
                            //infoMessage << L"Get FsFileIds Failed: " << sqlite3_errmsg(imgdb);
                            //LOGERROR(infoMessage.c_str());
                        }
                    }
                    sqlite3_close(tmpImgDB);
                    //end create custom function to access fileid
                    //fileidVector = imgdb->getFsFileIds(tmpWhere.toStdString());
                    QList<QList<QStandardItem*> > treeList;
                    foreach(tmpId, fileidVector)
                    {
                        ret = imgdb->getFileRecord(tmpId, tmpRecord);
                        fileRecordVector.push_back(tmpRecord);
                    }
                    for(int i=0; i < (int)fileRecordVector.size(); i++)
                    {
                        //QString fullPath = "Image Name/Partition #/Volume Name[FSTYPE]/[root]/";
                        fullPath += currentVolPath + currentFsPath;
                        // full path might contain more than i thought, to include unalloc and whatnot
                        fullPath += QString(fileRecordVector[i].fullPath.c_str());
                        QList<QStandardItem*> sleuthList;
                        sleuthList << new QStandardItem(QString::number((int)fileRecordVector[i].fileId));
                        sleuthList << new QStandardItem(QString(fileRecordVector[i].name.c_str()));
                        sleuthList << new QStandardItem(fullPath);
                        sleuthList << new QStandardItem(QString::number(fileRecordVector[i].size));
                        sleuthList << new QStandardItem(QString::number(fileRecordVector[i].crtime));
                        //sleuthList << new QStandardItem(QString(ctime(((const time_t*)fileRecordVector[i].crtime))));
                        sleuthList << new QStandardItem(QString(fileRecordVector[i].md5.c_str()));
                        treeList.append(sleuthList);
                    }
                    for(int i = 0; i < (int)fileRecordVector.size(); i++)
                    {
                        // NEED TO EXPAND FOR OTHER ICONS
                        QStandardItem* tmpItem2 = ((QStandardItem*)treeList[i].first());
                        if(((TskFileRecord)fileRecordVector[i]).dirType == 3)
                        {
                            tmpItem2->setIcon(QIcon(":/basic/treefolder"));
                        }
                        else
                        {
                            tmpItem2->setIcon(QIcon(":/basic/treefile"));
                        }
                        if(((TskFileRecord)fileRecordVector[i]).parentFileId == 1)
                        {
                            fsNode->appendRow(treeList[i]);
                            //imageNode->appendRow(treeList[i]);
                        }
                    }
                    for(int i=0; i < (int)fileRecordVector.size(); i++)
                    {
                        tmpRecord = fileRecordVector[i];
                        if(tmpRecord.parentFileId > 1)
                        {
                            ((QStandardItem*)treeList[tmpRecord.parentFileId-1].first())->appendRow(treeList[i]);
                        }
                    }
                    //END FILE ADD CODE
                }
                //volNode->appendRow(fsNode);
            }
        }
        //imageNode->appendRow(volNode);
    }
    /*
    QList<QList<QStandardItem*> > treeList;
    foreach(tmpId, fileidVector)
    {
        ret = imgdb->getFileRecord(tmpId, tmpRecord);
        fileRecordVector.push_back(tmpRecord);
    }
    for(int i=0; i < (int)fileRecordVector.size(); i++)
    {
        //QString fullPath = "Image Name/Partition #/Volume Name[FSTYPE]/[root]/";
        fullPath += currentVolPath + currentFsPath;
        fullPath += QString(fileRecordVector[i].fullPath.c_str());
        QList<QStandardItem*> sleuthList;
        sleuthList << new QStandardItem(QString::number((int)fileRecordVector[i].fileId));
        sleuthList << new QStandardItem(QString(fileRecordVector[i].name.c_str()));
        sleuthList << new QStandardItem(fullPath);
        sleuthList << new QStandardItem(QString::number(fileRecordVector[i].size));
        sleuthList << new QStandardItem(QString::number(fileRecordVector[i].crtime));
        //sleuthList << new QStandardItem(QString(ctime(((const time_t*)fileRecordVector[i].crtime))));
        sleuthList << new QStandardItem(QString(fileRecordVector[i].md5.c_str()));
        treeList.append(sleuthList);
    }
    for(int i = 0; i < (int)fileRecordVector.size(); i++)
    {
        QStandardItem* tmpItem2 = ((QStandardItem*)treeList[i].first());

        if(((TskFileRecord)fileRecordVector[i]).dirType == 3)
        {
            tmpItem2->setIcon(QIcon(":/basic/treefolder"));
        }
        else
        {
            tmpItem2->setIcon(QIcon(":/basic/treefile"));
        }
        if(((TskFileRecord)fileRecordVector[i]).parentFileId == 1)
        {
            // getFileUniqueIdentifiers()
            // if fs_id == fsFileId then add the file to fsNode
            imageNode->appendRow(treeList[i]);
        }
    }
    for(int i=0; i < (int)fileRecordVector.size(); i++)
    {
        tmpRecord = fileRecordVector[i];
        if(tmpRecord.parentFileId > 1)
        {
            ((QStandardItem*)treeList[tmpRecord.parentFileId-1].first())->appendRow(treeList[i]);
        }
    }*/
    return imageNode;
}

QString SleuthKitPlugin::GetFileContents(int fileID)
{
    TskFile *tmpFile = TskServices::Instance().getFileManager().getFile((uint64_t)fileID);
    fprintf(stderr, "TskFile ID: %i :: GetSize: %i :: Name: %s\n", tmpFile->getId(), tmpFile->getSize(), tmpFile->getName().c_str());
    char buffer[32768];
    ssize_t bytesRead = 0;
    bytesRead = tmpFile->read(buffer, 32768);
    QByteArray ba;
    QFile qFile("/home/pasquale/WombatForensics/tmpfiles/tmp.dat");
    qFile.open(QIODevice::ReadWrite);
    qFile.write((const char*)buffer, 32768);
    qFile.close();
    return "/home/pasquale/WombatForensics/tmpfiles/tmp.dat";
}
QString SleuthKitPlugin::GetFileTxtContents(int fileID)
{
    TskFile *tmpFile = TskServices::Instance().getFileManager().getFile((uint64_t)fileID);
    char buffer[32768];
    ssize_t bytesRead = 0;
    bytesRead = tmpFile->read(buffer, 32768);
    QFile qFile("/home/pasquale/WombatForensics/tmpfiles/tmp.txt");
    qFile.open(QIODevice::ReadWrite);
    qFile.write((const char*)buffer, 32768);
    qFile.close();
    return "/home/pasquale/WombatForensics/tmpfiles/tmp.txt";
}
