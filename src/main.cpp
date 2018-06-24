/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
#include "modules/xslt/xsltexecutor.h"
#include "modules/services/startactionsengine.h"
#include <QMessageBox>
#include <QTimer>
#include "licensedialog.h"

extern const char *APP_TITLE ;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,3)
#if defined(ENVIRONMENT_MACOS)
#define MACOS_SPECIFIC
#endif
#endif

int doXSL(ApplicationData *appData, StartParams *startParams);

#if defined(MACOS_SPECIFIC)
#include <QtMac>
#endif

#if !defined(QXMLEDIT_NOMAIN)
const char *APP_TITLE = QT_TR_NOOP("QXmlEdit");
#endif //QXMLEDIT_NOMAIN

#define QXMLEDIT_TRANSLATIONS_PREFIX "QXmlEdit_"
#define QXMLEDITWIDGET_TRANSLATIONS_PREFIX  "QXmlEditWidget_"
#define QXMLEDITSESSION_TRANSLATIONS_PREFIX "QXmlEditSessions_"
#define SCXML_TRANSLATIONS_PREFIX "SCXML_"

void printHelp();

bool decodeCommandLine(QStringList args, StartParams *params);
static void startTanslator(QApplication *app);
static void initLogger();
static void todo();
static void experimental();
static void installMsgHandler();
static void addMenuExtra(QXmlEditApplication *app, MainMenuBlock *mainMenuBlock);
static void removeMenuExtra(QXmlEditApplication *app, MainMenuBlock *mainMenuBlock);
static bool licenseAgreement();
static int doAnonymize(QXmlEditApplication *app, StartParams &startParams);
static bool handleCommandLineArguments(QXmlEditApplication &app, StartParams &startParams);

static QTranslator qtLibTranslator;
static QTranslator qXmlEditTranslator;
static QTranslator qXmlEditWidgetTranslator;
static QTranslator qXmlEditSessionTranslator;

static LogHandler logHandler;

int internalMain(int argc, char *argv[])
{
    StartParams startParams ;
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
    experimental();
    Element::loadIcons();
    app.setWindowIcon(QIcon(":/tree/icon.png"));

    if(!licenseAgreement()) {
        return -1 ;
    }

    bool decoded = startParams.decodeCommandLine(app.arguments());
    if(app.handleSingleInstance(&startParams)) {
        return 0;
    }
    if(startParams.type == StartParams::Anonymize) {
        int returnCode = doAnonymize(&app, startParams);
        return returnCode;
    }
    if(startParams.type == StartParams::XSLExec) {
        int returnCode = doXSL(&appData, &startParams);
        return returnCode;
    }

    if(!decoded) {
        printHelp();
    }

    if(!handleCommandLineArguments(app, startParams)) {
        StartActionsEngine startActionsEngine(&appData, &app);
        startActionsEngine.execute(startParams);
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

bool handleCommandLineArguments(QXmlEditApplication &app, StartParams &startParams)
{
    bool handled = false;
    switch(startParams.type) {
    default:
        break;
    case StartParams::VisFile: {
        MainWindow *mainWindow = new MainWindow(false, app.appData());
        mainWindow->show();
        mainWindow->loadVisFile(startParams.fileName);
        handled = true ;
    }
    break;
    }
    return handled;
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
        printHelp();
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
}

void experimental()
{
#if defined(QXMLEDIT_VERSION_IS_SNAPSHOT)
    QMessageBox warningBox(QMessageBox::Warning, APPLICATION_NAME,
                           QObject::tr("This is not a released version.\nThis is a developer preview."));
    QTimer::singleShot(2500, &warningBox, SLOT(hide()));
    warningBox.exec();
#endif
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
    if(qXmlEditWidgetTranslator.load(SCXML_TRANSLATIONS_PREFIX + QLocale::system().name(), ApplicationData::getResourceDir() + QString("/") + QString(STRINGIZE(TRANLASTION_DIR)))) {
        app->installTranslator(&qXmlEditWidgetTranslator);
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

#if !defined(QXMLEDIT_NOMAIN)

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
    stdOut << QObject::tr(" -xsl [-saxon] -xsl=<xslFile> -output=<outputfile>  {-p<name>=<value>}* <inputfile>: execute XSL transformation\n");
    stdOut << QObject::tr(" any other argument is used as a file to open.\n");
#endif //QXMLEDIT_NOMAIN
}

int doXSL(ApplicationData *appData, StartParams *startParams)
{
    Utils::setBatch(true);
    QTextStream stdErr(stderr);
    QTextStream stdOut(stdout);
    if(startParams->parametersError) {
        stdErr << startParams->errorMessage ;
        printHelp();
        return -1;
    }
    MessagesOperationResult result;
    bool success = XSLTExecutor::execBatch(appData,
                                           result,
                                           startParams->fileName, startParams->xsl,
                                           startParams->outputFile,
                                           startParams->params,
                                           startParams->forceSaxon);
#if !defined(QXMLEDIT_NOMAIN)
    foreach(SourceMessage* message, *result.messages()) {
        if(message->type() == SourceMessage::Error) {
            stdErr << QObject::tr("[ERROR]:");
        } else {
            stdErr << QObject::tr("[Output]:");
        }
        stdErr << message->description();
        stdErr << "\n";
    }
    if(success) {
        stdOut << QObject::tr("Success\n");
        return 0;
    } else {
        stdOut << QObject::tr("Error\n");
        return -1 ;
    }
#else
    if(success) {
        return 0;
    } else {
        return -1 ;
    }
#endif //QXMLEDIT_NOMAIN
}
