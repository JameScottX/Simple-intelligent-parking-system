#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->label_quche->installEventFilter(this);
    ui->label_tinche->installEventFilter(this);
    ui->label_info->installEventFilter(this);
    ui->label_set->installEventFilter(this);
    ui->pushButton_back1->installEventFilter(this);
    ui->pushButton_back2->installEventFilter(this);
    ui->pushButton_back3->installEventFilter(this);

    ui->pushButton_bltclose->setEnabled(false);
    floor_state(0);
    if(blt->localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff){

        ui->pushButton_bltclose->setEnabled(false);

    }else{

        ui->pushButton_bltclose->setEnabled(true);
    }

    connect(ui->checkBox_bltdiscover,SIGNAL(stateChanged(int)),this,SLOT(blt_if_Discover()));
    connect(ui->pushButton_scanblt,SIGNAL(released()),blt,SLOT(bltScan()));
    connect(blt,SIGNAL(emit_refresh()),this,SLOT(blt_Refresh()));
    connect(blt->discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(bltDevDiscover(QBluetoothDeviceInfo)));
    connect(blt->discoveryAgent,SIGNAL(finished()),this,SLOT(scanFinshed()));

    connect(ui->listWidget,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(itemActivated(QListWidgetItem*)));
    connect(ui->pushButton_bltclose,SIGNAL(released()),this,SLOT(bltColsed()));
    connect(ui->pushButton_pback,SIGNAL(released()),this,SLOT(packing_back()));

    ui->pushButton_pback->setEnabled(false);
    ui->pushButton_psure->setEnabled(false);

    connect(ui->pushButton_1sure,SIGNAL(released()),this,SLOT(getbike_sure()));
    connect(ui->pushButton_1back,SIGNAL(released()),this,SLOT(getbike_back()));

    connect(ui->pushButton_guiwei,SIGNAL(released()),this,SLOT(guiwei()));
    connect(ui->pushButton_msgguiwei,SIGNAL(released()),this,SLOT(msgguiwei()));

 /***********************************************************************************************************************/
    buttoncolor->setColor(QPalette::Button,QColor(0,255,0));

    buttoncolor->setColor(QPalette::Background,QColor(0,255,0));

    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(floor_state(int)));


    QSignalMapper *pmap = new QSignalMapper();
    QSignalMapper *pmap2 = new QSignalMapper();

    allLabel[0] =  ui->widget_p1->findChildren<QPushButton*>();
    allLabel[1] =  ui->widget_p2->findChildren<QPushButton*>();

    for(int i =0;i<allLabel[0].size();i++){

        connect(allLabel[0][i],SIGNAL(released()),pmap,SLOT(map()));

        allLabel[0][i]->setAutoFillBackground(true);
        allLabel[0][i]->setPalette(*buttoncolor);

        pmap ->setMapping(allLabel[0][i],allLabel[0][i]->text());
    }

    connect(pmap,SIGNAL(mapped(QString)),this,SLOT(packing_state(QString)));


    for(int i =0;i<allLabel[1].size();i++){

        connect(allLabel[1][i],SIGNAL(released()),pmap2,SLOT(map()));

        allLabel[1][i]->setAutoFillBackground(true);
        allLabel[1][i]->setPalette(*buttoncolor);

        pmap2 ->setMapping(allLabel[1][i],allLabel[1][i]->text());
    }

    connect(pmap2,SIGNAL(mapped(QString)),this,SLOT(packing_state(QString)));

    connect(ui->pushButton_psure,SIGNAL(released()),this,SLOT(go_packing()));
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::guiwei(){

    msg_bytes.append(0xFA);
    msg_bytes.append(0x52);
    msg_bytes.append(0xFC);
    bltSendMsg(&msg_bytes);
    msg_bytes.clear();
}

void MainWindow::msgguiwei(){

    msg_bytes.append(0xFD);
    msg_bytes.append(0x25);
    msg_bytes.append(0xFC);
    bltSendMsg(&msg_bytes);
    msg_bytes.clear();

}

void MainWindow::put_num_password_(){

     parking_num = ui->lineEdit->text().toInt();
     parking_password = ui->lineEdit_2->text().toInt();
}

void MainWindow::getbike_sure(){

    unsigned char mFloor = parking_num/100;
    unsigned char mPacknum = parking_num - mFloor*100;
    char floor_tmp = mFloor-1;
    char pack_tmp =mPacknum-1;

    if(floor_parkState[floor_tmp][pack_tmp] ==0x01 && parking_num<224){

        if(password_[floor_tmp][pack_tmp] == parking_password){

            msg_bytes.append(0xFE);
            msg_bytes.append(0x73);
            msg_bytes.append(&floor_tmp);
            msg_bytes.append(&pack_tmp);
            msg_bytes.append(0xFC);
            bltSendMsg(&msg_bytes);
            msg_bytes.clear();

            buttoncolor->setColor(QPalette::Button,QColor(0,255,0));
            buttoncolor->setColor(QPalette::Background,QColor(0,255,0));

            for(int i =0;i<allLabel[floor_tmp].size();i++){

                if(allLabel[floor_tmp][i]->text() == QString::number(mPacknum)){

                   allLabel[floor_tmp][i]->setPalette(*buttoncolor);
                   floor_parkState[floor_tmp][pack_tmp] = 0x00;

                   break;
                }
            }
            QMessageBox *msg = new QMessageBox(QMessageBox::NoIcon,QStringLiteral("哈哈"),QStringLiteral("取车成功！"));
            msg->show();


        }else{

            QMessageBox *msg = new QMessageBox(QMessageBox::NoIcon,QStringLiteral("注意"),QStringLiteral("您的密码错误！请重新输入！"));
            msg->show();
        }
    }else{
        QMessageBox *msg_ = new QMessageBox(QMessageBox::NoIcon,QStringLiteral("注意"),QStringLiteral("您所选的停车位没有车辆！请确认输入格式！"));
        msg_->show();
        return ;
    }
}

void MainWindow::getbike_back(){

    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
}

void MainWindow::floor_state(int a){

    floor_num = (char)a;
}

void MainWindow::packing_state(QString p){

    char temp = p.toInt();
    if(packing_state_flag){

    if(floor_parkState[floor_num][temp-1] ==0x00){

      packing_num = temp-1;

    }else{

        QMessageBox *msg = new QMessageBox(QMessageBox::NoIcon,QStringLiteral("注意"),QStringLiteral("你选的位置已经停车！"));
        msg->show();
        qDebug()<<QString::number(password_[floor_num][temp-1]);
        return;
    }

    ui->pushButton_psure->setEnabled(true);
    buttoncolor->setColor(QPalette::Button,QColor(0,0,255));
    buttoncolor->setColor(QPalette::Background,QColor(0,0,255));

    for(int i =0;i<allLabel[floor_num].size();i++){

        if(allLabel[floor_num][i]->text() == p){

           allLabel[floor_num][i]->setPalette(*buttoncolor);

           choosedButton_num = i;
           ui->pushButton_pback->setEnabled(true);
           break;
        }else{
            continue;
        }
    }
    packing_state_flag = false;

    }
}


void MainWindow::go_packing(){

        msg_bytes.append(0xFF);
        msg_bytes.append(0x68);
        msg_bytes.append(&floor_num);
        msg_bytes.append(&packing_num);
        msg_bytes.append(0xFC);

        bltSendMsg(&msg_bytes);
        msg_bytes.clear();


        double code_;

        for(int i = 0;i<6;i++){

            code_+= ((rand()%10)*pow(10,i));

        }

        password_[floor_num][packing_num] = code_;

        QString temp = QString::number(code_);
        QString temp_ = QString(QStringLiteral("您的密码是："))+temp+QString(QStringLiteral("请牢记！"));

        QMessageBox *msg_box= new QMessageBox(QMessageBox::NoIcon,QStringLiteral("停车成功"),temp_);
        msg_box->show();

        buttoncolor->setColor(QPalette::Button,QColor(255,0,0));
        buttoncolor->setColor(QPalette::Background,QColor(255,0,0));

        allLabel[floor_num][choosedButton_num]->setPalette(*buttoncolor);

        floor_parkState[floor_num][packing_num] =0x01;

        ui->pushButton_pback->setEnabled(false);
        packing_state_flag =true;
        ui->pushButton_psure->setEnabled(false);

}

void MainWindow::packing_back(){


    buttoncolor->setColor(QPalette::Button,QColor(0,255,0));
    buttoncolor->setColor(QPalette::Background,QColor(0,255,0));

    allLabel[floor_num][choosedButton_num]->setPalette(*buttoncolor);
    ui->pushButton_pback->setEnabled(false);
    ui->pushButton_psure->setEnabled(false);
    packing_state_flag =true;
}

void MainWindow::blt_Refresh(){

    ui->pushButton_bltclose->setEnabled(true);
}


void MainWindow::blt_if_Discover(){

    if(ui->checkBox_bltdiscover->isChecked() == false){

        blt->localDevice->hostMode() ==QBluetoothLocalDevice::HostDiscoverable;

    }else{
        blt->localDevice->hostMode() ==QBluetoothLocalDevice::HostConnectable;
    }
}

void MainWindow::bltDevDiscover(const QBluetoothDeviceInfo &info){

    QString label = QString("%1 %2").arg(info.address().toString()).arg(info.name());
    QList<QListWidgetItem *> items = ui->listWidget->findItems(label, Qt::MatchExactly);
    if (items.empty()) {

        QListWidgetItem *item = new QListWidgetItem(label);
        QBluetoothLocalDevice::Pairing pairingStatus = blt->localDevice->pairingStatus(info.address());
        if (pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired )
            item->setTextColor(QColor(Qt::green));
        else
            item->setTextColor(QColor(Qt::black));

        ui->listWidget->addItem(item);
    }
}

void MainWindow::scanFinshed(){

    ui->textBrowser_infoback->setText(QStringLiteral("蓝牙扫描完成！：）么么哒"));
}

static const QLatin1String serviceUuid("00001101-0000-1000-8000-00805F9B34FB");

void MainWindow::itemActivated(QListWidgetItem *item){

    QString text = item->text();
    int index =text.indexOf(' ');
    if(index==-1){
        return;
    }

    QBluetoothAddress Address(text.left(index));
    QString name(text.mid(index + 1));
    QString temp = QString("%1").arg(Address.toString());
    ui->textBrowser_infoback->append(QStringLiteral("你链接的蓝牙地址是：")+temp);
    ui->textBrowser_infoback->append(QStringLiteral("你链接的蓝牙名字：")+name);
    blt->socket->connectToService(Address, QBluetoothUuid(serviceUuid) ,QIODevice::ReadWrite);
    connect(blt->socket,SIGNAL(connected()),this,SLOT(bltConnected()));
}

void MainWindow::bltConnected(){

    ui->textBrowser_infoback->append(QStringLiteral("成功链接！"));
    connect(blt->socket,SIGNAL(readyRead()),this,SLOT(bltReadMsg()));
    blt_connected  = true;
}

void MainWindow::bltColsed(){

    blt_connected = false;
    blt->socket->disconnectFromService();

    blt->localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);

    ui->pushButton_bltclose->setEnabled(false);

    blt-> discoveryAgent->stop();

    ui->listWidget->clear();
    ui->textBrowser_infoback->clear();
}

void MainWindow::bltReadMsg(){

    QByteArray get = blt->socket->readAll();
    QString temp = get.toHex();
   ui->textBrowser_infoback->setText(temp);
    //。。。
}

void MainWindow::bltSendMsg(QByteArray *msg){

    QMessageBox *msg_box= new QMessageBox(QMessageBox::NoIcon,QStringLiteral("注意"),QStringLiteral("请链接BLT"));

    if(blt_connected ==true){

      blt->socket->write(*msg);

    }else{

        msg_box->show();
        buttoncolor->setColor(QPalette::Button,QColor(0,255,0));
        buttoncolor->setColor(QPalette::Background,QColor(0,255,0));

        allLabel[floor_num][choosedButton_num]->setPalette(*buttoncolor);
        packing_state_flag =true;
    }
}

bool MainWindow::eventFilter(QObject *target, QEvent *event){

    if(target == ui->label_quche){
        if(event->type() == QEvent::MouseButtonRelease){
            ui->stackedWidget->setCurrentIndex(1);
            return true;
        }else{

            return false;
        }
    }else if( target ==ui->label_tinche){

        if(event->type() == QEvent::MouseButtonRelease){
            ui->stackedWidget->setCurrentIndex(2);
            return true;
        }else{

            return false;
        }
    }else if( target ==ui->label_set){

            if(event->type() == QEvent::MouseButtonRelease){
                ui->stackedWidget->setCurrentIndex(3);
                return true;
            }else{

                return false;
            }
    }else if( target ==ui->label_info){

        if(event->type() == QEvent::MouseButtonRelease){
            QMessageBox *msg = new QMessageBox(QMessageBox::NoIcon,QStringLiteral("我们的团队"),QStringLiteral("*机械：李喻 温涛 唐鹏辉 杨金龙   *软件，电子：崔俊文"));
            msg->show();
            return true;
        }else{

            return false;
        }
     } else if(target == ui->pushButton_back1 |target == ui->pushButton_back2|target == ui->pushButton_back3){

        if(event->type() == QEvent::MouseButtonRelease){
            ui->stackedWidget->setCurrentIndex(0);
            return true;
        }else{

            return false;
        }
    }

}
