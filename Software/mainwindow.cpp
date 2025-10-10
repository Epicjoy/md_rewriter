#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

// Define a serial port pointer
QSerialPort *COM = new QSerialPort();

void delay(int milliseconds) {
    QThread::msleep(milliseconds);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , receivedDataLength(0)
    , address(0)
    , waitingForResponse(false)
{
    ui->setupUi(this);
}


MainWindow::~MainWindow()
{
    if (COM->isOpen())
        {
            COM->close();
        }
    delete ui;
}


void MainWindow::on_shuaxin_clicked()
{
    ui->jindutiao->setValue(0);
    jindu=0;
    ui->comliebiao->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())//Get available serial ports
       {
        ui->comliebiao->addItem(info.portName());
       }

     connect(COM, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
}


void MainWindow::on_lianjie_clicked()
{
    ui->jindutiao->setValue(0);
    jindu = 0;
    ui->xinxi->clear();
    if (COM->isOpen())
    {
       COM->close();
       ui->lianjie->setText("connect");
       ui->xinxi->append("Device disconnected");
    }
    else
    {
        COM->setPortName(ui->comliebiao->currentText());
        COM->setBaudRate(1000000);
        COM->setDataBits(QSerialPort::Data8);
        COM->setParity(QSerialPort::NoParity);
        COM->setStopBits(QSerialPort::OneStop);
        COM->setFlowControl(QSerialPort::NoFlowControl);
        if (COM->open(QIODevice::ReadWrite))
        {
            ui->lianjie->setText("disconnect");
            ui->xinxi->append("Device connected\r\n");
            const uint8_t controlByte = 0x0C;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
        }
        else
        {
            ui->xinxi->append("Unable to connect to device\r\n");
        }
     }
}

void MainWindow::on_qingkong_clicked()
{
    ui->jindutiao->setValue(0);
    jindu=0;
    ui->xinxi->clear();
}

void MainWindow::readSerialData()
{
   QByteArray response = COM->readAll();
   QString str_rev;
   if(response.size()>256){
       if (file.isOpen()) {
            file.write(response);
            QString selectedItem = ui->flashsize->currentText();
            if(selectedItem == "512K"){
                jindu=jindu+1;
                ui->jindutiao->setValue(jindu/5.12+1);
                }
            else if(selectedItem == "1M"){
                jindu=jindu+1;
                ui->jindutiao->setValue(jindu/10.24+1);
                }
            else if(selectedItem == "2M"){
                jindu=jindu+1;
                ui->jindutiao->setValue(jindu/20.48+1);
                }
            else if(selectedItem == "128K"){
                jindu=jindu+1;
                ui->jindutiao->setValue(jindu/1.28+1);
                }
            else if(selectedItem == "256K"){
                jindu=jindu+1;
                ui->jindutiao->setValue(jindu/2.56+1);
                }
            else{
                jindu=jindu+1;
                ui->jindutiao->setValue(jindu/40.96+1);
                }
           }
       }
   else{
       if (response.contains("OK\r\n")) {
           if (!waitingForResponse) {
               return;
           }
           //str_rev = QString(response);
           //ui->xinxi->insertPlainText(str_rev);
           //ui->xinxi->moveCursor(QTextCursor::End);
           waitingForResponse = false;
           address += 1024;
           addj = addj+1;
           cnt = cnt+1;
           if(cnt==0x40){
               bank=bank+1;
               addj = 0;
               cnt = 0;
           }
           sendData();
       }
       else if (response.contains("GK\r\n")) {
           if (!waitingForResponse) {
               return;
           }
           //str_rev = QString(response);
           //ui->xinxi->insertPlainText(str_rev);
           //ui->xinxi->moveCursor(QTextCursor::End);
           waitingForResponse = false;
           address += 1024;
           addj = addj+1;
           cnt = cnt+1;
           if(cnt==0x40){
               bank=bank+1;
               addj = 0;
               cnt = 0;
           }
           sendsramData();
       }
       else{
               str_rev = QString(response);
               ui->xinxi->insertPlainText(str_rev);
               ui->xinxi->moveCursor(QTextCursor::End);
               if (response.contains("s\r\n")){
                   jindu=jindu+1;
                   ui->jindutiao->setValue(jindu*2.2);
                   }
               if (response.contains("FINISH!!!\r\n")){
                   ui->jindutiao->setValue(100);
                   if (isSaving) {
                           file.close();
                           isSaving = false;
                           ui->xinxi->append("File saved\r\n");
                       }
                   }
            }
       }
    //QString hexString = response.toHex(' ').toUpper();
    //ui->shujuxianshi->append(hexString);
}

void MainWindow::on_shibiekadai_clicked()
{
    ui->jindutiao->setValue(0);
    jindu=0;
    ui->xinxi->clear();
    if (COM->isOpen()) {
            const uint8_t controlByte = 0x0D;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
        } else {
            ui->xinxi->append("Device not connected\r\n");
        }
}

void MainWindow::on_cachu_clicked()
{
    ui->jindutiao->setValue(0);
    jindu=0;
    ui->xinxi->clear();
    if (COM->isOpen()) {
            const uint8_t controlByte = 0x0E;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
        } else {
            ui->xinxi->append("Device not connected\r\n");
        }

}


void MainWindow::on_shaolu_clicked()
{
    ui->jindutiao->setValue(0);
    jindu=0;
    ui->xinxi->clear();
    if (COM->isOpen()){
            QString fileName = QFileDialog::getOpenFileName(this, tr("Please select the ROM file to Write"), "", tr("MDROM files (*.bin *.md *.smd);;All files (*.*)"));
               if (!fileName.isEmpty()) {
                   file.setFileName(fileName);
                   if (file.open(QIODevice::ReadOnly)) {
                       fileSize = file.size();
                       address = 0x0;
                       addj = 0x0;
                       cnt = 0x0;
                       bank = 0x0;
                       sendData();
                       }
           } else {
                ui->xinxi->append("File open failed!\r\n");
           }
    } else {
        ui->xinxi->append("Device not connected!\r\n");
    }
}


void MainWindow::sendData()
{
    if(fileSize==0x80000){
        jindu=jindu+1;
        ui->jindutiao->setValue(jindu/5.12+1);
         if(address >= 0x80000){
            const uint8_t controlByte = 0x0F;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
            file.close();
            ui->xinxi->append("Game burning completed\r\n");
            return;
            }
        }
    else if(fileSize==0x100000){
        jindu=jindu+1;
        ui->jindutiao->setValue(jindu/10.24+1);
        if(address >= 0x100000){
            const uint8_t controlByte = 0x0F;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
            file.close();
            ui->xinxi->append("Game burning completed\r\n");
            return;
            }
        }
    else if(fileSize==0x200000){
        jindu=jindu+1;
        ui->jindutiao->setValue(jindu/20.48+1);
        if(address >= 0x200000){
            const uint8_t controlByte = 0x0F;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
            file.close();
            ui->xinxi->append("Game burning completed\r\n");
            return;
            }
        }
    else if(fileSize==0x300000){
        jindu=jindu+1;
        ui->jindutiao->setValue(jindu/30.72+1);
        if(address >= 0x300000){
            const uint8_t controlByte = 0x0F;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
            file.close();
            ui->xinxi->append("Game burning completed\r\n");
            return;
            }
        }
    else if(fileSize==0x400000){
        jindu=jindu+1;
        ui->jindutiao->setValue(jindu/40.96+1);
        if(address >= 0x400000){
            const uint8_t controlByte = 0x0F;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
            file.close();
            ui->xinxi->append("Game burning completed\r\n");
            return;
            }
        }
    else {
        ui->jindutiao->setValue(0);
            const uint8_t controlByte = 0x0F;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
            file.close();
            ui->xinxi->append("The file size is not supported by the system\r\n");
            return;
        }
    const uint8_t controlByte = 0x0B;
    const uint8_t controlByte1 = 0xAA;
    const uint8_t controlByte2 = 0x55;
    const uint8_t controlByte3 = 0xBB;
    QByteArray data = file.read(1024);
    COM->write(data);
    QByteArray packet = 0;
    packet.append(controlByte);
    packet.append(controlByte1);
    packet.append(controlByte2);
    packet.append(controlByte1);
    packet.append(controlByte3);
    packet.append(addj&0xff);
    packet.append(bank&0xff);
    COM->write(packet);
    waitingForResponse = true;
}

void MainWindow::sendsramData()
{
   if(fileSize==0x2000){
        jindu=jindu+1;
        ui->jindutiao->setValue(jindu/0.08+1);
        if(address >= 0x2000){
            const uint8_t controlByte = 0x0F;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
            file.close();
            ui->xinxi->append("Archive burning completed\r\n");
            return;
            }
        }
    else if(fileSize==0x8000){
        jindu=jindu+1;
        ui->jindutiao->setValue(jindu/0.32+1);
        if(address >= 0x8000){
            const uint8_t controlByte = 0x0F;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
            file.close();
            ui->xinxi->append("Archive burning completed\r\n");
            return;
            }
        }
    else {
        ui->jindutiao->setValue(0);
            const uint8_t controlByte = 0x0F;
            const uint8_t controlByte1 = 0xAA;
            const uint8_t controlByte2 = 0x55;
            const uint8_t controlByte3 = 0xBB;
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            COM->write(packet);
            file.close();
            ui->xinxi->append("The file size is not supported by the system\r\n");
            return;
        }
    const uint8_t controlByte = 0x1B;
    const uint8_t controlByte1 = 0xAA;
    const uint8_t controlByte2 = 0x55;
    const uint8_t controlByte3 = 0xBB;
    QByteArray data = file.read(1024);
    COM->write(data);
    QByteArray packet = 0;
    packet.append(controlByte);
    packet.append(controlByte1);
    packet.append(controlByte2);
    packet.append(controlByte1);
    packet.append(controlByte3);
    packet.append(addj&0xff);
    packet.append(bank&0xff);
    COM->write(packet);
    waitingForResponse = true;
}


QByteArray MainWindow::hexStringToByteArray(const QString &hex)
{
    QByteArray byteArray;
    QStringList hexList = hex.split(" ");
    for (const QString &hexByte : hexList) {
        bool ok;
        byteArray.append(static_cast<char>(hexByte.toInt(&ok, 16)));
    }
    return byteArray;
}


void MainWindow::on_duqu_clicked()
{
    ui->jindutiao->setValue(0);
    jindu=0;
    QString fileName;
    ui->xinxi->clear();
    if (COM->isOpen()){
       fileName = QFileDialog::getSaveFileName(this, tr("Please select the ROM file to burn"), "", tr("MDROM files (*.bin);;All files (*.*)"));
       if (!fileName.isEmpty()) {
           file.setFileName(fileName);
           if (file.open(QIODevice::WriteOnly)) {
               isSaving = true;
               const uint8_t controlByte = 0x0A;
               const uint8_t controlByte1 = 0xAA;
               const uint8_t controlByte2 = 0x55;
               const uint8_t controlByte3 = 0xBB;
               const uint8_t FLASHSIZE512K = 0x01;
               const uint8_t FLASHSIZE1M = 0x02;
               const uint8_t FLASHSIZE2M = 0x03;
               const uint8_t FLASHSIZE4M = 0x04;
               QString selectedItem = ui->flashsize->currentText();
               if(selectedItem == "512K"){
                   QByteArray packet;
                   packet.append(controlByte);
                   packet.append(controlByte1);
                   packet.append(controlByte2);
                   packet.append(controlByte1);
                   packet.append(controlByte3);
                   packet.append(FLASHSIZE512K);
                   COM->write(packet);
               }
               if(selectedItem == "1M"){
                   QByteArray packet;
                   packet.append(controlByte);
                   packet.append(controlByte1); // 24-bit address high byte
                   packet.append(controlByte2); // 24-bit address high byte
                   packet.append(controlByte1); // 24-bit address high byte
                   packet.append(controlByte3);  // 24-bit address middle byte
                   packet.append(FLASHSIZE1M);
                   COM->write(packet);
               }
               if(selectedItem == "2M"){
                   QByteArray packet;
                   packet.append(controlByte);
                   packet.append(controlByte1); // 24-bit address high byte
                   packet.append(controlByte2); // 24-bit address high byte
                   packet.append(controlByte1); // 24-bit address high byte
                   packet.append(controlByte3);  // 24-bit address middle byte
                   packet.append(FLASHSIZE2M);
                   COM->write(packet);
               }
               if(selectedItem == "4M"){
                   QByteArray packet;
                   packet.append(controlByte);
                   packet.append(controlByte1); // 24-bit address high byte
                   packet.append(controlByte2); // 24-bit address high byte
                   packet.append(controlByte1); // 24-bit address high byte
                   packet.append(controlByte3);  // 24-bit address middle byte
                   packet.append(FLASHSIZE4M);
                   COM->write(packet);
               }
           } else {
               ui->xinxi->append("Error opening file\r\n");
           }

       }
       else{
         ui->xinxi->append("No file selected\r\n");
       }
    } else {
        ui->xinxi->append("Device not connected\r\n");
    }

}

void MainWindow::on_jiaoyanwenjian_clicked()
{
    ui->jindutiao->setValue(0);
    jindu=0;
    ui->xinxi->clear();
    filePath1 = QFileDialog::getOpenFileName(this, "Select file 1", "", "All files (*.*)");
    ui->xinxi-> append(filePath1);
    ui->xinxi-> append("File Opened\r\n");
    delay(100);
    filePath2 = QFileDialog::getOpenFileName(this, "Select file 2", "", "All files (*.*)");
    ui->xinxi-> append(filePath2);
    ui->xinxi-> append("File Opened\r\n");
    delay(100);
    if (filePath1.isEmpty() || filePath2.isEmpty()) {
            ui->xinxi->append("Please select two files for comparison first.\r\n");
            return;
        }
        QByteArray fileContent1 = readFile(filePath1);
        QByteArray fileContent2 = readFile(filePath2);
        compareFiles(fileContent1, fileContent2);
}


QByteArray MainWindow::readFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }

    QByteArray fileContent = file.readAll();
    file.close();

    return fileContent;
}

void MainWindow::compareFiles(const QByteArray &fileContent1, const QByteArray &fileContent2)
{
    QString result;
    uint8_t cnt = 0;
    int maxSize = qMax(fileContent1.size(), fileContent2.size());
    for (int i = 0; i < maxSize; ++i) {
        char byte1 = (i < fileContent1.size()) ? fileContent1[i] : 0;
        char byte2 = (i < fileContent2.size()) ? fileContent2[i] : 0;
        if (byte1 != byte2) {
            result += QString("Different byte position: %1\n").arg(i);
            cnt=cnt+1;
        }
    }

    if (result.isEmpty()) {
        result = "The contents of the two files are the same.\r\n";
    }

    if(cnt<=20){
        ui->xinxi->append(result);
    }

    else{
        ui->xinxi->append("More than 20 differences, not listing them all\r\n");
    }
}

void MainWindow::on_rongliangcachu_clicked()
{
    ui->jindutiao->setValue(0);
    jindu=0;
    ui->xinxi->clear();
    if (COM->isOpen()){
        const uint8_t controlByte = 0x1E;
        const uint8_t controlByte1 = 0xAA;
        const uint8_t controlByte2 = 0x55;
        const uint8_t controlByte3 = 0xBB;
        const uint8_t FLASHSIZE512K = 0x01;
        const uint8_t FLASHSIZE1M = 0x02;
        const uint8_t FLASHSIZE2M = 0x03;
        const uint8_t FLASHSIZE4M = 0x04;
        const uint8_t FLASHSIZE128K = 0x05;
        const uint8_t FLASHSIZE256K = 0x06;
        QString selectedItem = ui->flashsize->currentText();
        if(selectedItem == "128K"){
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1);
            packet.append(controlByte2);
            packet.append(controlByte1);
            packet.append(controlByte3);
            packet.append(FLASHSIZE128K);
            COM->write(packet);
        }
        if(selectedItem == "256K"){
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1);
            packet.append(controlByte2);
            packet.append(controlByte1);
            packet.append(controlByte3);
            packet.append(FLASHSIZE256K);
            COM->write(packet);
        }
        if(selectedItem == "512K"){
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1);
            packet.append(controlByte2);
            packet.append(controlByte1);
            packet.append(controlByte3);
            packet.append(FLASHSIZE512K);
            COM->write(packet);
        }
        if(selectedItem == "1M"){
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            packet.append(FLASHSIZE1M);
            COM->write(packet);
        }
        if(selectedItem == "2M"){
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            packet.append(FLASHSIZE2M);
            COM->write(packet);
        }
        if(selectedItem == "4M"){
            QByteArray packet;
            packet.append(controlByte);
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte2); // 24-bit address high byte
            packet.append(controlByte1); // 24-bit address high byte
            packet.append(controlByte3);  // 24-bit address middle byte
            packet.append(FLASHSIZE4M);
            COM->write(packet);
        }
      } else {
       ui->xinxi->append("Device not connected\r\n");
    }
}



void MainWindow::on_shaolucundang_clicked()
{
    ui->jindutiao->setValue(0);
    jindu = 0;
    ui->xinxi->clear();
    if (COM->isOpen()){
            QString fileName = QFileDialog::getOpenFileName(this, tr("Please select the Save file to Write"), "", tr("SAV files (*.srm *.sav);;All files (*.*)"));
               if (!fileName.isEmpty()) {
                   file.setFileName(fileName);
                   if (file.open(QIODevice::ReadOnly)) {
                       fileSize = file.size();
                       address = 0x0;
                       addj = 0x0;
                       cnt = 0x0;
                       bank = 0x0;
                       sendsramData();
                       }
                    }
           else {
                ui->xinxi->append("File Open Failed\r\n");
           }
    } else {
        ui->xinxi->append("Device not connected\r\n");
    }
}

void MainWindow::on_duqucundang_clicked()
{
    ui->jindutiao->setValue(0);
    jindu=0;
    ui->xinxi->clear();
    if (COM->isOpen()){
            QString fileName = QFileDialog::getSaveFileName(this, tr("Please select the file to Save"), "", tr("SAV files (*.srm);;All files (*.*)"));
            if (!fileName.isEmpty())
                {
                    file.setFileName(fileName);
                    if (file.open(QIODevice::WriteOnly))
                        {
                            isSaving = true;
                            const uint8_t controlByte = 0x1A;
                            const uint8_t controlByte1 = 0xAA;
                            const uint8_t controlByte2 = 0x55;
                            const uint8_t controlByte3 = 0xBB;
                            const uint8_t SRAMSIZE256K = 0x01;
                            const uint8_t SRAMSIZE64K = 0x02;
                            QString selectedItem = ui->sramsize->currentText();
                            if(selectedItem == "8K")
                                {
                                    QByteArray packet;
                                    packet.append(controlByte);
                                    packet.append(controlByte1);
                                    packet.append(controlByte2);
                                    packet.append(controlByte1);
                                    packet.append(controlByte3);
                                    packet.append(SRAMSIZE64K);
                                    COM->write(packet);
                                }
                            if(selectedItem == "32K")
                                {
                                    QByteArray packet;
                                    packet.append(controlByte);
                                    packet.append(controlByte1);
                                    packet.append(controlByte2);
                                    packet.append(controlByte1);
                                    packet.append(controlByte3);
                                    packet.append(SRAMSIZE256K);
                                    COM->write(packet);
                                }
                        }
                   else
                        {
                        ui->xinxi->append("File Open Failed\r\n");
                        }
                }
        }
    else
        {
            ui->xinxi->append("Device not connected\r\n");
        }
}
