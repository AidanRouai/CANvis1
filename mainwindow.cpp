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
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , playTimer(new QTimer(this))
    , currentRow(0)
    , isPlaying(false)
    , playbackSpeed(1) // Initialize playback speed
{
    ui->setupUi(this);

    // Connect buttons to their respective slots
    connect(ui->btnLoadFile, &QPushButton::clicked, this, &MainWindow::on_btnLoadFile_clicked);
    connect(ui->btnApplyFilter, &QPushButton::clicked, this, &MainWindow::on_btnApplyFilter_clicked);
    connect(ui->btnPlay, &QPushButton::clicked, this, &MainWindow::on_btnPlay_clicked);
    connect(ui->btnFF, &QPushButton::clicked, this, &MainWindow::on_btnFF_clicked);
    
    // Connect timer to update slot
    connect(playTimer, &QTimer::timeout, this, &MainWindow::updateTableRow);
    
    // Set timer interval (100ms for smooth updates)
    playTimer->setInterval(100);
    
    // Initialize current frame table
    ui->currentFrameTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Initialize click timer
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
        ui->btnPlay->setStyleSheet("background-color: green;"); // Change to green when playing
    } else {
        // Pause playing
        playTimer->stop();
        ui->btnPlay->setText("▶");
        ui->btnPlay->setStyleSheet("background-color: red;"); // Change to red when paused
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

void MainWindow::updateLabel(const QString &canID, const QString &dataBytes)
{
    // Check if the label for the CAN ID already exists
    if (!canIDLabelMap.contains(canID)) {
        // Create a new label for the CAN ID
        QLabel *label = new QLabel(this);
        label->setFixedSize(50, 50);
        label->setStyleSheet("background-color: red; border-radius: 25px;");
        label->setAlignment(Qt::AlignCenter);
        label->setText(canID);
        canIDLabelMap.insert(canID, label);

        int index = canIDLabelMap.size() - 1;
        int row = index / 5;
        int col = index % 5;
        ui->gridLayout_2->addWidget(label, row, col); 
    }

    // Get the label for the CAN ID
    QLabel *label = canIDLabelMap.value(canID);

    // Check the value of dataBytes
    if (dataBytes.toInt() != 0) {
        // Flash the label green
        label->setStyleSheet("background-color: green; border-radius: 25px;");
        QTimer::singleShot(500, [label]() {
            label->setStyleSheet("background-color: red; border-radius: 25px;");
        });
    }
}

void MainWindow::updateTableRow()
{
    int totalRows = ui->tableCANData->rowCount();
    if (currentRow < totalRows) {
        int progressPercent = (currentRow * 100) / totalRows;
        ui->progressBar->setValue(progressPercent);
        ui->tableCANData->selectRow(currentRow);
        ui->tableCANData->scrollTo(ui->tableCANData->model()->index(currentRow, 0));
        for (int col = 0; col < ui->tableCANData->columnCount(); col++) {
            QTableWidgetItem* sourceItem = ui->tableCANData->item(currentRow, col);
            if (sourceItem) {
                QTableWidgetItem* newItem = new QTableWidgetItem(sourceItem->text());
                ui->currentFrameTable->setItem(0, col, newItem);
            }
        }

        // Update the graph with the new data
        QTableWidgetItem* canIDItem = ui->tableCANData->item(currentRow, 1);
        QTableWidgetItem* dataBytesItem = ui->tableCANData->item(currentRow, 2);
        if (canIDItem && dataBytesItem) {
            updateLabel(canIDItem->text(), dataBytesItem->text());
        }

        currentRow++;
    } else {
        playTimer->stop();
        ui->btnPlay->setText("▶");
        isPlaying = false;
        currentRow = 0;
        ui->progressBar->setValue(100);
    }
}

void MainWindow::on_btnFF_clicked()
{
    if (clickTimer.elapsed() < 300) {
        return;
    }

    FastForward();
}

void MainWindow::FastForward()
{
        // Cycle through playback speeds: 1x, 2x, 4x, 8x, 16x
    switch (playbackSpeed) {
        case 1:
            playbackSpeed = 2;
            ui->btnFF->setText("2x");
            break;
        case 2:
            playbackSpeed = 4;
            ui->btnFF->setText("4x");
            break;
        case 4:
            playbackSpeed = 8;
            ui->btnFF->setText("8x");
            break;
        case 8:
            playbackSpeed = 16;
            ui->btnFF->setText("16x");
            break;
        case 16:
            playbackSpeed = 32;
            ui->btnFF->setText("32x");
            break;
        default: 
            playbackSpeed = 1;
            ui->btnFF->setText("1x");
            break;
    }

    // Adjust the timer interval based on the playback speed
    playTimer->setInterval(100 / playbackSpeed);

    clickTimer.restart();
}
