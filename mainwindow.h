#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QSysInfo>

#include <OS/GenericInput.h>
#ifdef Q_OS_WIN
#include <OS/WindowsInput.hpp>
#endif
#ifdef Q_OS_LINUX
#include <OS/LinuxInput.hpp>
#endif

#include <map>
#include <vector>
#include <string>
#include <sstream>      // std::stringstream
#include <ctype.h>      // std::tolower
#include <iostream>

#include <chrono>
#include <thread>
#include <memory>
#include <csvdataparser.hpp>
using namespace Utils;

//#include <keysequencer.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum class TokenState {
        OpenToken = 0,
        CloseToken,
        TokenText
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    int SendStringToActiveWindow( const std::wstring& text );

public slots:
    void on_buttonSendInfo_clicked(bool inState);
    void handleMenuOpen(bool inIsChecked);
    void handleMenuClose(bool inIsChecked);
    void handleCellClicked(int inRow, int inCol);
    void handleListItemDoubleClicked(QListWidgetItem* inItem);

protected:
    bool switchToLastActiveWindow();

private:
    Ui::MainWindow *ui;

    CsvData mCsvData;
    int mSelectedRow;

    std::unique_ptr<OS::GenericInput> mKeyInput;
};

#endif // MAINWINDOW_H
