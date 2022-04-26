#ifndef FILERCVWINDOW_H
#define FILERCVWINDOW_H

#include <QWidget>
#include"filereceiver.h"

namespace Ui {
class FileRcvWindow;
}

class FileRcvWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FileRcvWindow(QWidget *parent = nullptr);
    ~FileRcvWindow();
    FileRcvWindow(QList<QString> fileList,QList<qint64> fileSizeList,FileReceiver *fileReceiver);
private slots:
    void updateFileRcvProgress(int n, qint64 sizeLeft);
    void on_openDirBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::FileRcvWindow *ui;
};

#endif // FILERCVWINDOW_H
