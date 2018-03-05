/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef QXMLEDITAPPLICATION_H
#define QXMLEDITAPPLICATION_H

#include "xmlEdit.h"
#include <QLocalServer>
#include "log.h"
#include "operationresult.h"
#include "modules/services/startactionsengine.h"

class ApplicationData;
class StartParams;
class MainWindow;
class GuidedOperationsDialog;
class GuidedValidationDialog;

class QXmlEditApplication : public QApplication, StartActionsExecutor
{
    Q_OBJECT
private:
    static const QString ServerName;
    static const int ConnectionTimeout = 100 ;
    static const int ReadTimeOutMs = 100 ;

    ApplicationData *_appData;
    QLocalServer *_server;
    FrwLogger *_logger;
    GuidedOperationsDialog *_guidedOperationsDialog;
    GuidedValidationDialog *_guidedValidationDialog;
public:
    explicit QXmlEditApplication(int &argc, char **argv);
    virtual ~QXmlEditApplication();

    ApplicationData *appData() const;
    void setAppData(ApplicationData *appData);

    bool handleSingleInstance(StartParams *startParams);

    FrwLogger *logger() const;
    void setLogger(FrwLogger *logger);

    OperationResult *anonymizeBatch(const QString &newFileInputPath, const QString &newProfileName, const QString &newFileOutputPath);
    bool handleFirstAccess();
    void setupFirstAccessForPreferences();
    MainWindow *getOrCreateMainWindow();

    // region(StartAtcionsExecutor)
    virtual void startActionShowUserTypePanel();
    virtual bool startActionShowGuidedOperationsPanel();
    virtual void startActionSetupFirstAccessForPreferences();
    virtual void startActionTriggersWelcomeDialog();
    virtual void startActionLoadFile(const QString &fileName);
    // endregion(StartAtcionsExecutor)

protected:
    bool event(QEvent *event);
    bool startServer();
    bool connectToExistingServer(StartParams *startParams);
    QByteArray paramsToByteArray(StartParams *startParams);
    MainWindow *makeNewWindow();
    bool errorCloseConnection(QLocalSocket *client);
    bool showUserTypePanel();
    bool showGuidedOperationsPanel();
    bool showValidationOperationsPanel();
    void connectToCommandsPanel(const bool isConnect, GuidedOperationsDialog *target);
    void bindCommandOperation(const bool isConnect, const QObject *sender, const char *signal, const char *method);
    void taskChooseDetail();

public slots:
    void onNewWindow();
    void onCodePagesTools();
    void onManageSessions();
    void onEncodingTools();
    void onSplitFile();
    void onViewData();
    void onRaiseWindows();
private slots:
    void newServerConnection();
    void onCommandNew();
    void onCommandQuit();
    void onCommandOpen();
    void onCommandValidate();

};

#endif // QXMLEDITAPPLICATION_H
