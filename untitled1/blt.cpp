#include "blt.h"


BLT::BLT(QObject *parent) : QObject(parent)
{

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    localDevice = new QBluetoothLocalDevice();
    socket  =new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
//    Queue_blt mBLTQueue;
//    mBLTQueue.Queue_addData();
}

void BLT::bltScan(){

    if(localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff){
        localDevice->powerOn();
    }
    discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);
    discoveryAgent->start();

    emit emit_refresh();
}

void BLT::Queue_addData(QByteArray *get){

    if(mBLTQueue.Queue_WritePoint  == BUFFER_SIZE){
        mBLTQueue.Queue_WritePoint =0;
    }
    mBLTQueue.Queue_buffer[mBLTQueue.Queue_WritePoint] = *get;
    mBLTQueue.Queue_WritePoint++;
    mBLTQueue.Queue_DataSize++;
}

QByteArray BLT::Queue_readData(void){

    QByteArray temp;
    if(mBLTQueue.Queue_DataSize == 0){
        return 0x00;
    }
    if(mBLTQueue.Queue_ReadPoint == BUFFER_SIZE){
        mBLTQueue.Queue_ReadPoint =0;
    }
    temp =mBLTQueue.Queue_buffer[mBLTQueue.Queue_ReadPoint];
    mBLTQueue.Queue_DataSize--;
    mBLTQueue.Queue_ReadPoint++;
    return temp;
}


