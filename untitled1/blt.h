#ifndef BLT_H
#define BLT_H

#include <QObject>
#include <QtBluetooth/qbluetoothglobal.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QtBluetooth/qbluetoothsocket.h>
#include <QtBluetooth/qbluetoothservicediscoveryagent.h>
#include <QByteArray>
#define BUFFER_SIZE 150

class BLT:public QObject
{
    Q_OBJECT

public:
    BLT(QObject *parent = nullptr);

    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothLocalDevice *localDevice;
    QBluetoothSocket *socket;

    struct Queue_blt
    {
        QByteArray Queue_buffer[BUFFER_SIZE];
        int Queue_ReadPoint =0;
        int Queue_WritePoint =0;
        int Queue_DataSize =0;

    }mBLTQueue;
    void Queue_addData(QByteArray *get);
    QByteArray Queue_readData(void);

public slots:
    void bltScan();
signals:
    void emit_refresh();

};

#endif // BLT_H
