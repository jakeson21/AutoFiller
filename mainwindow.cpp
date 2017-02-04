#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <locale>         // std::locale, std::toupper

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

#ifdef Q_OS_WIN
    OS::WindowsInput* input = new OS::WindowsInput;
    mKeyInput.reset(input);
#endif

#ifdef Q_OS_LINUX
    OS::LinuxInput* input = new OS::WindowsInput;
    mKeyInput.reset(input);
#endif

}

MainWindow::~MainWindow()
{
    delete ui;
}

//#ifdef Q_OS_LINUX
//bool MainWindow::switchToLastActiveWindow()
//{
//    /*HWND handle = GetForegroundWindow(); // Handle of this window
//    std::cout << "Foreground window = " << handle << std::endl;

//    // Get handle of last active window: the window we want to send keystrokes to
//    HWND nextWindow = GetNextWindow( handle, GW_HWNDNEXT);

//    // Move to its true parent handle
//    while (true)
//    {
//        HWND temp = GetParent(nextWindow);
//        if (temp == NULL) break;
//        nextWindow = temp;
//    }
//    std::cout << "nextWindow = " << nextWindow << std::endl;

//    if( !SetForegroundWindow( nextWindow ) )
//    {
//        std::cout << "Could not nextWindow to foreground" << std::endl;
//        return false;
//    }
//    std::cout << "Set nextWindow to foreground" << std::endl;
//*/
//    std::cout << "Set nextWindow to foreground" << std::endl;
//    system("xdotool getactivewindow windowminimize");

//    return true;
//}
//#endif

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
            ui->tableWidget->setRowCount(static_cast<int>(data.size()));
            ui->tableWidget->setColumnCount(static_cast<int>(headers.size()));

            // Fill data table
            for(size_t column=0; column<headers.size(); column++)
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

            for(size_t row=0; row<data.size(); row++)
            {
                for(size_t column=0; column<headers.size(); column++)
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

    if (mCsvData.isEmpty())
    {
        std::cout << "No data loaded. Doing nothing." << std::endl;
        return;
    }
    else if (mSelectedRow==-1)
    {
        std::cout << "No data selected. Doing nothing." << std::endl;
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
            std::cout << "Got token:[" << tokenText << "]" << std::endl;

            try
            {
                int col = std::stoi(tokenText);
                mKeyInput->AddStringToQueue(dataToSend[col]);
            }
            catch (std::invalid_argument)
            {
                // could not convert to number, assume key-code
                try
                {
                    std::string s = tokenText;
                    transform(s.begin(), s.end(), s.begin(), toupper);
                    mKeyInput->AddSpecialKeyToQueue(mKeyInput->specialKeyToInt(s));
                }
                catch (std::invalid_argument)
                {
                    std::cout << "ERROR: Unrecognized token, aborting: " << tokenText << std::endl;
                    return;
                }
            }
        }
    }

    // Send to previously active window
    mKeyInput->SwitchToPreviousWindow();
    mKeyInput->SendKeys();


}



