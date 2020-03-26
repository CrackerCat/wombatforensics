#include "filecarvingdialog.h"
#include "ui_filecarvingdialog.h"

// Copyright 2013-2020 Pasquale J. Rinaldi, Jr.
// Distrubted under the terms of the GNU General Public License version 2

//FileCarvingDialog::FileCarvingDialog(QWidget *parent, qint64 curcheckcount, qint64 curlistcount) :
FileCarvingDialog::FileCarvingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileCarvingDialog)
{
    ui->setupUi(this);
    parentwidget = parent;
    connect(ui->processButton, SIGNAL(clicked()), this, SLOT(EnableProcess()));
    /*
    checkcount = curcheckcount;
    QString checktext = QString("Checked (") + QString::number(checkcount) + QString(")");
    listcount = curlistcount;
    QString listtext = QString("All (") + QString::number(listcount) + QString(")");
    ui->checkedFileRadioButton->setText(checktext);
    ui->listedFileRadioButton->setText(listtext);
    ui->processButton->setEnabled(false);
    ui->selectedFileRadioButton->setChecked(true);
    if(checkcount <= 0)
        ui->checkedFileRadioButton->setEnabled(false);
    else
        ui->checkedFileRadioButton->setEnabled(true);
    if(listcount <= 0)
        ui->listedFileRadioButton->setEnabled(false);
    else
        ui->listedFileRadioButton->setEnabled(true);
    ui->md5radiobutton->setEnabled(false);
    ui->sha1radiobutton->setEnabled(false);
    ui->sha256radiobutton->setEnabled(false);
    connect(ui->thumbnailcheckBox, SIGNAL(clicked(bool)), this, SLOT(EnableProcess(bool)));
    connect(ui->videocheckBox, SIGNAL(clicked(bool)), this, SLOT(EnableProcess(bool)));
    connect(ui->hashcheckbox, SIGNAL(clicked(bool)), this, SLOT(EnableProcess(bool)));
    connect(ui->processButton, SIGNAL(clicked()), this, SLOT(DigDeeperFiles()));
    */
}

FileCarvingDialog::~FileCarvingDialog()
{
    delete ui;
}

void FileCarvingDialog::PopulateFileTypes()
{
    QString homepath = QDir::homePath();
    homepath += "/.local/share/wombatforensics/";
    QFile ctypes(homepath + "carvetypes");
    QStringList categorylist;
    categorylist.clear();
    QStringList linelist;
    linelist.clear();
    if(!ctypes.isOpen())
        ctypes.open(QIODevice::ReadOnly | QIODevice::Text);
    if(ctypes.isOpen())
    {
        QTextStream in(&ctypes);
        //int rowcount = 0;
        // ui->filetypetree
        while(!in.atEnd())
        {
            //if(rowcount == trowcount)
                //ui->filetypestablewidget->setRowCount(trowcount + 5);
            QString tmpstr = in.readLine();
            //qDebug() << "each line:" << in.readLine();
            linelist.append(tmpstr);
            categorylist.append(tmpstr.split(",").first());
            //QStringList linelist = in.readLine().split(",");
            //for(int i=0; i < linelist.count(); i++)
            //{
                //ui->filetypestablewidget->setItem(rowcount, i, new QTableWidgetItem(linelist.at(i)));
            //}
            //qDebug() << "rowcount:" << rowcount;
            //rowcount++;
        }
    ctypes.close();
    }
    categorylist.removeDuplicates();
    //qDebug() << "categorylist:" << categorylist;
    //qDebug() << "linelist:" << linelist;
    // build treewidget
    for(int i=0; i < categorylist.count(); i++)
    {
        QTreeWidgetItem* tmpitem = new QTreeWidgetItem(ui->filetypetree);
        tmpitem->setText(0, categorylist.at(i));
        tmpitem->setCheckState(0, Qt::Unchecked);
        for(int j=0; j < linelist.count(); j++)
        {
            if(linelist.at(j).split(",").first().contains(categorylist.at(i)))
            {
                QTreeWidgetItem* tmpsubitem = new QTreeWidgetItem(tmpitem);
                tmpsubitem->setText(0, linelist.at(j).split(",").at(1));
                tmpsubitem->setCheckState(0, Qt::Unchecked);
            }
        }
    }
}

void FileCarvingDialog::PopulatePartitions(QStringList plist)
{
    for(int i=0; i < plist.count(); i++)
    {
	new QListWidgetItem(plist.at(i), ui->partitionlist);
    }
}

//void FileCarvingDialog::EnableProcess(bool checked)
void FileCarvingDialog::EnableProcess()
{
    this->close();
    //ui->processButton->setEnabled(checked);
    /*
    if(ui->hashcheckbox->isChecked())
    {
        ui->md5radiobutton->setEnabled(true);
        ui->sha1radiobutton->setEnabled(true);
        ui->sha256radiobutton->setEnabled(true);
    }
    if(ui->hashcheckbox->isChecked() || ui->thumbnailcheckBox->isChecked() || ui->videocheckBox->isChecked())
        ui->processButton->setEnabled(true);
    */
}

/*
void FileCarvingDialog::DigDeeperFiles()
{
    if(ui->selectedFileRadioButton->isChecked())
        digtype = 0;
    if(ui->checkedFileRadioButton->isChecked())
        digtype = 1;
    if(ui->listedFileRadioButton->isChecked())
        digtype = 2;
    if(ui->thumbnailcheckBox->isChecked() && !ui->videocheckBox->isChecked())
        digoptions.append(0);
    else if(!ui->thumbnailcheckBox->isChecked() && ui->videocheckBox->isChecked())
        digoptions.append(4);
    else if(ui->thumbnailcheckBox->isChecked() && ui->videocheckBox->isChecked())
        digoptions.append(5);
    if(ui->hashcheckbox->isChecked())
    {
        if(ui->md5radiobutton->isChecked())
            digoptions.append(1);
        else if(ui->sha1radiobutton->isChecked())
            digoptions.append(2);
        else if(ui->sha256radiobutton->isChecked())
            digoptions.append(3);
    }
    emit StartDig(digtype, digoptions);
    this->close();
}
*/
