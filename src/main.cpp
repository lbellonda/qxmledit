/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#include <QApplication>
#include "xmlEdit.h"
#include "xmleditglobals.h"
#include "config.h"
#include "mainwindow.h"
#include "element.h"
#include "utils.h"
#include "StartParams.h"
#include "services/loghandler.h"
#include <QtGlobal>
#include "qxmleditapplication.h"
#include <QMenu>
#include "licensedialog.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,3)
#if defined(ENVIRONMENT_MACOS)
#define MACOS_SPECIFIC
#endif
#endif


#if defined(MACOS_SPECIFIC)
#include <QtMac>
#endif

#if !defined(QXMLEDIT_NOMAIN)

const char *APP_TITLE = QT_TR_NOOP("QXmlEdit");

#define QXMLEDIT_TRANSLATIONS_PREFIX "QXmlEdit_"
#define QXMLEDITWIDGET_TRANSLATIONS_PREFIX  "QXmlEditWidget_"
#define QXMLEDITSESSION_TRANSLATIONS_PREFIX "QXmlEditSessions_"

static bool decodeCommandLine(QApplication &app, StartParams *params);
static void startTanslator(QApplication *app);
static void initLogger();
static void todo();
static void installMsgHandler();
static void addMenuExtra(QXmlEditApplication *app, MainMenuBlock *mainMenuBlock);
static void removeMenuExtra(QXmlEditApplication *app, MainMenuBlock *mainMenuBlock);
static bool licenseAgreement();

static QTranslator qtLibTranslator;
static QTranslator qXmlEditTranslator;
static QTranslator qXmlEditWidgetTranslator;
static QTranslator qXmlEditSessionTranslator;

static LogHandler logHandler;
static StartParams startParams ;

int main(int argc, char *argv[])
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

    todo();

    Element::loadIcons();
    app.setWindowIcon(QIcon(":/tree/icon.png"));

    if(!licenseAgreement()) {
        return -1 ;
    }

    decodeCommandLine(app, &startParams);
    MainWindow mainWindow(false, &app, &appData);

    mainWindow.show();
    switch(startParams.type) {
    default:
    case StartParams::Nothing:
        mainWindow.triggersWelcomeDialog();
        break;
    case StartParams::OpenFile:
        mainWindow.loadFile(startParams.fileName);
        break;
    case StartParams::VisFile:
        mainWindow.loadVisFile(startParams.fileName);
        break;
    }
    app.connect(appData.notifier(), SIGNAL(newWindowRequested()), &app, SLOT(onNewWindow()));
    app.connect(appData.notifier(), SIGNAL(encodingToolsRequested()), &app, SLOT(onEncodingTools()));
    app.connect(appData.notifier(), SIGNAL(codePageToolsRequested()), &app, SLOT(onCodePagesTools()));
    app.connect(appData.notifier(), SIGNAL(sessionRequested()), &app, SLOT(onManageSessions()));
    app.connect(appData.notifier(), SIGNAL(splitFileRequested()), &app, SLOT(onSplitFile()));
    app.connect(appData.notifier(), SIGNAL(viewMapRequested()), &app, SLOT(onViewData()));
    MainMenuBlock mainMenuBlock;
    addMenuExtra(&app, &mainMenuBlock);
    int result = app.exec();
    removeMenuExtra(&app, &mainMenuBlock);
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

#if defined(MACOS_SPECIFIC)

#endif
}

static void initLogger()
{
    bool isEnabled = Config::getBool(Config::KEY_LOGS_ENABLED, false);
    int logLevel = Config::getInt(Config::KEY_LOGS_LEVEL, FrwLogger::INFO);
    logHandler.setLevel((FrwLogger::ELogLevel)logLevel);
    logHandler.setEnabled(isEnabled);
}

static bool decodeCommandLine(QApplication &app, StartParams *params)
{
    QStringList args = app.arguments();
    if(args.size() > 1) {
        QString arg1 = args.at(1);

        if(arg1 == QString("-vis")) {
            if(args.size() > 2) {
                params->fileName = args.at(2);
                if(!params->fileName.isEmpty()) {
                    params->type = StartParams::VisFile ;
                    return true;
                } else {
                    params->parametersError = true ;
                }
            }
        } else {
            params->fileName = args.at(1);
            if(!params->fileName.isEmpty()) {
                params->type = StartParams::OpenFile ;
                return true;
            } else {
                params->parametersError = true ;
            }
        }
    }
    return false;
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
#include <QMenu>
void todo()
{
#if defined(MACOS_SPECIFIC) && defined(VERSION_IS_SNAPSHOT)
    QtMac::setBadgeLabelText("Beta");
#endif
    Utils::TODO_THIS_RELEASE("check searchlets");
    Utils::TODO_THIS_RELEASE("progetto sessions: gestire header precompilati");
    Utils::TODO_THIS_RELEASE("abilita indenta a livello di editor");
    Utils::TODO_THIS_RELEASE("manca info su sequence e choice");
    Utils::TODO_THIS_RELEASE("mettere i tooltip sui pulsanti del widget");
    Utils::TODO_THIS_RELEASE("fare mappa sito e decidere cosa va nel sito e cosa no");
    Utils::TODO_THIS_RELEASE("mettere i tutorial nell;altro sito.");
    Utils::TODO_THIS_RELEASE("xsd: liste esagonali non centrate con il testo");
    Utils::TODO_THIS_RELEASE("xsd: metti i tooltip a tutti gli oggetti xsd");
    Utils::TODO_THIS_RELEASE("fai test per nsprefix multipli");
    Utils::TODO_THIS_RELEASE("fai test manuale per apertura dialog");
    Utils::TODO_THIS_RELEASE("fai test per edit nootation con xsd di fop");
    Utils::TODO_THIS_RELEASE("attenzione che qualche item ha gia il suo tooltip, controlla es: reference o import");
    Utils::TODO_THIS_RELEASE("manca icon info su annotazione choice e sequence");
    Utils::TODO_THIS_RELEASE("a bvolte non fa cancel su edit inner");
    Utils::TODO_THIS_RELEASE("viewxsd non chiude con cancel");
    Utils::TODO_THIS_RELEASE("fattorizza il Idff");
    Utils::TODO_THIS_RELEASE("edit annotation non fa il focus su start");
    Utils::TODO_THIS_RELEASE("compara xml tooltip deve mostrare full di entrambi i rami");
    Utils::TODO_THIS_RELEASE("fai un previous location");
    Utils::TODO_THIS_RELEASE("pagina di Github deve fare il redirect su .org");
    Utils::TODO_THIS_RELEASE("gallery: fare versione immagini a bassa risoluzione e link ad alta risoluzione");
    Utils::TODO_THIS_RELEASE("test per validazione");
    Utils::TODO_THIS_RELEASE("elimina validation results dialog");
    Utils::TODO_THIS_RELEASE("il pulsante valida deve validare usando l'ultima opzione (file o reference)");
    Utils::TODO_THIS_RELEASE("messaggio di errore di validazione scompare troppo presto.");
    Utils::TODO_THIS_RELEASE("Indica qual'e' l'XSD che verra' utilizzato per la validazione");
    Utils::TODO_THIS_RELEASE("fare test: 1- no schema?");
    Utils::TODO_THIS_RELEASE("2- wrong attributo, wrong element e cotnrolla errori");
    Utils::TODO_THIS_RELEASE("ma attenzione alla finestra di load file che si apre: bad for tests");

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

#endif //QXMLEDIT_NOMAIN
