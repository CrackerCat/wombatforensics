#ifndef WOMBATFORENSICS_H
#define WOMBATFORENSICS_H

#include <QMainWindow>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <QDir>
#include <QStringList>
#include <QActionGroup>
#include <QFileDialog>
#include <QObject>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <string>
#include <QString>
#include <QThreadPool>
#include <QBoxLayout>
#include <QtWidgets>
#include <QStringList>
#include <QSizePolicy>
#include <QFrame>
#include <QStatusBar>
#include <QtWebKitWidgets>
#include <QWebView>

#include "wombatvariable.h"
#include "wombatdatabase.h"
#include "ui_wombatforensics.h"
#include "progresswindow.h"
#include "exportdialog.h"
#include "sleuthkit.h"
#include "hexEditor.hpp"
#include "translate.hpp"

namespace Ui {
class WombatForensics;
}

class WombatForensics : public QMainWindow
{
    Q_OBJECT

public:
    explicit WombatForensics(QWidget *parent = 0);
    ~WombatForensics();
    WombatDatabase *wombatcasedata;
    WombatVariable wombatvariable;
    ProgressWindow* wombatprogresswindow;
    ExportDialog* exportdialog;
    SleuthKitPlugin* isleuthkit;

signals:
    void LogVariable(WombatVariable wombatVariable);
private slots:
    void AddEvidence();
    void RemEvidence();
    void ExportEvidence();
    void on_actionNew_Case_triggered();
    void on_actionOpen_Case_triggered();
    void on_actionView_Progress_triggered(bool checked);
    void UpdateProgress(int count, int processcount);
    void UpdateMessageTable();
    void GetImageNode(QStandardItem* imageNode);
    void dirTreeView_selectionChanged(const QModelIndex &index);
    void HideProgressWindow(bool checkstate);
    void DisplayError(QString errorNumber, QString errorType, QString errorValue);
    void PopulateProgressWindow(WombatVariable wvariable);
    void UpdateCaseData(WombatVariable wvariable);
    void ResizeColumns(QStandardItemModel* currentmodel);
    void ResizeViewColumns(const QModelIndex &index)
    {
        ResizeColumns((QStandardItemModel*)index.model());
    }
    void FileExport(FileExportData exportdata);
    void setScrollBarRange(off_t low, off_t high);
    void setScrollBarValue(off_t pos);
    void setOffsetLabel(off_t pos);
    void UpdateSelectValue(const QString &txt);
    void LoadFileContents(QString filepath);

protected:
    void closeEvent(QCloseEvent* event);
private:
    Ui::WombatForensics *ui;

    void SetupDirModel(void);
    void SetupHexPage();
    void InitializeSleuthKit();
    void InitializeAppStructure();
    void RemoveTmpFiles();
    
    void LoadHexModel(QString tmpFilePath);
    void LoadTxtContent(QString asciiText);
    void LoadOmniContent(QString filePath);

    QStandardItem* GetCurrentImageDirectoryTree(QObject *plugin, QString imageDbPath, QString imageName);
    std::string GetTime(void);
    QThreadPool *threadpool;
    int StandardItemCheckState(QStandardItem* tmpitem, int checkcount);
    int StandardItemListCount(QStandardItem* tmpitem, int listcount);
    std::vector<FileExportData> SetFileExportProperties(QStandardItem* tmpitem, FileExportData tmpexport, std::vector<FileExportData>);
    std::vector<FileExportData> SetListExportProperties(QStandardItem* tmpitem, FileExportData tmpexport, std::vector<FileExportData>);
    int DetermineOmniView(QString currentSignature);
    //QTextEdit* currenttxtwidget; // replace with a txt version of the hexeditor
    QModelIndex curselindex;

    off_t offset() const;
    HexEditor* hexwidget;
    QScrollBar* hexvsb;
    QStatusBar* hstatus;
    QLabel* selectedoffset;
    QLabel* selectedhex;
    QLabel* selectedascii;
    QLabel* selectedinteger;
    QLabel* selectedfloat;
    QLabel* selecteddouble;

    QStandardItemModel* currenttreemodel;
    QStandardItemModel* wombatdirmodel;
    QStandardItemModel* wombattypmodel;
};

class ThreadRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:
    ThreadRunner(QObject* object, QString input, WombatVariable wVariable)
    {
        method = input;
        caller = (SleuthKitPlugin*)object;
        wombatvariable = wVariable;
    };
    void run()
    {
        if(method.compare("initialize") == 0)
            caller->Initialize(wombatvariable);
        if(method.compare("openevidence") == 0)
            caller->OpenEvidence(wombatvariable);
        if(method.compare("populatecase") == 0)
            caller->PopulateCase(wombatvariable);
        if(method.compare("showfile") == 0)
            caller->ShowFile(wombatvariable);
        if(method.compare("refreshtreeviews") == 0)
            caller->RefreshTreeViews(wombatvariable);
        if(method.compare("exportfiles") == 0)
            caller->ExportFiles(wombatvariable);
    };
private:
    QString method;
    SleuthKitPlugin* caller;
    WombatVariable wombatvariable;
};

#endif // WOMBATFORENSICS_H
