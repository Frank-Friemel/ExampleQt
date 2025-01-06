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
#include "localization/StringIDs.h"
#include <spdlog/spdlog.h>

using namespace std;
using namespace literals;

MainDlg::MainDlg(const SharedPtr<IValueCollection>& config, const std::string& configName)
    : m_config{ config }
    , m_strConfigName{ configName }
    , m_iconApplication{ ":/Application.ico" }

{
    // create signal/slot connections
    connect(this, &MainDlg::ShowMessage, this, &MainDlg::OnShowMessage);

    setWindowIcon(QIcon(":/Application.png"));
    setWindowTitle(QString::fromUtf8(PROJECT_NAME));

    // create menu and widgets
    CreateMenuBar();

    QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;

    mainLayout->setMenuBar(m_menuBar);

    setLayout(mainLayout);
    mainLayout->setParent(this);

    auto geometry = VariantValue::Key("WindowGeometry").TryGet<vector<BYTE>>(m_config);

    // restore Window Geometry
    if (geometry.has_value())
    {
        const auto geometryBlob = std::move(geometry.value());

        const QByteArray geo((const char*)geometryBlob.data(), geometryBlob.size());
        restoreGeometry(geo);
    }
}

MainDlg::~MainDlg()
{
}

QString MainDlg::GetString(int id) const
{
    return GetLanguageManager(m_config)->GetString(id).c_str();
}

void MainDlg::CreateMenuBar()
{
    m_menuBar = new QMenuBar;

    QPointer<QMenu> fileMenu = new QMenu(GetString(StringID::MENU_FILE), this);

    const auto strQuit = GetString(StringID::MENU_QUIT);

#ifdef Q_OS_WIN
    const QKeySequence quitShortcutSequence(QKeySequence::StandardKey::Close);
#else
    const QKeySequence quitShortcutSequence(QKeySequence::StandardKey::Quit);
#endif
    fileMenu->addAction(QIcon(":/exit-16.ico"), strQuit, this, &MainDlg::OnQuit, quitShortcutSequence);

    QPointer<QMenu> helpMenu = new QMenu(GetString(StringID::MENU_HELP), this);
    helpMenu->addAction(QIcon(":/info-16.ico"), GetString(StringID::MENU_ABOUT), this, &MainDlg::OnAbout);

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
    try
    {
        const QByteArray geometry = saveGeometry();

        if (!geometry.isEmpty())
        {
            const auto blob = MakeShared<BlobStream>(geometry.size(), geometry.data());
            VariantValue::Key("WindowGeometry").Set(m_config, blob);
        }
    }
    catch (...)
    {
        spdlog::error("failed to save geometry");
    }
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

    dlg->setWindowTitle(GetString(StringID::DIALOG_ABOUT));

    QPointer<QVBoxLayout> groupLayout = new QVBoxLayout;
    QPointer<QPushButton> closeButton = new QPushButton(GetString(StringID::CLOSE));

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
