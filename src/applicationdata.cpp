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

#include "applicationdata.h"
#include <QDesktopServices>
#include <QDir>
#include "qxmleditconfig.h"
#include "utils.h"
#include "stylepersistence.h"
#include "uiservices.h"
#include "balsamiqsrc/balsamiqplugin.h"
#include "xsdeditor/xsdplugin.h"
#include "sessions/data_access/testdataaccess.h"
#include "sessions/data_access/sqllitedataaccess.h"
#include "modules/services/systemservices.h"
#include "modules/xsd/namespacemanager.h"
#include "configurationdialog.h"
#include "modules/style/infoonkeyboardshortcutsdialog.h"
#include "modules/help/shortcutsdialog.h"
#include "modules/anonymize/anonimyzebatchdialog.h"
#include "mainwindow.h"

#define HELP_FILE       "QXmlEdit_manual.pdf"

#define VERSION_STRING_PREFIXED  "\0" VERSION_STRING
#define DB_DATA_DIR "dbstorage"
#define DB_FILE_NAME "qxmledit_sessions.db"
#define BACKEND_SESSION_DB 1

const char VERSION[] = { VERSION_STRING_PREFIXED };

int ApplicationData::pluginCode = 0 ;

ApplicationData::ApplicationData()
{
    _dbStarted = false ;
    _dataInterface = NULL ;
    _sessionDataInterface = NULL ;
    _uiServices = NULL ;
    _logger = NULL ;
    pluginCode = 0;
    BalsamiqPlugin *plugin = new BalsamiqPlugin();
    if(NULL == plugin) {
        Utils::error(tr("Unable to load Balsamiq plugin")) ;
    } else {
        pluginCode ++ ;
        _plugins.insert(QString(pluginCode), plugin);
    }
    XsdPlugin *xsdPlugin = new XsdPlugin();
    _xsdPlugin = xsdPlugin;
    if(NULL == xsdPlugin) {
        Utils::error(tr("Unable to load XSD plugin")) ;
    } else {
        pluginCode ++ ;
        _plugins.insert(QString(pluginCode), xsdPlugin);
    }
    configureXsdCache();
    _uiServices = new UIServices() ;
    if(NULL == _uiServices) {
        Utils::error(tr("Unable to initialize the application.")) ;
    }
}

ApplicationData::~ApplicationData()
{
    foreach(IQXmlEditPlugIn * plugin, _plugins) {
        delete plugin ;
    }
    if(NULL != _uiServices) {
        delete _uiServices;
    }
    if(NULL != _sessionDataInterface) {
        _sessionDataInterface->closeAndDispose();
    }
}


void ApplicationData::init()
{
    // already done QXmlEditData::init();
    connect(&_sessionManager, SIGNAL(sessionActivated(const int)), this, SLOT(onSessionActivated(const int)));
    connect(&_sessionManager, SIGNAL(clearSession()), this, SLOT(onClearSession()));
    _sessionManager.setSessionDataFactory(this);
    _sessionManager.init(sessionDBFilePath());
    if(!_sessionManager.isStarted()) {
        Utils::error(tr("Sessions not started"));
    }
    _sessionManager.setEnabled(areSessionsEnabled());
    activateSessionIfEnabled();
    SessionDataInterface *dataInterface = sessionDataInterface("");
    _attributeFilterManagement.setDataAccess(dataInterface);
    _namespaceManager->setDataInterface(storageManager());
}

/** hook for pre-delete
  */
void ApplicationData::end()
{
    QXmlEditData::end();
    _sessionManager.end();
}

QList<MainWindow*> ApplicationData::windows() const
{
    return _windows.toList();
}

MainWindow *ApplicationData::findWindowByPath(const QString &filePath)
{
    foreach(MainWindow * window, _windows) {
        if(window->getRegola()->fileName() == filePath) {
            return window ;
        }
    }
    return NULL ;
}

void ApplicationData::addWindow(MainWindow* newWindow)
{
    if((NULL != newWindow) && !_windows.contains(newWindow)) {
        _windows.append(newWindow);
    }
}

void ApplicationData::removeWindow(MainWindow* newWindow)
{
    if(NULL != newWindow) {
        int index = _windows.indexOf(newWindow);
        if(index >= 0) {
            _windows.remove(index);
        }
    }
}

int ApplicationData::windowsCount()
{
    return _windows.size();
}

MainWindow *ApplicationData::newWindow()
{
    MainWindow *mainWindow = new MainWindow(false, this);
    return mainWindow ;
}

void ApplicationData::updateEditors(const bool invalidateAll)
{
    foreach(MainWindow * window, windows()) {
        if(invalidateAll) {
            window->getEditor()->invalidatePaintData();
        } else {
            window->updateAfterPreferences();
        }
    }
}

void ApplicationData::setSessionDBPath(const QString &sessionDBPath)
{
    _sessionDBPath = sessionDBPath ;
}

void ApplicationData::setLogger(FrwLogger *newLogger)
{
    _logger = newLogger;
    _sessionManager.setLogger(newLogger);
}

FrwLogger *ApplicationData::logger()
{
    return _logger;
}

QString ApplicationData::snippetsLocalDir()
{
    return SystemServices::storageDataLocation() + QDir::separator() + "snippets";
}


QString ApplicationData::snippetsProgramDir()
{
    QString toReturn = getResourceDir() + QDir::separator() + "snippets" ;
    return toReturn;
}

const QMap<QString, IQXmlEditPlugIn*> &ApplicationData::plugins() const
{
    return _plugins ;
}

IQXmlEditPlugIn * ApplicationData::xsdPlugin()
{
    return _xsdPlugin ;
}

//--- region(xsdCache)

bool ApplicationData::isXsdCacheEnabled()
{
    return Config::getBool(Config::KEY_XSDCACHE_ENABLED, false);
}

void ApplicationData::setXsdCacheEnabled(const bool newState)
{
    Config::saveBool(Config::KEY_XSDCACHE_ENABLED, newState);
    configureXsdCache();
}

int ApplicationData::xsdCacheLimit()
{
    return 1000000; // 1Mega
    //TODO enable when it will be possible to configure it. return Config::getInt(Config::KEY_XSDCACHE_LIMIT, 1000000);
}

void ApplicationData::configureXsdCache()
{
    if(isXsdCacheEnabled()) {
        QNetworkDiskCache *xsdDiskCache = new QNetworkDiskCache();
        if(NULL == xsdDiskCache) {
            Utils::error(tr("Error while getting a disk cache object."));
            return ;
        }
        QString cacheProgramDirectoryPath = cacheProgramDirectory();
        xsdDiskCache->setMaximumCacheSize(xsdCacheLimit());
        xsdDiskCache->setCacheDirectory(cacheProgramDirectoryPath);
        _networkAccessManager.setCache(xsdDiskCache);
    } else {
        _networkAccessManager.setCache(NULL);
    }
}

//--- endregion(xsdCache)

//--- region(welcomeDialog)
void ApplicationData::enableWelcomeDialog(const bool enabled)
{
    Config::saveBool(Config::KEY_WELCOMEDIALOG_ENABLED, enabled) ;
    Config::saveInt(Config::KEY_WELCOMEDIALOG_VERSION, VERSION_NUMBER_WELCOME) ;
}

bool ApplicationData::isWelcomeDialogEnabled()
{
    return Config::getBool(Config::KEY_WELCOMEDIALOG_ENABLED, false) ;
}

bool ApplicationData::shouldShowWelcomeDialog()
{
    bool welcomeEanbled = isWelcomeDialogEnabled();
    int welcomeVersion = Config::getInt(Config::KEY_WELCOMEDIALOG_VERSION, 0) ;
    if(welcomeEanbled
            || (welcomeVersion < VERSION_NUMBER_WELCOME)) {
        return true ;
    }
    return false;
}

//--- endregion(welcomeDialog)

//--- region(behaviour)
void ApplicationData::enableAutoscroll(const bool enabled)
{
    Config::saveBool(Config::KEY_GENERAL_AUTOSCROLL_ENABLED, enabled) ;
}


bool ApplicationData::isAutoscroll()
{
    return Config::getBool(Config::KEY_GENERAL_AUTOSCROLL_ENABLED, true) ;
}

//--- endregion(behaviour)

//--- region(session)
SessionManager *ApplicationData::sessionManager()
{
    return &_sessionManager;
}

void ApplicationData::enableSessions(const bool enabled)
{
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, enabled) ;
    _sessionManager.setEnabled(enabled);
}

bool ApplicationData::areSessionsEnabled()
{
    return Config::getBool(Config::KEY_SESSIONS_ENABLED, true) ;
}

void ApplicationData::setActiveSessionCode(const int code)
{
    Config::saveInt(Config::KEY_SESSIONS_ACTIVECODE, code) ;
}

void ApplicationData::resetActiveSessionCode()
{
    Config::saveInt(Config::KEY_SESSIONS_ACTIVECODE, 0) ;
}

int ApplicationData::activeSessionCode()
{
    return Config::getInt(Config::KEY_SESSIONS_ACTIVECODE, 0) ;
}

Session::SessionState ApplicationData::activeSessionState()
{
    return (Session::SessionState)Config::getInt(Config::KEY_SESSIONS_ACTIVESTATE, Session::NoSession) ;
}

void ApplicationData::setActiveSessionState(const Session::SessionState state)
{
    Config::saveInt(Config::KEY_SESSIONS_ACTIVESTATE, state) ;
}

QString ApplicationData::sessionDBFilePath()
{
    if(!_sessionDBPath.isEmpty()) {
        return _sessionDBPath;
    }
    // db coordinates
    QString dirPath = dataDirectory();
    if(!dirPath.isEmpty()) {
        dirPath.append("/");
    }
    dirPath.append(DB_DATA_DIR);
    QDir qDir;
    if(!qDir.mkpath(dirPath)) {
        Utils::error(tr("Error accessing sessions folder."));
        return "";
    }
    dirPath.append("/");
    dirPath.append(DB_FILE_NAME);
    _sessionDBPath = dirPath ;
    return dirPath;
}

void ApplicationData::activateSessionIfEnabled()
{
    if(_sessionManager.isEnabled()) {
        int idSession = activeSessionCode();
        if(0 != idSession) {
            Session::SessionState sessionState = activeSessionState();
            if(!_sessionManager.setActiveSession(idSession, sessionState)) {
                Utils::error(tr("Error restoring session state"));
            }
        } else {
            _lastFilesNames.clear();
            Config::loadStringArray(Config::KEY_MAIN_RECENTFILES, _lastFilesNames);
            if(!_sessionManager.setDefaultSession(_lastFilesNames)) {
                Utils::error(tr("Error starting default session."));
            }
        }
    }
}

void ApplicationData::onSessionActivated(const int idSession)
{
    if(areSessionsEnabled()) {
        if(idSession > 0) {
            setActiveSessionCode(idSession);
        }
    }
}

void ApplicationData::onClearSession()
{
    if(areSessionsEnabled()) {
        setActiveSessionCode(0);
    }
}

//--- endregion(session)


//--- region(SessionDataFactory)
// TODO: investigate for leak to confirm.
DataInterface *ApplicationData::storageManager()
{
    setupStorage();
    return _dataInterface;
}

SessionDataInterface *ApplicationData::sessionDataInterface(const QString & /*configuration*/)
{
    setupStorage();
    return _sessionDataInterface;
}

void ApplicationData::setupStorage()
{
    if(_dbStarted) {
        return ;
    }
    _dbStarted = true ;
#if defined(BACKEND_SESSION_DB)
    SQLLiteDataAccess *dataAccess = new SQLLiteDataAccess();
    dataAccess->setLogger(_logger);
    if(!dataAccess->init(_sessionDBPath)) {
        delete dataAccess ;
        return ;
    }
    setStorageManager(dataAccess);
    _sessionDataInterface = dataAccess;
#else
    TestDataAccess testData = new TestDataAccess();
    _sessionDataInterface = testData;
    setStorageManager(testData);
#endif
}

//--- endregion(SessionDataFactory)

UIServices* ApplicationData::uiServices()
{
    return _uiServices ;
}


void ApplicationData::setUiServices(UIServices* value)
{
    _uiServices = value ;
}


AttributeFilterManagement *ApplicationData::attributeFilterManagement()
{
    return &_attributeFilterManagement;
}

//--- region(xsl)
bool ApplicationData::isUseSaxonXSL()
{
    return Config::getBool(Config::KEY_XSL_USE_SAXON, false);
}

void ApplicationData::setUseSaxonXSL(const bool newValue)
{
    Config::saveBool(Config::KEY_XSL_USE_SAXON, newValue);
}

QString ApplicationData::saxonXSLPath()
{
    return Config::getString(Config::KEY_XSL_SAXON_PATH, "");
}

void ApplicationData::setSaxonXSLPath(const QString &newValue)
{
    Config::saveString(Config::KEY_XSL_SAXON_PATH, newValue);
}

//--- endregion(xsl)


//--- region(baseEditMode)
bool ApplicationData::evaluateConditionForShowShortcuts()
{
    Utils::TEST_ME("test me");
    if(areEditShortcutsUsed() || isShortcutUsedDialogShown()) {
        return false;
    }
    const uint times = getElementDoubleClickedInSessionCount() ;
    if(times > LimitForShowShortcutsDialog) {
        return true;
    }
    return false;
}

bool ApplicationData::isShortcutUsedDialogShown()
{
    return Config::getBool(Config::KEY_ELEMENT_EDIT_SHORTCUT_DIALOG_SHOWN, false);
}

void ApplicationData::setShortcutUsedDialogShown()
{
    if(!Config::getBool(Config::KEY_ELEMENT_EDIT_SHORTCUT_DIALOG_SHOWN, false)) {
        Config::saveBool(Config::KEY_ELEMENT_EDIT_SHORTCUT_DIALOG_SHOWN, true);
    }
}

void ApplicationData::resetBehaviorData()
{
    Config::saveBool(Config::KEY_ELEMENT_EDIT_SHORTCUT_DIALOG_SHOWN, false);
    Config::saveBool(Config::KEY_ELEMENT_EDIT_SHORTCUT_USED, false);
    Config::saveBool(Config::KEY_ELEMENT_EDIT_MODEFORM_MODIFIED, false);
    Config::saveBool(Config::KEY_ELEMENT_EDIT_TYPE_DIALOG_SHOWN, false);
    Utils::TEST_ME("");
    resetUserGuidedOperation();
    resetUserFirstAccess();
}

//--- endregion(baseEditMode)

//--- region(access)

bool ApplicationData::isUserFirstAccess()
{
    return Config::getBool(Config::KEY_USERPROFILING_FIRSTUSE, true);
}

void ApplicationData::resetUserFirstAccess()
{
    Config::saveBool(Config::KEY_USERPROFILING_FIRSTUSE, true);
}

void ApplicationData::fireUserFirstAccess()
{
    Config::saveBool(Config::KEY_USERPROFILING_FIRSTUSE, false);
}

bool ApplicationData::isUserGuidedOperation()
{
    return Config::getBool(Config::KEY_USERPROFILING_GUIDED, false);
}

void ApplicationData::setUserGuidedOperation(const bool value)
{
    Config::saveBool(Config::KEY_USERPROFILING_GUIDED, value);
}

void ApplicationData::resetUserGuidedOperation()
{
    Config::saveBool(Config::KEY_USERPROFILING_GUIDED, false);
}

void ApplicationData::setUserType(const EUserType userType)
{
    Config::saveBool(Config::KEY_USERPROFILING_GUIDED, UserTypeGuided == userType);
    Config::saveInt(Config::KEY_USERPROFILING_USERTYPE, userType);
}

bool ApplicationData::testAndMarkFirstAccessForViewPreferences()
{
    if(!Config::getBool(Config::KEY_GENERAL_VIEW_EDITOR_ADJUST, false)) {
        Config::saveBool(Config::KEY_GENERAL_VIEW_EDITOR_ADJUST, true);
        return false ;
    }
    return true;
}

//--- endregion(access)

bool ApplicationData::preferences(QWidget *parent)
{
    int version = styleVersion();
    ConfigurationDialog::doOptions(parent, this) ;
    int nowVersion = styleVersion();
    if(version != nowVersion) {
        return true ;
    }
    return false;
}

void ApplicationData::showUserManual()
{
    QString resourceHelp = getDocsDir() + "/" + HELP_FILE ;
    QDesktopServices::openUrl(QUrl::fromLocalFile(resourceHelp));
}

void ApplicationData::showEditingShortcuts(QWidget *parent)
{
    InfoOnKeyboardShortcutsDialog infoDialog(parent);
    infoDialog.exec();
    if(infoDialog.isOpenShortcutDialog()) {
        ShortcutsDialog::display(parent);
    }
    setShortcutUsedDialogShown();
}

void ApplicationData::anonymizeFile(QWidget *window)
{
    AnonimyzeBatchDialog dlg(this, window);
    dlg.setModal(true);
    dlg.exec();
}
