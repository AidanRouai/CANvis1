#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <QThread>
#include <QElapsedTimer> 


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , playTimer(new QTimer(this))
    , currentRow(0)
    , isPlaying(false)
{
    ui->setupUi(this);

    // Connect buttons to their respective slots
    connect(ui->btnLoadFile, &QPushButton::clicked, this, &MainWindow::on_btnLoadFile_clicked);
    connect(ui->btnApplyFilter, &QPushButton::clicked, this, &MainWindow::on_btnApplyFilter_clicked);
    connect(ui->btnPlay, &QPushButton::clicked, this, &MainWindow::on_btnPlay_clicked);
    
    // Connect timer to update slot
    connect(playTimer, &QTimer::timeout, this, &MainWindow::updateTableRow);
    
    // Set timer interval (100ms for smooth updates)
    playTimer->setInterval(100);
    
    // Initialize current frame table
    ui->currentFrameTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    clickTimer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnLoadFile_clicked()
{
    if (clickTimer.elapsed() < 300) {
        return;
    }
    // Open file dialog with filters for .log and .csv files
    QString filePath = QFileDialog::getOpenFileName(this, "Open CAN Log File", "",
                                                    "Log Files (*.log);;CSV Files (*.csv);;All Files (*)");

    if (!filePath.isEmpty())
    {
        ui->lblSelectedFile->setText(filePath); // Display selected file in the label
        loadCANLogFile(filePath);              // Parse the selected file
    }

    clickTimer.restart();
}

void MainWindow::loadCANLogFile(const QString &filePath)
{
    if (filePath.endsWith(".log", Qt::CaseInsensitive))
    {
        // Handle .log file parsing
        QFile file(filePath);

        // Attempt to open the .log file
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(this, "Error", "Unable to open the LOG file.");
            return;
        }

        // Clear existing data in the table
        ui->tableCANData->setRowCount(0);
        ui->currentFrameTable->clearContents();
        currentRow = 0;
        ui->progressBar->setValue(0);

        QTextStream in(&file);
        int rowCount = 0;

        // Regular expression to parse log lines
        QRegularExpression logRegex(R"(\((.*?)\)\s+(.*?)\s+(.*?)#(.*))");
        // Explanation for regex:
        // - \(.*?\): Captures the timestamp (anything inside parentheses)
        // - \s+: Matches whitespace
        // - .*?: Captures the CAN interface (e.g., "can0")
        // - .*?#(.*): Captures CAN ID and data separated by #

        while (!in.atEnd())
        {
            QString line = in.readLine();

            // Match the line with the regex
            QRegularExpressionMatch match = logRegex.match(line);
            if (match.hasMatch())
            {
                QString timestamp = match.captured(1);
                QString canInterface = match.captured(2);
                QString canID = match.captured(3);
                QString dataBytes = match.captured(4);

                // Add a new row to the table
                int currentRow = ui->tableCANData->rowCount();
                ui->tableCANData->insertRow(currentRow);

                // Populate the table columns
                ui->tableCANData->setItem(currentRow, 0, new QTableWidgetItem(timestamp));
                ui->tableCANData->setItem(currentRow, 1, new QTableWidgetItem(canID));
                ui->tableCANData->setItem(currentRow, 2, new QTableWidgetItem(dataBytes));
                ui->tableCANData->setItem(currentRow, 3, new QTableWidgetItem(canInterface));

                rowCount++;
            }
        }

        file.close(); // Close the file after reading

        QMessageBox::information(this, "File Loaded",
                                 QString("Successfully loaded %1 rows from the LOG file!").arg(rowCount));
    }
    else if (filePath.endsWith(".csv", Qt::CaseInsensitive))
    {
        // Handle .csv file parsing
        QFile file(filePath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(this, "Error", "Unable to open the CSV file.");
            return;
        }

        // Clear the existing data in the table
        ui->tableCANData->setRowCount(0);
        ui->currentFrameTable->clearContents();
        currentRow = 0;
        ui->progressBar->setValue(0);

        QTextStream in(&file);
        int rowCount = 0;
        QStringList headers;

        // Read the file line by line
        while (!in.atEnd())
        {
            QString line = in.readLine();

            // Split the line by commas for CSV parsing
            QStringList columns = line.split(',');

            // Use the first line as headers
            if (rowCount == 0)
            {
                headers = columns;
                ui->tableCANData->setColumnCount(headers.size());
                ui->tableCANData->setHorizontalHeaderLabels(headers);
            }
            else
            {
                // Insert a new row for each subsequent line
                ui->tableCANData->insertRow(ui->tableCANData->rowCount());
                for (int col = 0; col < columns.size(); ++col)
                {
                    ui->tableCANData->setItem(rowCount - 1, col, new QTableWidgetItem(columns[col]));
                }
            }

            rowCount++;
        }

        file.close();

        QMessageBox::information(this, "File Loaded",
                                 QString("Successfully loaded %1 rows from the CSV file!").arg(rowCount));
    }
    else
    {
        QMessageBox::warning(this, "Unsupported File",
                             "The selected file format is not supported.");
    }
}

void MainWindow::on_btnApplyFilter_clicked()
{
    if (clickTimer.elapsed() < 300) {
        return;
    }
    QString filterID = ui->lineEditFilterID->text();
    applyFilter(filterID);

    clickTimer.restart();

}

void MainWindow::applyFilter(const QString &filterID)
{

    // Filter table rows based on the provided filter ID
    int rowCount = ui->tableCANData->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        // Check if the CAN ID matches the filter ID
        QTableWidgetItem *item = ui->tableCANData->item(row, 1);
        if (item)
        {
            bool matches = item->text().contains(filterID, Qt::CaseInsensitive);
            ui->tableCANData->setRowHidden(row, !matches); // Hide non-matching rows
        }
    }

    QMessageBox::information(this, "Filter Applied",
                             QString("Filter applied for ID: %1").arg(filterID));
}

void MainWindow::Play()
{
    if (!isPlaying) {
        // Start playing
        if (currentRow >= ui->tableCANData->rowCount()) {
            currentRow = 0;
            ui->progressBar->setValue(0);
        }
        playTimer->start();
        ui->btnPlay->setText("⏸");
    } else {
        // Pause playing
        playTimer->stop();
        ui->btnPlay->setText("▶");
    }
    isPlaying = !isPlaying;
}

void MainWindow::on_btnPlay_clicked()
{
    if (clickTimer.elapsed() < 300) {
        return;
    }
    Play();

    clickTimer.restart();
}

void MainWindow::updateTableRow()
{
    int totalRows = ui->tableCANData->rowCount();
    if (currentRow < totalRows) {
        // Update progress bar
        int progressPercent = (currentRow * 100) / totalRows;
        ui->progressBar->setValue(progressPercent);
        
        // Highlight current row in main table
        ui->tableCANData->selectRow(currentRow);
        ui->tableCANData->scrollTo(ui->tableCANData->model()->index(currentRow, 0));
        
        // Update current frame table
        for (int col = 0; col < ui->tableCANData->columnCount(); col++) {
            QTableWidgetItem* sourceItem = ui->tableCANData->item(currentRow, col);
            if (sourceItem) {
                QTableWidgetItem* newItem = new QTableWidgetItem(sourceItem->text());
                ui->currentFrameTable->setItem(0, col, newItem);
            }
        }
        
        currentRow++;
    } else {
        // Stop at the end
        playTimer->stop();
        ui->btnPlay->setText("▶");
        isPlaying = false;
        currentRow = 0;
        ui->progressBar->setValue(100);
    }
}
