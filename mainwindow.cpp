#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect buttons to their slots
    connect(ui->btnLoadFile, &QPushButton::clicked, this, &MainWindow::on_btnLoadFile_clicked);
    connect(ui->btnApplyFilter, &QPushButton::clicked, this, &MainWindow::on_btnApplyFilter_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnLoadFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open CAN Log File", "", "CSV Files (*.csv);;All Files (*)");
    if (!filePath.isEmpty())
    {
        ui->lblSelectedFile->setText(filePath);
        loadCANLogFile(filePath);
    }
}

void MainWindow::loadCANLogFile(const QString &filePath)
{
    // TODO: Implement actual file parsing here
    QMessageBox::information(this, "File Loaded", "File loaded successfully: " + filePath);

    // Mock data for the table
    ui->tableCANData->setRowCount(5);
    for (int i = 0; i < 5; i++)
    {
        ui->tableCANData->setItem(i, 0, new QTableWidgetItem(QString("Timestamp %1").arg(i)));
        ui->tableCANData->setItem(i, 1, new QTableWidgetItem(QString("ID %1").arg(i)));
        ui->tableCANData->setItem(i, 2, new QTableWidgetItem("Data"));
        ui->tableCANData->setItem(i, 3, new QTableWidgetItem("Description"));
    }
}

void MainWindow::on_btnApplyFilter_clicked()
{
    QString filterID = ui->lineEditFilterID->text();
    applyFilter(filterID);
}

void MainWindow::applyFilter(const QString &filterID)
{
    // TODO: Filter table rows based on filterID
    QMessageBox::information(this, "Filter Applied", "Filter applied for ID: " + filterID);
}