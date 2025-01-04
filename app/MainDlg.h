#pragma once

#include <QLabel>
#include <QMenuBar>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QProgressBar>
#include <QPixmap>
#include <QString>
#include <QByteArray>
#include <QFormLayout>
#include <QPointer>
#include <QSystemTrayIcon>
#include <QAction>

#include <future>
#include <atomic>
#include <memory>
#include <thread>
#include <mutex>
#include <list>
#include <chrono>
#include <stdint.h>

//
// based on Qt example: https://doc.qt.io/qt-6/qtwidgets-layouts-basiclayouts-example.html
//
class MainDlg
    : public QWidget
{
    Q_OBJECT

public:
    MainDlg(const std::string& configName);
    ~MainDlg();

private:
    void CreateMenuBar();

protected:
    // QWidget overrides
    void closeEvent(QCloseEvent* event) override;

signals:
    void ShowMessage(int text) const;

private slots:
    void OnQuit();
    void OnShowMessage(int text);
    void OnAbout();

private:
    const std::string                   m_strConfigName;

    // Qt Widgets
    // Menu
    // read about the Qt Object Model: https://doc.qt.io/qt-6/object.html
    QPointer<QMenuBar>                  m_menuBar;
};
