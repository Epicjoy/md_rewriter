#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QFileDialog>
#include <QFile>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_shuaxin_clicked();
    void on_lianjie_clicked();
    void on_qingkong_clicked();
    void readSerialData();
    void on_shibiekadai_clicked();
    void on_cachu_clicked();
    void on_shaolu_clicked();
    void on_duqu_clicked();
    void on_jiaoyanwenjian_clicked();
    void on_rongliangcachu_clicked();
    void on_shaolucundang_clicked();
    void on_duqucundang_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QString filePath;
    QByteArray hexStringToByteArray(const QString &hex);
    qint64 receivedDataLength;
    qint64 fileSize;
    QFile file;
    QByteArray buffer;
    bool isSaving;
    uint32_t address;
    uint32_t addj;
    uint32_t cnt;
    uint32_t bank;
    uint32_t jindu;
    bool waitingForResponse;
    void sendData();
    void sendsramData();
    QString filePath1;
    QString filePath2;
    QByteArray readFile(const QString &filePath);
    void compareFiles(const QByteArray &fileContent1, const QByteArray &fileContent2);
};
#endif // MAINWINDOW_H
