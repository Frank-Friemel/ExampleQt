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

#include "localization/StringIDs.h"
#include "localization/LanguageManager.h"

#include "MainDlg.h"

#include <QApplication>
#include <QMessageBox>

#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>

using namespace std;
using namespace literals;

static SharedPtr<IValueCollection> LoadConfig(const string& configPath);
static void SaveConfig(const SharedPtr<IValueCollection>& config, const string& configPath);
static void InitializeConfig(const SharedPtr<IValueCollection>& config);

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
    const auto strConfigPath = GetHomeDirectoryA() + GetPathDelimiter() + strConfigFileName;

    // set random seed
    srand(static_cast<unsigned int>(time(nullptr)));

    setlocale(LC_ALL, "C");

    int result = -1;
    shared_ptr<spdlog::async_logger> logger;
    SharedPtr<IValueCollection> config;
    unique_ptr<QApplication> app;

    try
    {
        // we have to create the app-object here
        // because QT has to get to know the "main thread"        
        app = make_unique<QApplication>(argc, argv);

        // init logger thread-pool
        spdlog::init_thread_pool(4096, 1);

        // create the logger
        logger = make_shared<spdlog::async_logger>(
            string(PROJECT_NAME),
            spdlog::sinks_init_list
            {
#ifdef _WIN32
                make_shared<spdlog::sinks::windebug_sink_mt>(),
#endif
                make_shared<spdlog::sinks::stdout_color_sink_mt>()
            },
            spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);

#ifndef NDEBUG
        logger->set_level(spdlog::level::level_enum::debug);
#endif
        // make our logger the default-logger
        spdlog::set_default_logger(logger);

        spdlog::info("Starting ...");

        // loading the config
        config = LoadConfig(strConfigPath);

        if (!debugLogToFile && VariantValue::Key("DebugLogFile").Has(config))
        {
            // enable "log to file" option, if config says so
            if (VariantValue::Key("DebugLogFile").Get<bool>(config))
            {
                debugLogToFile = true;
            }
        }

        // enable log to file, if enabled
        if (debugLogToFile)
        {
            //EnableLogToFile(true, LOG_FILE_NAME);
        }
        // initializing the config
        InitializeConfig(config);

        // create GUI
        MainDlg mainDialog(config, strConfigName);

        // QT best practices:
        // https://de.slideshare.net/slideshow/how-to-make-your-qt-app-look-native/2622616
        mainDialog.show();

        // enter the main loop (which blocks)
        result = app->exec();

        // saving the config
        SaveConfig(config, strConfigPath);

        spdlog::info("Terminating with result {}", result);
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
    config.Clear();
    logger.reset();
    spdlog::shutdown();

    return result;
}

static SharedPtr<IValueCollection> LoadConfig(const string& configPath)
{
    auto config = MakeShared<ValueCollection>();
    auto configStream = MakeShared<BlobStream>();

    spdlog::debug("read config file: {}", configPath);

    if (configStream->FromFile(configPath))
    {
        if (!FromJson(config, configStream))
        {
            spdlog::error("Failed to parse config: {}", Stringify<string>(configStream));
            assert(false);
        }
    }
    else
    {
        const auto error = GetLastError();

        if (error != ERROR_FILE_NOT_FOUND)
        {
            spdlog::error("Failed to load config: {}", error);
            assert(false);
        }
    }
    return config;
}

static void SaveConfig(const SharedPtr<IValueCollection>& config, const string& configPath)
{
    auto configStream = MakeShared<BlobStream>();

    if (!ToJson(config, configStream, JsonFormat::humanreadable))
    {
        spdlog::error("Failed to create config");
    }
    else
    {
        const auto newConfigfile = configPath + ".new"s;

        if (!configStream->ToFile(newConfigfile))
        {
            spdlog::error("Failed (error: {}) to write new config: {}", GetLastError(), Stringify<string>(configStream));
            DeleteFileA((newConfigfile).c_str());
        }
        else
        {
            if (!DeleteFileA(configPath.c_str()) && GetLastError() != ERROR_FILE_NOT_FOUND)
            {
                spdlog::error("Failed (error: {}) to delete old config: {}", GetLastError(), configPath);
            }

            if (!MoveFileA((newConfigfile).c_str(), configPath.c_str()))
            {
                spdlog::error("Failed (error: {}) to rename new config: {} to {}", GetLastError(),
                    newConfigfile, configPath);
            }
#ifdef _WIN32
            if (!::SetFileAttributesA(configPath.c_str(), FILE_ATTRIBUTE_HIDDEN))
            {
                spdlog::error("Failed (error: {}) to set config hidden", GetLastError());
            }
#endif
        }
    }
}

static void InitializeConfig(const SharedPtr<IValueCollection>& config)
{
    // create Language Manager
    VariantValue::Key("LanguageManager").Set(config, MakeShared<Localization::LanguageManager>());

    if (VariantValue::Key("Language").Has(config))
    {
        // set language from config (language override)
        const auto language = VariantValue::Key("Language").Get<string>(config);

        GetLanguageManager(config)->SetCurrentLanguage(language);
        spdlog::debug("set language from config: {}", language);
    }
    else
    {
        // test
        // GetLanguageManager(config)->SetCurrentLanguage("en-en"s);
        // GetLanguageManager(config)->SetCurrentLanguage("ja-jp"s);

        // log the system language
        spdlog::debug("current language is: {}", GetLanguageManager(config)->GetCurrentLanguage());
    }
}