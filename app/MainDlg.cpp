#include "MainDlg.h"
#include <string>

#include <QApplication>
#include <QSizePolicy>
#include <QPainter>
#include <QDialog>
#include <QDialogButtonBox>
#include <QSpacerItem>
#include <QDesktopServices>
#include <QSlider>
#include <QComboBox>
#include <QUrl>

#include "version.h"
#include "definitions.h"

using namespace std;
using namespace literals;

MainDlg::MainDlg(const std::string& configName)
    : m_strConfigName{ configName }
{
    // create signal/slot connections
    connect(this, &MainDlg::ShowMessage, this, &MainDlg::OnShowMessage);

    // create menu and widgets
    CreateMenuBar();

    QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;

    mainLayout->setMenuBar(m_menuBar);

    setLayout(mainLayout);
    mainLayout->setParent(this);
}

MainDlg::~MainDlg()
{
}

void MainDlg::CreateMenuBar()
{
    m_menuBar = new QMenuBar;

    QPointer<QMenu> fileMenu = new QMenu(QString::fromUtf8("&File"s), this);

    const auto strQuit = QString::fromUtf8("&Quit"s);

#ifdef Q_OS_WIN
    const QKeySequence quitShortcutSequence(QKeySequence::StandardKey::Close);
#else
    const QKeySequence quitShortcutSequence(QKeySequence::StandardKey::Quit);
#endif
    fileMenu->addAction(strQuit, this, &MainDlg::OnQuit, quitShortcutSequence);

    QPointer<QMenu> helpMenu = new QMenu(QString::fromUtf8("&Help"s), this);
    helpMenu->addAction(QString::fromUtf8("&About..."s), this, &MainDlg::OnAbout);

    m_menuBar->addMenu(fileMenu);
    m_menuBar->addMenu(helpMenu);
}

// Widget slot: "This process should end"
void MainDlg::OnQuit()
{
    QWidget::close();
}

// Widget override: the dialog is closing -> end/shtutdown
void MainDlg::closeEvent(QCloseEvent* event)
{

    QWidget::closeEvent(event);
}

// Widget slot: ShowMessage
void MainDlg::OnShowMessage(int text)
{
}

// Widget slot: show about dialog
void MainDlg::OnAbout()
{
    QPointer<QDialog> dlg = new QDialog(this);

    dlg->setWindowTitle(QString::fromUtf8("About"s));

    QPointer<QVBoxLayout> groupLayout = new QVBoxLayout;
    QPointer<QPushButton> closeButton = new QPushButton(QString::fromUtf8("Close"s));

    closeButton->setDefault(true);
    closeButton->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    dlg->connect(closeButton, &QPushButton::clicked, dlg, &QDialog::close);

	const string strProject = PROJECT_NAME;
	const string strVersion = PROJECT_VERSION;

    QPointer<QLabel> versionLabel = new QLabel(QString::fromUtf8(
        "<p><a href=\"https://github.com/Frank-Friemel/"s + strProject + "\">"s + strProject + "</a> "s + strVersion + "</p>"s));

    dlg->connect(versionLabel, &QLabel::linkActivated, [](QString link)
        {
            const QUrl url(link);
            QDesktopServices::openUrl(url);
        });

    QPointer<QLabel> infoLabel = new QLabel(QString::fromUtf8("Info about this application"s));
    infoLabel->setAlignment(Qt::AlignmentFlag::AlignHCenter);

    groupLayout->addWidget(versionLabel);
    groupLayout->addWidget(infoLabel);
    groupLayout->addWidget(closeButton);

    groupLayout->setAlignment(versionLabel, Qt::AlignmentFlag::AlignHCenter);
    groupLayout->setAlignment(infoLabel, Qt::AlignmentFlag::AlignHCenter);
    groupLayout->setAlignment(closeButton, Qt::AlignmentFlag::AlignHCenter);

    QPointer<QGroupBox> group = new QGroupBox;
    group->setLayout(groupLayout);

    QPointer<QVBoxLayout> mainLayout = new QVBoxLayout(dlg);

    mainLayout->addWidget(group);

    dlg->setLayout(mainLayout);
    dlg->exec();
}
