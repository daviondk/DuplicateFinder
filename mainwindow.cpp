#include "mainwindow.h"
#include "searcherdialog.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->folderInput, &QLineEdit::textChanged, this, &MainWindow::update_addFolderBtn);
    update_addFolderBtn();
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::on_openFolderBtn_clicked()
{
    QString folder = QFileDialog::getExistingDirectory(this, "select directory", QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->folderInput->setText(folder);
}


void MainWindow::on_addFolderBtn_clicked()
{
    QString new_folder = QFileInfo(ui->folderInput->text()).absoluteFilePath();

    for(int i=0; i < ui->directoryList->count(); ++i)
        if(ui->directoryList->item(i)->text().startsWith(new_folder))
            delete ui->directoryList->takeItem(i--);

    ui->directoryList->addItem(new_folder);
    ui->addFolderBtn->setEnabled(false);
    ui->searchButton->setEnabled(true);
}

void MainWindow::update_addFolderBtn()
{
    QFileInfo file_info = QFileInfo(ui->folderInput->text());
    ui->addFolderBtn->setEnabled(file_info.isDir() && check_subfolders(file_info.absoluteFilePath()));
}

bool MainWindow::check_subfolders(const QString &folder) {
   for(int i=0; i < ui->directoryList->count(); ++i)
        if(folder.startsWith(ui->directoryList->item(i)->text()))
            return false;
    return true;
}

void MainWindow::on_deleteFolderBtn_clicked()
{
    foreach(QListWidgetItem * item, ui->directoryList->selectedItems())
        delete ui->directoryList->takeItem( ui->directoryList->row(item) );

    ui->searchButton->setEnabled(ui->directoryList->count() > 0);
    update_addFolderBtn();
}

void MainWindow::on_directoryList_itemSelectionChanged()
{
    ui->deleteFolderBtn->setEnabled(ui->directoryList->selectedItems().size() > 0);
}


void MainWindow::on_searchButton_clicked()
{
    QList<QString> list;
    for (int i = 0; i < ui->directoryList->count(); i++)
        list.push_back(ui->directoryList->item(i)->text());

    auto scannerWindow = new SearcherDialog(list, this);

    scannerWindow->exec();
}
