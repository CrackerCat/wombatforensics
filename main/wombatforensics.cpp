#include "interfaces.h"
#include "wombatforensics.h"
#include "wombatcasedb.h"

#include <QPluginLoader>

WombatForensics::WombatForensics(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WombatForensics)
{
    ui->setupUi(this);
    wombatCaseData = new WombatCaseDb(this);
    //connect(ui->actionNew_Case_2, SIGNAL(triggered()), ui->menuEvidence, SLOT(on_actionNew_Case_triggered()));
    //connect(ui->actionOpen_Case_2, SIGNAL(triggered()), ui->menuEvidence, SLOT(on_actionOpen_Case_triggered()));
    currentcaseid = -1;
    QString homePath = QDir::homePath();
    homePath += "/WombatForensics/";
    wombatsettingspath = homePath + "settings/";
    wombatdatapath = homePath + "data/";
    bool mkPath = (new QDir())->mkpath(wombatsettingspath);
    if(mkPath == false)
        wombatCaseData->DisplayError(this, "2.0", "App Settings Folder Failed.", "App Settings Folder was not created.");
    mkPath = (new QDir())->mkpath(wombatdatapath);
    if(mkPath == false)
        wombatCaseData->DisplayError(this, "2.1", "App Data Folder Failed.", "Application Data Folder was not created.");
    QString tmpPath = wombatdatapath + "WombatData.db";
    bool doesFileExist = wombatCaseData->FileExists(tmpPath.toStdString());
    if(!doesFileExist)
    {
        const char* errstring = wombatCaseData->CreateCaseDB(tmpPath);
        if(strcmp(errstring, "") != 0)
            wombatCaseData->DisplayError(this, "1.0", "File Error", errstring);
    }
    else
    {
        const char* errstring = wombatCaseData->OpenCaseDB(tmpPath);
        if(strcmp(errstring, "") != 0)
            wombatCaseData->DisplayError(this, "1.1", "SQL", errstring);
    }
    if(wombatCaseData->ReturnCaseCount() == 0)
    {
        ui->actionOpen_Case->setEnabled(false);
        ui->actionOpen_Case_2->setEnabled(false);
    }
    else if(wombatCaseData->ReturnCaseCount() > 0)
    {
        ui->actionOpen_Case->setEnabled(true);
        ui->actionOpen_Case_2->setEnabled(true);
    }
    else
    {
        wombatCaseData->DisplayError(this, "1.0", "Case Count", "Invalid Case Count returned.");
    }
    pluginFileNames.clear();
    pluginFileNames = locatePlugins();

    ui->menuEvidence->setEnabled(!ui->menuEvidence->actions().isEmpty());
    ui->menuSettings->setEnabled(!ui->menuSettings->actions().isEmpty());
}

bool WombatForensics::isPluginLoaded(QString pluginFileName)
{
    int pluginexists = 0;
    QString fileName;
    foreach(fileName, pluginFileNames)
    {
        if(fileName.compare(pluginFileName) == 0)
        {
            loadPlugin(pluginFileName);
            pluginexists++;
        }
    }
    if(pluginexists <= 0)
    {
        fprintf(stderr, "string compare failed.");
        return false;
    }
    else
        return true;
}
QStringList WombatForensics::locatePlugins()
{
    QStringList tmpList;
    tmpList.clear();
    QString tmpPath = qApp->applicationDirPath();
    tmpPath += "/plugins/";
    QDir plugDir = QDir(tmpPath);

    foreach(QString fileName, plugDir.entryList(QDir::Files))
    {
        tmpList.append(plugDir.absoluteFilePath(fileName));
    }
    return tmpList;
}

void WombatForensics::loadPlugin(QString fileName)
{
   QPluginLoader loader(fileName);
    QObject* plugin = loader.instance();
    if(!loader.isLoaded())
        fprintf(stderr, "%s\n", loader.errorString().toStdString().c_str());
    if (plugin)
    {
        populateActions(plugin);
        populateToolBox(plugin);
        populateTabWidget(plugin);
        setupSleuthKitProperties(plugin, "/home/pasquale/Projects/wombatforensics/build/data/tsk-config.xml");
        setupSleuthKitLog(plugin, "/home/pasquale/Projects/wombatforensics/build/data/tsk-log.xml");
        setupSleuthKitImgDb(plugin, "/home/pasquale/Projects/wombatforensics/build/data/");
        setupSleuthKitBlackboard(plugin);
        setupSleuthKitSchedulerQueue(plugin);
        setupSleuthKitFileManager(plugin);
        //sleuthKitLoadEvidence(plugin, "/home/pasquale/Projects/TestImages/1-extend-part/ext-part-test-2.dd"); done when add evidence
    }
}

void WombatForensics::populateActions(QObject *plugin)
{
    EvidenceInterface *iEvidence = qobject_cast<EvidenceInterface *>(plugin);
    if (iEvidence)
    {
        addActions(plugin, iEvidence->evidenceActions(), iEvidence->evidenceActionIcons(), ui->mainToolBar, ui->menuEvidence, SLOT(alterEvidence()));
    }
}

void WombatForensics::populateToolBox(QObject *plugin)
{
    BasicToolsInterface *iBasicTools = qobject_cast<BasicToolsInterface *>(plugin);
    if(iBasicTools)
    {
        ui->toolBox->addItem(iBasicTools->setupToolBoxDirectoryTree(), ((QStringList)iBasicTools->toolboxViews())[0]);
        ui->toolBox->addItem(iBasicTools->setupToolBoxFileExtensionTree(), ((QStringList)iBasicTools->toolboxViews())[1]);
    }
}

void WombatForensics::populateTabWidget(QObject *plugin)
{
    BasicToolsInterface *iBasicTools = qobject_cast<BasicToolsInterface *>(plugin);
    if(iBasicTools)
    {
        ui->fileViewTabWidget->addTab(iBasicTools->setupHexTab(), "Hex View");
        ui->fileViewTabWidget->addTab(iBasicTools->setupTxtTab(), "Text View");
    }
}

void WombatForensics::addActions(QObject *plugin, const QStringList &texts, const QStringList &icons, QToolBar *toolbar, QMenu *menu, const char *member, QActionGroup *actionGroup)
{
    for(int i = 0; i < texts.count(); i++)
    {
        QAction *action1 = new QAction(texts[i], plugin);
        QAction *action2 = new QAction(texts[i], plugin);
        action1->setIcon(QIcon(icons[i]));
        action2->setIcon(QIcon(icons[i]));
        connect(action1, SIGNAL(triggered()), this, member);
        connect(action2, SIGNAL(triggered()), this, member);
        toolbar->addAction(action1);
        menu->addAction(action2);

        if(actionGroup)
        {
            action1->setCheckable(true);
            action2->setCheckable(true);
            actionGroup->addAction(action1);
            actionGroup->addAction(action2);
        }
    }
}

void WombatForensics::alterEvidence()
{
    QAction *action = qobject_cast<QAction *>(sender());
    EvidenceInterface *iEvidence = qobject_cast<EvidenceInterface *>(action->parent());
    if(action->text() == tr("Add Evidence"))
        iEvidence->addEvidence(currentcaseid);
    else if(action->text() == tr("Remove Evidence"))
        iEvidence->remEvidence(currentcaseid);
}

WombatForensics::~WombatForensics()
{
    const char* errmsg = wombatCaseData->CloseCaseDB();
    delete ui;
}

void WombatForensics::on_actionNew_Case_triggered()
{
    int ret = QMessageBox::Yes;
    // determine if a case is open
    if(currentcaseid > 0) // a case is open, provide action dialog first
    {
        ret = QMessageBox::question(this, tr("Close Current Case"), tr("There is a case already open. Are you sure you want to close it?"), QMessageBox::Yes | QMessageBox::No);
    }
    if (ret == QMessageBox::Yes)
    {
        // create new case here
        bool ok;
        QString text = QInputDialog::getText(this, tr("New Case Creation"), "Enter Case Name: ", QLineEdit::Normal, "", &ok);
        if(ok && !text.isEmpty())
        {
            currentcaseid = wombatCaseData->InsertCase(text);

            QString tmpTitle = "Wombat Forensics - ";
            tmpTitle += text;
            this->setWindowTitle(tmpTitle); // update application window.
            // make Cases Folder
            QString userPath = QDir::homePath();
            userPath += "/Cases/";
            userPath += QString::number(currentcaseid);
            userPath += "-";
            userPath += text;
            userPath += "/";
            bool mkPath = (new QDir())->mkpath(userPath);
            if(mkPath == true)
            {
                currentcasedirpath = userPath;
            }
            else
            {
                wombatCaseData->DisplayError(this, "2.0", "Cases Folder Creation Failed.", "New Case folder was not created.");
            }
            if(wombatCaseData->ReturnCaseCount() > 0)
            {
                ui->actionOpen_Case->setEnabled(true);
                ui->actionOpen_Case_2->setEnabled(true);
            }
            isPluginLoaded("/home/pasquale/Projects/wombatforensics/build/plugins/libevidenceplugin.so"); // manually load evidence plugin
            isPluginLoaded("/home/pasquale/Projects/wombatforensics/build/plugins/libbasictoolsplugin.so"); // manually load basictools plugin
            isPluginLoaded("/home/pasquale/Projects/wombatforensics/build/plugins/libsleuthkitplugin.so"); // manually load sleuthkit plugin
            ui->menuEvidence->setEnabled(!ui->menuEvidence->actions().isEmpty());
            ui->menuSettings->setEnabled(!ui->menuSettings->actions().isEmpty());
        }
    }

}

void WombatForensics::on_actionOpen_Case_triggered()
{
    int ret = QMessageBox::Yes;
    // determine if a case is open
    if(currentcaseid > 0) // a case is open, provide action dialog first
    {
        ret = QMessageBox::question(this, tr("Close Current Case"), tr("There is a case already open. Are you sure you want to close it?"), QMessageBox::Yes | QMessageBox::No);
    }
    if (ret == QMessageBox::Yes)
    {
        // open case here
        QStringList caseList;
        caseList.clear();
        // populate case list here
        bool ok;
        QString item = QInputDialog::getItem(this, tr("Open Existing Case"), tr("Select the Case to Open: "), caseList, 0, false, &ok);
        if(ok && !item.isEmpty()) // open selected case
        {
           QString tmpTitle = "Wombat Forensics - ";
            tmpTitle += item;
            this->setWindowTitle(tmpTitle);
        }
    }
}
void WombatForensics::setupSleuthKitProperties(QObject *plugin, QString configFileName)
{
    SleuthKitInterface *iSleuthKit = qobject_cast<SleuthKitInterface *>(plugin);
    if(iSleuthKit)
    {
        iSleuthKit->SetupSystemProperties(configFileName);
    }
}
void WombatForensics::setupSleuthKitLog(QObject *plugin, QString logFileName)
{
    SleuthKitInterface *iSleuthKit = qobject_cast<SleuthKitInterface *>(plugin);
    if(iSleuthKit)
    {
        iSleuthKit->SetupSystemLog(logFileName);
    }
}
void WombatForensics::setupSleuthKitImgDb(QObject *plugin, QString imgDBPath)
{
    SleuthKitInterface *iSleuthKit = qobject_cast<SleuthKitInterface *>(plugin);
    if(iSleuthKit)
    {
        iSleuthKit->SetupImageDatabase(imgDBPath);
    }
}
void WombatForensics::setupSleuthKitBlackboard(QObject *plugin)
{
    SleuthKitInterface *iSleuthKit = qobject_cast<SleuthKitInterface *>(plugin);
    if(iSleuthKit)
    {
        iSleuthKit->SetupSystemBlackboard();
    }
}
void WombatForensics::setupSleuthKitSchedulerQueue(QObject *plugin)
{
    SleuthKitInterface *iSleuthKit = qobject_cast<SleuthKitInterface *>(plugin);
    if(iSleuthKit)
    {
        iSleuthKit->SetupSystemSchedulerQueue();
    }
}
void WombatForensics::setupSleuthKitFileManager(QObject *plugin)
{
    SleuthKitInterface *iSleuthKit = qobject_cast<SleuthKitInterface *>(plugin);
    if(iSleuthKit)
    {
        iSleuthKit->SetupSystemFileManager();
    }
}
void WombatForensics::sleuthKitLoadEvidence(QObject *plugin, QString evidencePath)
{
    SleuthKitInterface *iSleuthKit = qobject_cast<SleuthKitInterface *>(plugin);
    if(iSleuthKit)
    {
        iSleuthKit->OpenEvidence(evidencePath);
    }
}
