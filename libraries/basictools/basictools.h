#ifndef BASICTOOLS_H
#define BASICTOOLS_H

#include "main/interfaces.h"
#include "binviewwidget.h"
#include <QtPlugin>
#include <QObject>
#include <QtWidgets>
#include <sqlite3.h>
#include <QStringList>
#include <QFileDialog>

class BasicTools : public QObject, public BasicToolsInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "wombat.BasicToolsInterface" FILE "basictools.json")
    Q_INTERFACES(BasicToolsInterface)
    
public:
    //BasicTools Interface Functions
    QStringList toolboxViews() const;
    QStringList evidenceToolboxIcons() const;

    QWidget* setupToolBoxDirectoryTree() const;
    QWidget* setupToolBoxFileExtensionTree() const;
    QWidget* setupHexTab();
    QWidget* setupTxtTab() const;
    QWidget* setupColumnView() const;
    void LoadHexModel(QString tmpFilePath);

private:
    sqlite3_stmt *sqlStatement;
    int currentcaseid;
    BinViewModel* hexmodel;
    BinViewWidget* hexwidget;

};

#endif // BASICTOOLS_H
