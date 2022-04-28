#ifndef COMMITFILEWINDOW_H
#define COMMITFILEWINDOW_H

#include <QWidget>

namespace Ui {
class CommitFileWindow;
}

class CommitFileWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CommitFileWindow(QWidget *parent = nullptr);
    ~CommitFileWindow();

private slots:
    void on_chooseFileBtn_clicked();

    void on_commitBtn_clicked();

private:
    Ui::CommitFileWindow *ui;

    QList<QString> fileList;
    QList<qint64> fileSizeList;
};

#endif // COMMITFILEWINDOW_H
