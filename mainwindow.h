#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTimer>
#include <QElapsedTimer>
#include <QLabel>
#include <QMap>
#include <QGridLayout> 

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
    void on_btnLoadFile_clicked();
    void on_btnApplyFilter_clicked();
    void on_btnPlay_clicked();
    void on_btnFF_clicked();
    void updateTableRow();
    void FastForward();
    void updateLabel(const QString &canID, const QString &dataBytes);

private:
    Ui::MainWindow *ui;
    void loadCANLogFile(const QString &filePath);
    void applyFilter(const QString &filterID);
    void Play();
    
    QTimer *playTimer;
    int currentRow;
    bool isPlaying;
    QElapsedTimer clickTimer; 
    int playbackSpeed; 

    QMap<QString, QLabel*> canIDLabelMap; // Map to store labels for each CAN ID
    QGridLayout *gridLayout; 
};

#endif // MAINWINDOW_H