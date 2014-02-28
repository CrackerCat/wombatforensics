#ifndef WOMBATVARIABLE_H
#define WOMBATVARIABLE_H

#include "wombatinclude.h"

struct CaseObject
{
    int id;
    QString name;
    QString dbname;
    QString dirpath;
};

struct VolumeObject
{
    int id;
    int objecttype;
    int type;
    int blocksize;
    int byteoffset;
    int parentid;
    QString name;
};

struct PartitionObject
{
    int id;
    int objecttype;
    int flags;
    int sectstart;
    int sectlength;
    int parentid;
    QString name;
};

struct FileSystemObject
{
    int id;
    QString name;
};

struct EvidenceObject
{
    int id;
    int type;
    int objecttype;
    int itemcount;
    QString name;
    int size;
    int sectsize;
    QStringList namelist;
    QString fullpath; 
    std::vector<std::string> fullpathvector;
    TSK_IMG_INFO* imageinfo; // These may disappear when the image is closed, will have to test this concept
    TSK_VS_INFO* volinfo; // may not need..
    //std::vector<TSK_VS_INFO*> volinfovector; // should use instead of volinfo, simply search for volumes and add them based on the size and offset to search for another one...
    TSK_VS_PART_INFO* partinfo; // may not need.
    std::vector<const TSK_VS_PART_INFO*> partinfovector;
    std::vector<TSK_FS_INFO*> fsinfovector; 
    QString dbname;
    void Clear()
    {
        fullpath = "";
        name = "";
        id = 0;
        type = 0;
        itemcount = 0;
        size = 0;
        sectsize = 0;
        namelist.clear();
        //dirpath = "";
        fullpathvector.clear();
        imageinfo = NULL;
        volinfo = NULL;
        partinfo = NULL;
        partinfovector.clear();
        fsinfovector.clear();
        dbname = "";
    };
};

struct FileExportData
{
    enum PathStatus
    {
        include = 0,
        exclude = 1
    };

    enum FileStatus
    {
        selected = 0,
        checked = 1,
        listed = 2
    };

    std::string exportpath;
    int id;
    FileStatus filestatus;
    PathStatus pathstatus;
    std::string name;
    std::string fullpath;
    EvidenceObject evidenceobject;
    //std::string evidencedbname;
    //std::string evidencepath;
    int exportcount;
};

struct SelectedObject
{
    int id;
    int type;
    int name;
};

struct WombatVariable
{

    // ADD ENUMS FOR JOBSTATUS, JOBTYPE, ETC TO SIMPLIFY INTEGER ASSOCIATION AND READABILITY IN CODE
    enum JobStatus
    {
        failed = 0,
        finish = 1,
        finisherror = 2,
        cancelled = 3
    };

    enum JobType
    {
        generic = 0,
        addevidence = 1,
        remedvidence = 2,
        exportfiles = 3
    };

    // id values
    int jobtype;
    int jobid;
    int fileid;
    int volid;
    int filecount;
    int processcount;
    int omnivalue;
    int visibleviewer;
    QString casespath;
    QString settingspath;
    QString datapath;
    QString tmpfilepath;
    QString curerrmsg;
    QString wombatdbname;
    QStandardItemModel* dirmodel;
    QList<int> objectidlist;
    CaseObject caseobject;
    QStringList casenamelist;
    EvidenceObject evidenceobject;
    VolumeObject volumeobject;
    QVector<EvidenceObject> evidenceobjectvector;
    QVector<VolumeObject> volumeobjectvector; // may not need
    PartitionObject partitionobject;
    QVector<PartitionObject> partitionobjectvector;
    FileSystemObject filesystemobject;
    QVector<FileSystemObject> filesystemobjectvector;
    SelectedObject selectedobject;
    FileExportData exportdata;
    QVector<FileExportData> exportdatavector;
    QByteArray bootbytearray; // byte array of currently extracted rawdata -> clear() before each use
    std::vector<uchar> rawbyteintvector;
    QString htmlcontent;
    QSqlDatabase casedb;
    QSqlDatabase appdb;
    QList<QSqlRecord> sqlrecords;
    QList<QVariant> bindvalues;
};

Q_DECLARE_METATYPE(WombatVariable)
Q_DECLARE_METATYPE(FileExportData)
Q_DECLARE_METATYPE(CaseObject)
Q_DECLARE_METATYPE(EvidenceObject)
Q_DECLARE_METATYPE(PartitionObject)
Q_DECLARE_METATYPE(VolumeObject)
Q_DECLARE_METATYPE(FileSystemObject)
Q_DECLARE_METATYPE(SelectedObject);

#endif // WOMBATVARIABLE_H
