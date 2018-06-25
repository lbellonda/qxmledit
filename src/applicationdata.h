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
#ifndef APPLICATIONDATA_H
#define APPLICATIONDATA_H

#include "xmlEdit.h"
#include "style.h"
#include "plugin.h"

#include "qxmleditdata.h"
#include "sessionmanager"
#include "sessiondataaccess"
#include "log.h"
#include "filter/attributefiltermanagement.h"

#include <QtNetwork/QNetworkDiskCache>
#include <QtNetwork/QNetworkAccessManager>

class XsdPlugin;
class UIServices;
class MainWindow;

class ApplicationData : public QXmlEditData, public SessionDataFactory
{
    Q_OBJECT

    static int pluginCode;
    QMap<QString, IQXmlEditPlugIn*> _plugins;
    XsdPlugin *_xsdPlugin ;
    SessionManager _sessionManager;
    FrwLogger *_logger;
    QString _sessionDBPath;
    QVector<MainWindow*> _windows;
    UIServices *_uiServices;
    AttributeFilterManagement _attributeFilterManagement;
    SessionDataInterface *_sessionDataInterface;
    bool _dbStarted;
    MainWindow *_lastActivatedWindow;
    QWidget *_keyInfoWidget;

private:
    void setupStorage();


public:
    ApplicationData();
    virtual ~ApplicationData();

    virtual void init();
    virtual void end();

    void addWindow(MainWindow* newWindow);
    void removeWindow(MainWindow* newWindow);
    int windowsCount();
    QList<MainWindow*> windows() const ;
    MainWindow *findWindowByPath(const QString &filePath);
    bool isValidWindow(MainWindow *window);
    MainWindow *newWindow();

    void updateEditors(const bool invalidateAll = false);

    void setLogger(FrwLogger *newLogger);
    void setSessionDBPath(const QString &sessionDBPath);
    FrwLogger *logger();

    const QMap<QString, IQXmlEditPlugIn*> &plugins() const ;

    QString snippetsLocalDir();
    QString snippetsProgramDir();

    IQXmlEditPlugIn *xsdPlugin();

    UIServices* uiServices();
    void setUiServices(UIServices* value);

    //--- region(xsdCache)

    bool isXsdCacheEnabled();
    void setXsdCacheEnabled(const bool newState);
    int xsdCacheLimit();
    void configureXsdCache();
    //--- endregion(xsdCache)

    //--- region(welcomeDialog)
    void enableWelcomeDialog(const bool enabled);
    bool isWelcomeDialogEnabled();
    bool shouldShowWelcomeDialog();
    //--- endregion(welcomeDialog)

    //--- region(behaviour)
    void enableAutoscroll(const bool enabled);
    bool isAutoscroll();
    //--- endregion(behaviour)

    //--- region(session)
    SessionManager *sessionManager();
    void enableSessions(const bool enabled);
    bool areSessionsEnabled();
    QString sessionDBFilePath();
    int activeSessionCode();
    void resetActiveSessionCode();
    void setActiveSessionCode(const int code);
    Session::SessionState activeSessionState();
    void setActiveSessionState(const Session::SessionState state);
    void activateSessionIfEnabled();
    //--- endregion(session)

    //--- region(SessionDataFactory)
    SessionDataInterface *sessionDataInterface(const QString &configuration) ;
    virtual DataInterface *storageManager();
    //--- endregion(SessionDataFactory)

    //--- region(xsl)
    bool isUseSaxonXSL() ;
    void setUseSaxonXSL(const bool newValue) ;
    QString saxonXSLPath() ;
    void setSaxonXSLPath(const QString &newValue) ;
    //--- endregion(xsl)

    //--- region(baseEditMode)
    static const uint LimitForShowShortcutsDialog = 4 ;
    bool evaluateConditionForShowShortcuts();
    bool isShortcutUsedDialogShown();
    void setShortcutUsedDialogShown();
    void resetBehaviorData();
    //--- endregion(baseEditMode)

    //--- region(access)
    bool isUserFirstAccess();
    void resetUserFirstAccess();
    void fireUserFirstAccess();
    bool isUserGuidedOperation();
    void setUserGuidedOperation(const bool value);
    void resetUserGuidedOperation();
    bool testAndMarkFirstAccessForViewPreferences();
    //--- endregion(access)

    //--- region(keyinfo)
    bool _shortcutPanelState;
    void requestShowHideKeyboardInfo();
    void newStateKeyboardInfo(const bool newState);
    bool keyboardInfoState();
    QWidget *keyboardInfoWidget();
    void setKeyboardInfoWidget(QWidget *theWidget);
    QRect keyboardInfoGeometry();
    //--- endregion(keyinfo)

    enum EUserType {
        UserTypeExpert = 0,
        UserTypeGuided = 1
    } ;
    void setUserType(const EUserType userType);
    //--- endregion(access)

    /// @return true if paint data are changed
    bool preferences(QWidget *parent);

    void showUserManual();
    void showEditingShortcuts(QWidget *parent);
    void anonymizeFile(QWidget *window);
    void showUserTypePanel();
    void emitOpenUserGuidedPanel();
    void chooseVisualDetail();
    void setDefaultViewDetail();

    AttributeFilterManagement *attributeFilterManagement();

    void newWindowActivationStatus(MainWindow *window, const bool activated);
    void activateShortcut(const QString & actionName);
    void newSelectionState(MainWindow *window);

private slots:
    void onSessionActivated(const int idSession);
    void onClearSession();

signals:
    void openUserGuidedPanel();
    void openUserTypePanel(const bool firstAccess);
    void windowActivated(MainWindow *window, bool activated);
    void stateKeyboardShortcutChanged(bool isVisible);
    void keyboardShortcutOpenCloseRequest();
    void requestEnableKeys(MainWindow * window);
    void windowsCountChanged(int count);
};

#endif // APPLICATIONDATA_H
