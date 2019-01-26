#ifndef SEARCHERDIALOG_H
#define SEARCHERDIALOG_H

#include <QObject>
#include <QDialog>
#include <QThread>
#include <QTreeWidgetItem>
#include <set>


namespace Ui {
class SearcherDialog;
}

class SearcherDialog : public QDialog
{
    Q_OBJECT

public:
     explicit SearcherDialog(QList<QString> folders, QWidget *parent = nullptr);
    ~SearcherDialog();

private slots:
    void startSearch();    
    void check_deleteBtn();
    void on_deleteBtn_clicked();

    void on_resulTree_itemChanged(QTreeWidgetItem *item);

signals:


private:
    void setStatus(const QString status);
    QString getSha256(QString const& str);

private:
    Ui::SearcherDialog *ui;
    QThread *searchThread;
    QList<QString> folders;
    std::set<std::pair<QString, QTreeWidgetItem *>> remove_files;

};

#endif // SEARCHERDIALOG_H

