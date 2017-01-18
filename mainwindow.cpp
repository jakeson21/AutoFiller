#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <TCHAR.H>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mCsvData(),
    mSelectedRow(-1)
{
    ui->setupUi(this);

    QObject::connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::handleMenuOpen);
    QObject::connect(ui->actionClose, &QAction::triggered, this, &MainWindow::handleMenuClose);
    QObject::connect(ui->tableWidget, &QTableWidget::cellClicked, this, &MainWindow::handleCellClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::switchToLastActiveWindow()
{
    HWND handle = GetForegroundWindow(); // Handle of this window
    std::cout << "Foreground window = " << handle << std::endl;

    // Get handle of last active window: the window we want to send keystrokes to
    HWND nextWindow = GetNextWindow( handle, GW_HWNDNEXT);

    // Move to its true parent handle
    while (true)
    {
        HWND temp = GetParent(nextWindow);
        if (temp == NULL) break;
        nextWindow = temp;
    }
    std::cout << "nextWindow = " << nextWindow << std::endl;

    if( !SetForegroundWindow( nextWindow ) )
    {
        std::cout << "Could not nextWindow to foreground" << std::endl;
        return false;
    }
    std::cout << "Set nextWindow to foreground" << std::endl;
    return true;
}

void MainWindow::handleMenuOpen(bool /*inIsChecked*/)
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open .csv"),
                                            "",
                                            tr("Csv Files (*.csv *.txt)"));
    if (!fileName.isNull())
    {
        if (mCsvData.parseCsvDataFile(fileName.toStdString()))
        {
            std::vector<std::string> headers = mCsvData.getHeaders();
            CsvData::DataMapType data = mCsvData.getData();

            ui->tableWidget->clearContents();
            mSelectedRow = -1;

            // EMPTY TABLE CONTENTS
            for(int column=0; column<ui->tableWidget->columnCount(); column++)
            {
                delete ui->tableWidget->takeHorizontalHeaderItem(column);
                for(int row=1; row<=ui->tableWidget->rowCount(); row++)
                    delete ui->tableWidget->takeItem(row, column);
            }
            while(QListWidgetItem* item = ui->listWidget->takeItem(0)) { delete item; }

            // Setup new data table size
            ui->tableWidget->setRowCount(data.size());
            ui->tableWidget->setColumnCount(headers.size());

            // Fill data table
            for(int column=0; column<headers.size(); column++)
            {
                QTableWidgetItem* newItem = new QTableWidgetItem();
                newItem->setText(QString::fromStdString(headers[column]));
                ui->tableWidget->setHorizontalHeaderItem(column, newItem);

                // Fill data item list
                std::stringstream ss;
                ss << "[" << column << "] " << headers[column];
                ui->listWidget->addItem(QString::fromStdString(ss.str()));                                
            }
            connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::handleListItemDoubleClicked);

            for(int row=0; row<data.size(); row++)
            {
                for(int column=0; column<headers.size(); column++)
                {
                    QTableWidgetItem* newItem = new QTableWidgetItem();
                    newItem->setText(QString::fromStdString(data[row][column]));
                    ui->tableWidget->setItem(row,column,newItem);
                }
            }
        }
    }
}

void MainWindow::handleListItemDoubleClicked(QListWidgetItem* inItem)
{
    int index = ui->listWidget->row(inItem);
    std::cout << "Selected index " << index << std::endl;
    ui->lineEditKeySequence->insert(QString("[%1]").arg(index));
}

void MainWindow::handleMenuClose(bool /*inIsChecked*/)
{
    ui->tableWidget->clearContents();
    mSelectedRow = -1;

    // EMPTY TABLE CONTENTS
    for(int column=0; column<ui->tableWidget->columnCount(); column++)
    {
        delete ui->tableWidget->takeHorizontalHeaderItem(column);
        for(int row=1; row<=ui->tableWidget->rowCount(); row++)
            delete ui->tableWidget->takeItem(row, column);
    }
    while(QListWidgetItem* item = ui->listWidget->takeItem(0)) { delete item; }

    // Setup new data table size
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);

    mCsvData.clear();
}


void MainWindow::handleCellClicked(int inRow, int /*inCol*/)
{
    mSelectedRow = inRow;
}

void MainWindow::on_buttonSendInfo_clicked(bool /*inState*/)
{
    QString tokens = ui->lineEditKeySequence->text();

    // Parse key sequence into tokens
    std::string tokenText;
    TokenState state = TokenState::OpenToken;

    std::vector<std::unique_ptr<KeyCommandInterface>> keySequence;

    if (mSelectedRow==-1 || mCsvData.isEmpty())
    {
        std::cout << "No data loaded. Doing nothing." << std::endl;
        return;
    }
    std::vector<std::string> headers = mCsvData.getHeaders();
    std::vector<std::string> dataToSend = mCsvData.getData().at(mSelectedRow);

    for (char c : tokens.toStdString())
    {
        switch (state)
        {
            case TokenState::OpenToken:
            switch (tolower(c))
            {
                case '[':
                tokenText.clear();
                state = TokenState::TokenText;
                break;

                case ']':
                state = TokenState::OpenToken;
                break;

                default:
                state = TokenState::TokenText;
                tokenText.push_back(c);
            }
            break;

            case TokenState::CloseToken:
            switch (tolower(c))
            {
                case '[':
                state = TokenState::OpenToken;
                tokenText.clear();
                break;

                default:
                    std::cout << "Error, expected '['" << std::endl;
            }
            break;

            case TokenState::TokenText:
                switch (tolower(c))
                {
                    case '[':
                    state = TokenState::OpenToken;
                    break;

                    case ']':
                    state = TokenState::CloseToken;
                    break;

                    default:
                    tokenText.push_back(c);
                }
            break;

        }

        if (state == TokenState::CloseToken)
        {
            // do something with token text
            std::cout << "Got token: " << tokenText << std::endl;

            try
            {
                int col = std::stoi(tokenText);
                keySequence.emplace_back(new StringKeys(dataToSend[col]));
            }
            catch (std::invalid_argument)
            {
                // could not convert to number, assume key-code
                try
                {
                    keySequence.emplace_back(new TokenKeys(tokenText));
                }
                catch (std::invalid_argument)
                {
                    std::cout << "ERROR: Unrecognized token, aborting: " << tokenText << std::endl;
                    return;
                }
            }
        }
    }

    std::cout << "Key Sequence size = " << keySequence.size() << std::endl;
    if (keySequence.size() == 0) { return; }

    // Send to previously active window
    switchToLastActiveWindow();

    for (int n=0; n<keySequence.size(); n++)
    {
        keySequence[n]->SendKeys();
    }
}



