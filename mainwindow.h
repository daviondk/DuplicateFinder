#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openFolderBtn_clicked();    
    void on_addFolderBtn_clicked();
    void on_deleteFolderBtn_clicked();
    void on_directoryList_itemSelectionChanged();
    void on_searchButton_clicked();
    void update_addFolderBtn();

private:
    bool check_subfolders(const QString &folder);


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
