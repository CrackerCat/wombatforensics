#ifndef WOMBATDATABASE_H
#define WOMBATDATABASE_H

#include "wombatinclude.h"
#include "wombatvariable.h"
#include "wombatfunctions.h"

class WombatDatabase : public QObject
{
    Q_OBJECT
public:
    WombatDatabase(WombatVariable* wombatvarptr);
    void CreateAppDB(void);
    void CreateCaseDB(void);
    void OpenCaseDB(void);
    void OpenAppDB(void);
    void CloseAppDB(void);
    void CloseCaseDB(void);
    void InsertEvidenceObject(void);
    void GetEvidenceObject(void);
    void GetEvidenceObjects(void);
    void InsertVolumeObject(void);
    void GetVolumeObjects(void);
    void InsertPartitionObjects(void);
    void GetPartitionObjects(void);
    void InsertFileSystemObjects(void);
    void InsertFileObjects(void);
    void GetFileSystemObjects(void);
    void GetObjectValues(void);
    void GetRootInum(void);
    void GetRootNodes(void);

    ~WombatDatabase();

    int ReturnCaseCount(void);
    void InsertCase();
    void ReturnCaseNameList(void);
    void ReturnCaseID(void);

    QList<QSqlRecord> GetSqlResults(QString query, QVariantList invalues);
    int InsertSqlGetID(QString query, QVariantList invalues);
    void InsertSql(QString query, QVariantList invalues);
    void InsertSql(QString query);

signals:
    void DisplayError(QString errorNumber, QString errorType, QString errorValue);
private:
    WombatVariable* wombatptr;
};

#endif // WOMBATDATABASE_H
