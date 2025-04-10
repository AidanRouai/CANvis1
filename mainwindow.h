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
#include <QPropertyAnimation>
#include <string>
#include <vector>
#include <utility>
#include "libs/can-utils/dbc/dbc_parser.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void populateMessageTable(const std::map<uint32_t, std::string> &messages);

private slots:
    void on_btnLoadFile_clicked();
    void on_btnApplyFilter_clicked();
    void on_btnPlay_clicked();
    void on_btnPlay_2_clicked();
    void on_btnFF_clicked();
    void on_btnFF_2_clicked();
    void on_btnLoadDBC_clicked();
    void updateTableRow();
    void FastForward();
    void updateLabel(const QString &canID, const QString &dataBytes);

private:
    Ui::MainWindow *ui;
    void loadCANLogFile(const QString &filePath);
    void loadDBC(const QString &filePathDBC);
    void applyFilter(const QString &filterID);
    void Play();

    QTimer *playTimer;
    int currentRow;
    bool isPlaying;
    QElapsedTimer clickTimer; 
    int playbackSpeed;
    QWidget *sideBarWidget; 
    bool isSidebarVisible;

    QMap<QString, QLineEdit*> canIDLabelMap; // Map to store editable labels for each CAN ID
    QGridLayout *gridLayout;
    QTableWidget *valTableWidget; // Pointer to the VAL_TABLE widget    
};

#endif // MAINWINDOW_H
