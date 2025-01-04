#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <stdexcept>
#include <stdint.h>
#include <assert.h>

#ifndef _WIN32
#include <cstring>
#endif

#include "definitions.h"
#include "version.h"

#include "MainDlg.h"

#include <QApplication>
#include <QMessageBox>

using namespace std;
using namespace literals;

// commandline parameters for debugging (Visual Studio): .vs/launch.vs.json
// https://stackoverflow.com/questions/30104520/adding-command-line-arguments-to-project
//
int main(int argc, char* argv[])
{
    bool debugLogToFile = false;
    string strConfigName;
    string strConfigFileName = "."s + PROJECT_NAME + "_Config.json"s;

    if (argv)
    {
        for (int nArg = 1; nArg < argc; ++nArg)
        {
            if (argv[nArg])
            {
                if (strstr(argv[nArg], "-log"))
                {
                    debugLogToFile = true;
                }
                else if (const char* configName = strstr(argv[nArg], "-config="))
                {
                    configName += strlen("-config=");

                    while (*configName)
                    {
                        const char c = *configName;

                        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
                        {
                            strConfigName += c;
                        }
                        ++configName;
                    }

                    if (!strConfigName.empty())
                    {
                        const auto pos = strConfigFileName.rfind('.');
                        assert(pos != string::npos);

                        strConfigFileName.insert(pos, strConfigName);
                    }
                }
            }
        }
    }
    // set random seed
    srand(static_cast<unsigned int>(time(nullptr)));

    setlocale(LC_ALL, "C");

    int result = -1;
    unique_ptr<QApplication> app;

    try
    {
        // we have to create the app-object here
        // because QT has to get to know the "main thread"        
        app = make_unique<QApplication>(argc, argv);

        // create GUI
        MainDlg mainDialog(strConfigName);

        // QT best practices:
        // https://de.slideshare.net/slideshow/how-to-make-your-qt-app-look-native/2622616
        mainDialog.show();

        // enter the main loop (which blocks)
        result = app->exec();
    }
    catch (bad_alloc)
    {
        // no message box without having QT initialized, sorry
        if (app)
        {
            QMessageBox msgBox;
            msgBox.setText("The system is out of memory.");
            msgBox.addButton("Close", QMessageBox::AcceptRole);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
    }
    catch (const runtime_error& e)
    {
        // no message box without having QT initialized, sorry
        if (app)
        {
            QMessageBox msgBox;

            msgBox.setText(e.what());
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
    }

    // teardown
    app.reset();

    return result;
}