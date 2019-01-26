#include "searcherdialog.h"
#include "ui_searcherdialog.h"

#include <QThread>
#include <QDirIterator>
#include <QTreeWidget>
#include <QCryptographicHash>
#include <QDialog>
#include <QMessageBox>
#include <QCheckBox>

SearcherDialog::SearcherDialog(QList<QString> folders, QWidget *parent) :
    QDialog(parent),    
    ui(new Ui::SearcherDialog),
    folders(folders)
{

    searchThread = new QThread();
    connect(searchThread, SIGNAL(started()), this, SLOT(startSearch()));
    searchThread->start();

    ui->setupUi(this);

    connect(ui->resulTree, SIGNAL(itemSelectionChanged()), this, SLOT(check_deleteBtn()));
}

SearcherDialog::~SearcherDialog()
{
    delete ui;
    if(searchThread->isRunning())
        searchThread->exit();
    delete searchThread;
}

void SearcherDialog::startSearch(){
    QMap<qint64, QList<QString>> size_map;

    int num = 0;

    ui->progressBar->setRange(0, folders.size());

    for (auto &folder : folders) {
        setStatus("fetching files in:\n " + folder);
        QDirIterator it(folder, QDir::NoDotAndDotDot|QDir::Hidden|QDir::Files|QDir::NoSymLinks , QDirIterator::Subdirectories);
        while(it.hasNext()) {
            QString file_name = it.next();
            if (QFileInfo(file_name).isSymLink())
                continue;

            qint64 size = QFileInfo(file_name).size();
            auto cur = size_map.find(size);
            if (cur != size_map.end()) {
                cur->push_back(file_name);
            } else {
                QList<QString> new_list;
                new_list.push_back(file_name);
                size_map.insert(size, new_list);
            }
        }
        ui->progressBar->setValue(++num);
    }

    num = 0;
    int groups = 0;
    ui->progressBar->setRange(0, size_map.size());


    for (auto list : size_map) {
        if (list.size() > 1) {
            QMap<QString, QList<QString>> hash_map;
            setStatus ("calculate hashes of a group \n" +
                QString::number(num) + " / " +QString::number(list.size()));

            for (QString file_name : list) {

                QString hash = getSha256(file_name);
                if (hash == "") {
                    continue;
                }

                auto it = hash_map.find(hash);
                if (it != hash_map.end()) {
                    it->push_back(file_name);
                } else {
                    QList<QString> new_list;
                    new_list.push_back(file_name);
                    hash_map.insert(hash, new_list);
                }
            }

            for (auto files : hash_map) {
                if (files.size() > 1) {
                    QTreeWidgetItem* parent = new QTreeWidgetItem();
                    parent->setText(0, "group " + QString::number(++groups) + " ( "
                                    + QString::number(files.size()) + " / " +
                                    QString::number(QFileInfo(files[0]).size()) +"b)" );
                    for (QString file : files) {
                        QTreeWidgetItem* item = new QTreeWidgetItem();
                        item->setText(0, file);
                        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                        item->setCheckState(0, Qt::Unchecked);
                        parent->insertChild(0, item);
                    }
                    ui->resulTree->addTopLevelItem(parent);
                }
            }
        }
        ui->progressBar->setValue(++num);
    }
    setStatus ("search finished");
}

void SearcherDialog::setStatus(const QString status) {
    ui->statusDescription->setText(status);
}

QString SearcherDialog::getSha256(QString const& str){
    QFile file(str);
    if (file.open(QIODevice::ReadOnly)) {
        QCryptographicHash crypt (QCryptographicHash::Sha3_256);
        if (crypt.addData(&file))
            return QString(crypt.result().toHex());
    }
    return "";
}

void SearcherDialog::check_deleteBtn() {
    ui->deleteBtn->setEnabled(remove_files.size() != 0);
}

void SearcherDialog::on_deleteBtn_clicked()
{
    if (remove_files.size() == 0)
        return;

    ui->deleteBtn->setEnabled(false);
    static bool skip_error = false;

    for (auto item : remove_files) {
        QString filename = item.first;
        QFile file(item.first);

        if (file.permissions().testFlag(QFileDevice::WriteUser) && file.remove()) {
            QTreeWidgetItem* parent_item = item.second->parent();
            parent_item->removeChild(item.second);
            if (parent_item->childCount() == 0) {
                ui->resulTree->invisibleRootItem()->removeChild(parent_item);
            }
            remove_files.erase({item.first, item.second});
        } else {
            if (skip_error)
                continue;
            QMessageBox* m = new QMessageBox(QMessageBox::Warning, QString("Error"),
                    QString("failed to delete file: \n" + item.first), QMessageBox::StandardButtons(QMessageBox::Ignore),this);
            QCheckBox* check = new QCheckBox("don't show this error again", m);
            m->setCheckBox(check);
            if (m->exec() == QMessageBox::Ignore) {
                skip_error = (m->checkBox()->checkState() == Qt::Checked);
            }
            ui->deleteBtn->setEnabled(true);
        }
    }
}

void SearcherDialog::on_resulTree_itemChanged(QTreeWidgetItem *item)
{
    QString file_name = item->text(0);

    if (item->checkState(0) == Qt::Checked) {
        remove_files.emplace(file_name, item);
    } else if (remove_files.find({file_name, item}) != remove_files.end()) {
        remove_files.erase(remove_files.find({file_name, item}));
    }

    ui->deleteBtn->setEnabled(remove_files.size());
}
