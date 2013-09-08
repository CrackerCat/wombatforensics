#include "progresswindow.h"

ProgressWindow::ProgressWindow(QWidget *parent) : QDialog(parent), ui(new Ui::ProgressWindow)
{
    ui->setupUi(this);
    connect(ui->hideButton, SIGNAL(clicked()), this, SLOT(HideClicked()));
    ui->analysisTreeWidget->hideColumn(1);
    ui->msgTableWidget->setCurrentCell(-1, -1, QItemSelectionModel::NoUpdate);
    counter = 0;
}

ProgressWindow::~ProgressWindow()
{
    //this->close();
    //delete ui;
}

void ProgressWindow::HideClicked()
{
    this->hide();
    //need to call signal/slot to set mainwindow button slot to unchecked...
    emit HideProgressWindow(false);
}
void ProgressWindow::UpdateAnalysisState(QString analysisState) // analysisStateEdit
{
    ui->analysisStateEdit->setText(analysisState);
}

void ProgressWindow::UpdateFilesFound(QString filesFound) // filesFoundEdit
{
    ui->filesFoundEdit->setText(filesFound);
}

void ProgressWindow::UpdateFilesProcessed(QString filesProcessed) // filesProcessedEdit
{
    ui->filesProcessedEdit->setText(filesProcessed);
}

void ProgressWindow::UpdateAnalysisTree(int parentIndex, QTreeWidgetItem *child) // analysisTreeWidget
{
    ui->analysisTreeWidget->collapseAll();
    ui->analysisTreeWidget->topLevelItem(parentIndex)->addChild(child);
    //ui->analysisTreeWidget->expand(itemAt(parentIndex)) // look online next time i'm online
    ui->analysisTreeWidget->setCurrentItem(child);
}
void ProgressWindow::ClearTableWidget()
{
    ui->msgTableWidget->clearContents();
}

void ProgressWindow::UpdateMessageTable(int currow, QString msgType, QString msgValue) // msgTableWidget
{
    ui->msgTableWidget->setCurrentCell(currow, 0, QItemSelectionModel::NoUpdate);
    ui->msgTableWidget->setItem(ui->msgTableWidget->currentRow() + 1, 0, new QTableWidgetItem(msgType));
    ui->msgTableWidget->setItem(ui->msgTableWidget->currentRow() + 1, 1, new QTableWidgetItem(msgValue));
}

void ProgressWindow::UpdateProgressBar(int progressValue) // progressBar
{
    ui->progressBar->setValue(progressValue);
}

void ProgressWindow::StepProgress(void)
{
    counter++;
}
