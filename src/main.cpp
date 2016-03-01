/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2016 by Luca Bellonda and individual contributors  *
 *    as indicated in the AUTHORS file                                    *
 *  lbellonda _at_ gmail.com                                              *
 *                                                                        *
 * This library is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU Library General Public            *
 * License as published by the Free Software Foundation; either           *
 * version 2 of the License, or (at your option) any later version.       *
 *                                                                        *
 * This library is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 * Library General Public License for more details.                       *
 *                                                                        *
 * You should have received a copy of the GNU Library General Public      *
 * License along with this library; if not, write to the                  *
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,       *
 * Boston, MA  02110-1301  USA                                            *
 **************************************************************************/

#include "xmlEdit.h"
#include <QApplication>
#include "xmleditglobals.h"
#include "qxmleditconfig.h"
#include "mainwindow.h"
#include "element.h"
#include "utils.h"
#include "StartParams.h"
#include "services/loghandler.h"
#include <QtGlobal>
#include "qxmleditapplication.h"
#include "modules/services/anotifier.h"

#include "licensedialog.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,3)
#if defined(ENVIRONMENT_MACOS)
#define MACOS_SPECIFIC
#endif
#endif

#if !defined(QXMLEDIT_NOMAIN)

#if defined(MACOS_SPECIFIC)
#include <QtMac>
#endif

const char *APP_TITLE = QT_TR_NOOP("QXmlEdit");

#define QXMLEDIT_TRANSLATIONS_PREFIX "QXmlEdit_"
#define QXMLEDITWIDGET_TRANSLATIONS_PREFIX  "QXmlEditWidget_"
#define QXMLEDITSESSION_TRANSLATIONS_PREFIX "QXmlEditSessions_"

void printHelp();

bool decodeCommandLine(QStringList args, StartParams *params);
static void startTanslator(QApplication *app);
static void initLogger();
static void todo();
static void installMsgHandler();
static void addMenuExtra(QXmlEditApplication *app, MainMenuBlock *mainMenuBlock);
static void removeMenuExtra(QXmlEditApplication *app, MainMenuBlock *mainMenuBlock);
static bool licenseAgreement();
static int doAnonymize(QXmlEditApplication *app, StartParams &startParams);

static QTranslator qtLibTranslator;
static QTranslator qXmlEditTranslator;
static QTranslator qXmlEditWidgetTranslator;
static QTranslator qXmlEditSessionTranslator;

static LogHandler logHandler;
static StartParams startParams ;

int internalMain(int argc, char *argv[])
{
    QXmlEditApplication app(argc, argv);
    Q_INIT_RESOURCE(risorse);

    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QXmlEditGlobals::setAppTitle(APP_TITLE);

    installMsgHandler();
    if(!Config::init()) {
        Utils::errorReadingUserSettings();
    }
    ApplicationData appData;
    appData.setLogger(&logHandler);
    initLogger();
    appData.init();
    startTanslator(&app);
    app.setAppData(&appData);
    app.setLogger(&logHandler);

    todo();
    Element::loadIcons();
    app.setWindowIcon(QIcon(":/tree/icon.png"));

    if(!licenseAgreement()) {
        return -1 ;
    }

    decodeCommandLine(app.arguments(), &startParams);
    if(app.handleSingleInstance(&startParams)) {
        return 0;
    }
    if(startParams.type == StartParams::Anonymize) {
        int returnCode = doAnonymize(&app, startParams);
        return returnCode;
    }

    MainWindow *mainWindow = new MainWindow(false, &app, &appData);

    mainWindow->show();
    switch(startParams.type) {
    default:
    case StartParams::Nothing:
        mainWindow->triggersWelcomeDialog();
        break;
    case StartParams::OpenFile:
        mainWindow->loadFile(startParams.fileName);
        break;
    case StartParams::VisFile:
        mainWindow->loadVisFile(startParams.fileName);
        break;
    }
    app.connect(appData.notifier(), SIGNAL(newWindowRequested()), &app, SLOT(onNewWindow()));
    app.connect(appData.notifier(), SIGNAL(encodingToolsRequested()), &app, SLOT(onEncodingTools()));
    app.connect(appData.notifier(), SIGNAL(codePageToolsRequested()), &app, SLOT(onCodePagesTools()));
    app.connect(appData.notifier(), SIGNAL(sessionRequested()), &app, SLOT(onManageSessions()));
    app.connect(appData.notifier(), SIGNAL(splitFileRequested()), &app, SLOT(onSplitFile()));
    app.connect(appData.notifier(), SIGNAL(viewMapRequested()), &app, SLOT(onViewData()));
    app.connect(appData.notifier(), SIGNAL(raiseWindowsRequested()), &app, SLOT(onRaiseWindows()));
    MainMenuBlock mainMenuBlock;
    addMenuExtra(&app, &mainMenuBlock);
    int result = app.exec();
    removeMenuExtra(&app, &mainMenuBlock);
    app.disconnect(appData.notifier(), SIGNAL(raiseWindowsRequested()), &app, SLOT(onRaiseWindows()));
    app.disconnect(appData.notifier(), SIGNAL(viewMapRequested()), &app, SLOT(onViewData()));
    app.disconnect(appData.notifier(), SIGNAL(splitFileRequested()), &app, SLOT(onSplitFile()));
    app.disconnect(appData.notifier(), SIGNAL(sessionRequested()), &app, SLOT(onManageSessions()));
    app.disconnect(appData.notifier(), SIGNAL(codePageToolsRequested()), &app, SLOT(onCodePagesTools()));
    app.disconnect(appData.notifier(), SIGNAL(encodingToolsRequested()), &app, SLOT(onEncodingTools()));
    app.disconnect(appData.notifier(), SIGNAL(newWindowRequested()), &app, SLOT(onNewWindow()));
    appData.end();
    if(!Config::end()) {
        Utils::errorSavingUserSettings();
    }
    return result;
}

static void addMenuExtra(QXmlEditApplication *app, MainMenuBlock *mainMenuBlock)
{
    mainMenuBlock->setup();
    app->connect(mainMenuBlock->_newWindowAction, SIGNAL(triggered()), app, SLOT(onNewWindow()));
    app->connect(mainMenuBlock->_encodingToolsAction, SIGNAL(triggered()), app, SLOT(onEncodingTools()));
    app->connect(mainMenuBlock->_codePageToolsAction, SIGNAL(triggered()), app, SLOT(onCodePagesTools()));
    app->connect(mainMenuBlock->_sessionAction, SIGNAL(triggered()), app, SLOT(onManageSessions()));
    app->connect(mainMenuBlock->_splitFileAction, SIGNAL(triggered()), app, SLOT(onSplitFile()));
    app->connect(mainMenuBlock->_viewMapAction, SIGNAL(triggered()), app, SLOT(onViewData()));
    app->connect(mainMenuBlock->_raiseWindows, SIGNAL(triggered()), app, SLOT(onRaiseWindows()));

#if defined(MACOS_SPECIFIC)
    qt_mac_set_dock_menu(mainMenuBlock->_contextMenu);
#endif
}

static void removeMenuExtra(QXmlEditApplication *app, MainMenuBlock *mainMenuBlock)
{
    app->disconnect(mainMenuBlock->_newWindowAction, SIGNAL(triggered()), app, SLOT(onNewWindow()));
    app->disconnect(mainMenuBlock->_encodingToolsAction, SIGNAL(triggered()), app, SLOT(onEncodingTools()));
    app->disconnect(mainMenuBlock->_codePageToolsAction, SIGNAL(triggered()), app, SLOT(onCodePagesTools()));
    app->disconnect(mainMenuBlock->_sessionAction, SIGNAL(triggered()), app, SLOT(onManageSessions()));
    app->disconnect(mainMenuBlock->_splitFileAction, SIGNAL(triggered()), app, SLOT(onSplitFile()));
    app->disconnect(mainMenuBlock->_viewMapAction, SIGNAL(triggered()), app, SLOT(onViewData()));
    app->disconnect(mainMenuBlock->_raiseWindows, SIGNAL(triggered()), app, SLOT(onRaiseWindows()));

#if defined(MACOS_SPECIFIC)

#endif
}

static int doAnonymize(QXmlEditApplication *app, StartParams &startParams)
{
    Utils::setBatch(true);
    QTextStream stdErr(stderr);
    if(startParams.parametersError) {
        stdErr << startParams.errorMessage ;
        return -1;
    }
    OperationResult *result = app->anonymizeBatch(startParams.fileName, startParams.arg1, startParams.arg2);
    if(NULL == result) {
        stdErr << QObject::tr("Error: unable to complete the operation.");
        return -1 ;
    }
    int returnCode = 0;
    if(result->isError()) {
        stdErr << result->message();
        returnCode = -1 ;
    }
    delete result;
    return returnCode ;
}


static void initLogger()
{
    bool isEnabled = Config::getBool(Config::KEY_LOGS_ENABLED, false);
    int logLevel = Config::getInt(Config::KEY_LOGS_LEVEL, FrwLogger::INFO);
    logHandler.setLevel((FrwLogger::ELogLevel)logLevel);
    logHandler.setEnabled(isEnabled);
}

void msgBoxDebug(const QString message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

/** \brief: insert here all the fundamentals todo using Utils::TODO_THIS_RELEASE().
  */
#if defined(MACOS_SPECIFIC)
void qt_mac_set_dock_menu(QMenu *menu);
#endif

void todo()
{
#if defined(MACOS_SPECIFIC) && defined(QXMLEDIT_VERSION_IS_SNAPSHOT)
    QtMac::setBadgeLabelText("Beta");
#endif
}

static void startTanslator(QApplication *app)
{
    if(qtLibTranslator.load("qt_" + QLocale::system().name(),
                            QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        app->installTranslator(&qtLibTranslator);
    }
    if(qXmlEditTranslator.load(QXMLEDIT_TRANSLATIONS_PREFIX + QLocale::system().name(), ApplicationData::getResourceDir() + QString("/") + QString(STRINGIZE(TRANLASTION_DIR)))) {
        app->installTranslator(&qXmlEditTranslator);
    }
    if(qXmlEditWidgetTranslator.load(QXMLEDITWIDGET_TRANSLATIONS_PREFIX + QLocale::system().name(), ApplicationData::getResourceDir() + QString("/") + QString(STRINGIZE(TRANLASTION_DIR)))) {
        app->installTranslator(&qXmlEditWidgetTranslator);
    }
    if(qXmlEditSessionTranslator.load(QXMLEDITSESSION_TRANSLATIONS_PREFIX + QLocale::system().name(), ApplicationData::getResourceDir() + QString("/") + QString(STRINGIZE(TRANLASTION_DIR)))) {
        app->installTranslator(&qXmlEditSessionTranslator);
    }
}

void installMsgHandler()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    qInstallMessageHandler(LogHandler::msgHandlerCategory);
#else
    qInstallMsgHandler(LogHandler::msgHandler);
#endif
}

static bool licenseAgreement()
{
    if(!Config::getBool(Config::KEY_GENERAL_LICENSE_AGREED, false)) {
        LicenseDialog dlg;
        if(dlg.exec() == QDialog::Accepted) {
            Config::saveBool(Config::KEY_GENERAL_LICENSE_AGREED, true);
            return true ;
        }
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    return internalMain(argc, argv);
}

#endif //QXMLEDIT_NOMAIN

void printHelp()
{
#if !defined(QXMLEDIT_NOMAIN)
    QTextStream stdOut(stdout);
    stdOut << QString("QXmlEdit %1\n").arg(VERSION);
    stdOut << QObject::tr("Usage:\n");
    stdOut << QObject::tr(" -vis <file>: opens the data visualization panel\n");
    stdOut << QObject::tr(" -anonymize <inputfile> <profile> <outputfile>: anonymize\n");
    stdOut << QObject::tr(" any other argument is used as a file to open.\n");
#endif //QXMLEDIT_NOMAIN
}

bool decodeCommandLine(QStringList args, StartParams *params)
{
    if(args.size() > 1) {
        QString arg1 = args.at(1);

        if(arg1 == QString("-vis")) {
            params->type = StartParams::VisFile ;
            if(args.size() > 2) {
                params->fileName = args.at(2);
                if(params->fileName.isEmpty()) {
                    params->parametersError = true ;
                    params->errorMessage = QObject::tr("The input file name is empty.");
                    return false;
                }
            } else {
                params->parametersError = true ;
                params->errorMessage = QObject::tr("Missing parameters for the data visualization option.");
                printHelp();
                return false;
            }
        } else if(arg1 == QString("-anonymize")) {
            params->type = StartParams::Anonymize ;
            if(args.size() > 4) {
                params->fileName = args.at(2);
                params->arg1 = args.at(3);
                params->arg2 = args.at(4);
                if(params->fileName.isEmpty()) {
                    params->parametersError = true ;
                    params->errorMessage = QObject::tr("The input file name is empty.");
                    return false;
                }
                if(params->arg1.isEmpty()) {
                    params->parametersError = true ;
                    params->errorMessage = QObject::tr("The profile name is empty.");
                    return false;
                }
                if(params->arg2.isEmpty()) {
                    params->parametersError = true ;
                    params->errorMessage = QObject::tr("The anonymized file name (output) is empty.");
                    return false;
                }
                params->type = StartParams::Anonymize ;
            } else {
                params->parametersError = true ;
                params->errorMessage = QObject::tr("Missing parameters for the anonymize option.");
                printHelp();
                return false;
            }
        } else {
            params->fileName = args.at(1);
            if(!params->fileName.isEmpty()) {
                params->type = StartParams::OpenFile ;
            } else {
                params->parametersError = true ;
                params->errorMessage = QObject::tr("The input file name is empty.");
                return false;
            }
        }
    }
    return true;
}

