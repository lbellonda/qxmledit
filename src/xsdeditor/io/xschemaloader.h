/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#ifndef XSCHEMALOADER_SPEC_H
#define XSCHEMALOADER_SPEC_H

#include <QObject>
#include "libQXmlEdit_global.h"
#include "xsdeditor/xschema.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <framework/include/log.h>

class XSchemaLoaderError;
/**
 * @brief Pool of info shared by included schemas
 * this structure is onwed by the main including schema
 * the top level objects live inside their schema along the redifinions
 */

class XSchemaLoaderHelper;
class LIBQXMLEDITSHARED_EXPORT XSchemaLoader : public QObject
{
    Q_OBJECT

    class OperationResults;
public:
    /**
     * @brief operation state
     */

    enum Code {
        SCHEMA_READY,   // the schema is ready
        SCHEMA_ERROR, // fatal error
        SCHEMA_WITHERRORS, // some error, but the schema is loaded
        SCHEMA_WAIT // still loading
    };

    /**
     * @brief registered errors
     */
    enum ErrorCode {
        ERROR_UNKNOWN,
        ERROR_FILE_OPEN,
        ERROR_FILE_ACCESS,
        ERROR_MEMORY,
        ERROR_SCHEMA_LOADING,
        ERROR_SCHEMA_DEPENDENT,
        ERR_CIRCULAR_REFERENCE
    };

    /**
     * @brief The State enum
     * external state
     */
    enum State {
        STATE_READY,   // the schema is ready
        STATE_WAIT // still loading
    };

private:
    enum InternalState {
        I_STATE_LOADING,
        I_STATE_LOADED,
        I_STATE_INCLUDES,
        I_STATE_INCLUDE_WAIT,
        I_STATE_REDEFINE,
        I_STATE_REDEFINE_WAIT,
        I_STATE_IMPORT,
        I_STATE_IMPORT_WAIT,
        I_STATE_END
    };

    QSet<QString> _allSchemas;
    QStack<QString> _currentLoadStack;
    Code _code;
    QString _schemaURL;
    InternalState _state;
    XSchemaLoaderHelper *_helper;
    XSchemaLoader* _parentLoader;
    XSchemaLoader* _childLoading;
    XSDLoadContext *_loadContext;
    XSDLoadContext _internalContext;
    FrwLogger *_logger;
    /**
     * @brief _isWaiting if wating for a network operation
     */
    bool _isWaiting;
    /**
     * @brief _isAsynch asynchronous mode activated
     */
    bool _isAsynch;
    /**
     * @brief _isError some error in the operation
     */
    bool _isError ;
    QList<XSchemaLoaderError*> _errorsList;
    QStringList _messagesList;
    /**
     * @brief _folderPath the folder where the main file or schema is located
     */
    QString _folderPath;
    XSDSchema *_schema;
    bool _externalSchema;
    bool _isSchemaProcessed;
    QNetworkAccessManager *_networkAccessManager;
    /**
     * @brief _ownNetworkAccessManager used if no principal access manager
     */
    QNetworkAccessManager *_ownNetworkAccessManager;

    //-- region(data)--
    QSet<QString> _allFilesLocation;
    QSet<QString> _includeLocations;
    QSet<QString> _importLocations;
    QList<XSchemaInclude*> _includes;
    QList<XSchemaRedefine*> _redefines;
    QList<XSchemaImport*> _imports;
    QString _currentChildUrl;
    XSDSchema *_currentChildSchema;
    //-- endregion(data)--
    bool _propagateMain ;


    void reset();
    State processStep();
    State externalState();
    void end();
    InternalState process();
    void clearError();
    bool isWaiting();
    bool isEnded();
    InternalState processInternal();
    InternalState handleLoadedState();
    InternalState handleLoadingState();
    InternalState handleImportState();
    InternalState handleImportStateWait();
    InternalState handleRedefineState();
    InternalState handleRedefineStateWait();
    InternalState handleIncludeState();
    InternalState handleIncludeStateWait();
    bool hasProtocol(const QUrl& url);
    bool handleFileAccess(const QString &schemaUrl, const QUrl &url);
    bool loadFile(const QString &fileName);
    bool readFromFile(QFile *inputFile);
    void setError(const ErrorCode errorCode, const QString &error);
    bool handleUrlLoadingDirect(QUrl &url);
    bool handleUrlLoadingAsynch(QUrl &url);
    QNetworkAccessManager* ownNetworkAccessManager();
    bool readFromNetworkReply(QNetworkReply* reply);
    void disconnectHelper();
    bool disconnectChild(XSchemaLoader *child);
    State loadAsChild(XSchemaLoader* theParentLoader, const QString &schemaURL, const bool newIsAsynch, const QString &paramFolderPath, QNetworkAccessManager *newNetworkAccessManager);
    bool loadChildSchema(const QString &url, const bool propagateMain = true);
    void resultFromChild(XSchemaLoader *child);
    void processCurrentInclude();
    void processCurrentRedefine();
    void processCurrentImport();
    InternalState handleIncludeInternal();
    InternalState handleRedefineInternal();
    InternalState handleImportInternal();
    XSDSchema *mainSchema();
    XSDSchema *parentSchema();
    XSchemaLoader *mainLoader();
    void registerLocation(const QString &newLocation);
    bool alreadyLoadedGlobally(const QString &newLocation);
    QString pathFromRoot();

public:

    explicit XSchemaLoader(const bool propagateMain = true, QObject *parent = 0);
    ~XSchemaLoader();

    State load(XSDLoadContext *loadContext, const QString &schemaURL, const bool isAsynch, const QString &paramFolderPath, QNetworkAccessManager *networkAccessManager = NULL);
    State loadDependent(XSDLoadContext *loadContext, XSDSchema *theSchema, const QString &paramFolderPath, QNetworkAccessManager *newNetworkAccessManager);
    XSDSchema *schema();
    XSDSchema *getSchemaAndTakeOwnership();
    Code  code();
    State state();
    bool isError();
    bool isAsynchMode() const;
    void abort();
    QString urlString();
    XSDLoadContext *loadContext();

    FrwLogger *logger() const;
    void setLogger(FrwLogger *logger);

signals:
    void finished(XSchemaLoader *entity, const XSchemaLoader::Code code);
public slots:

private slots:
    void onHelperFinished(QNetworkReply* networkReply);
    void onHelperError(QNetworkReply::NetworkError error, const QString &errorString);
    void onFinishedChild(XSchemaLoader *child, const XSchemaLoader::Code code);

};

class LIBQXMLEDITSHARED_EXPORT XSchemaLoaderError
{
    XSchemaLoader::ErrorCode _errorCode;
    QString _message;
public:
    XSchemaLoaderError(const XSchemaLoader::ErrorCode newErrorCode, const QString &newMessage);
    ~XSchemaLoaderError();
};


#endif // XSCHEMALOADER_SPEC_H
