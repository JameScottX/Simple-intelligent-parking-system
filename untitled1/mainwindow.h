#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "blt.h"
#include <QListWidgetItem>
#include <QPushButton>
#include <QSignalMapper>
#include <QPalette>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    char floor_num=0x00;
    char packing_num;

    QByteArray msg_bytes;     //此为发送字节


private:

    Ui::MainWindow *ui;
    BLT *blt = new BLT();
    bool eventFilter(QObject *target, QEvent *event);
    QPalette *buttoncolor = new QPalette();
    QList<QPushButton*>allLabel[2];
    short choosedButton_num=0;


private slots:

    void blt_if_Discover();
    void bltDevDiscover(const QBluetoothDeviceInfo &info);
    void scanFinshed();
    void itemActivated(QListWidgetItem *item);
    void bltConnected();
    void bltReadMsg();

    void blt_Refresh();
    void bltColsed();

private :
    void bltSendMsg(QByteArray *msg);

    bool blt_connected = false;

    unsigned char floor_parkState[2][24]={{0}};
    double password_[2][24];

    short parking_num =0;
    unsigned int parking_password=0;
    bool packing_state_flag = true;

private slots:

    void floor_state(int a);
    void packing_state(QString p);
    void go_packing();
    void packing_back();

    void put_num_password_();
    void getbike_sure();
    void getbike_back();

   private slots:
    void guiwei();
    void msgguiwei();
};

#endif // MAINWINDOW_H
