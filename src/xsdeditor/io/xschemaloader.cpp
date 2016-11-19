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
//#define LOG_CONSOLE
#include <xmlEdit.h>
#include "xschemaloader.h"
#include "xsdeditor/xschema.h"
#include "utils.h"
#include "xschemaloaderhelper.h"

//------------------------------------------------
class XSchemaLoader::OperationResults
{
public:
    XSDSchema *schema;
    XSchemaLoader::Code code ;
    bool used ;
    OperationResults()
    {
        used = false;
        schema = NULL ;
        code = XSchemaLoader::SCHEMA_WAIT ;
    }
    ~OperationResults() {}
};

//------------------------------------------------
XSchemaLoaderError::XSchemaLoaderError(const XSchemaLoader::ErrorCode newErrorCode, const QString &newMessage)
{
    _errorCode = newErrorCode ;
    _message = newMessage;
}

XSchemaLoaderError::~XSchemaLoaderError()
{
}

//------------------------------------------------

XSchemaLoader::XSchemaLoader(const bool newPropagateMain, QObject *parent) :
    QObject(parent)
{
    _loadContext = &_internalContext ;
    _externalSchema = false ;
    _logger = NULL;
    _propagateMain = newPropagateMain ;
    _isSchemaProcessed = false;
    _helper = NULL ;
    _schema = NULL ;
    reset();
    _networkAccessManager = NULL ;
    _ownNetworkAccessManager = new QNetworkAccessManager();

}

XSchemaLoader::~XSchemaLoader()
{
    reset();
    if(NULL != _ownNetworkAccessManager) {
        delete _ownNetworkAccessManager ;
    }
    if(NULL != _helper) {
        delete _helper ;
        _helper = NULL ;
    }
}

XSDSchema *XSchemaLoader::schema()
{
    return _schema;
}

QString XSchemaLoader::urlString()
{
    return _schemaURL;
}

XSDLoadContext *XSchemaLoader::loadContext()
{
    return _loadContext;
}

XSDSchema *XSchemaLoader::getSchemaAndTakeOwnership()
{
    XSDSchema *result = _schema;
    _schema = NULL ;
    return result;
}

XSchemaLoader::Code XSchemaLoader::code()
{
    return _code;
}

XSchemaLoader::State XSchemaLoader::state()
{
    return isEnded() ? STATE_READY : STATE_WAIT;
}

bool XSchemaLoader::isError()
{
    return _isError ;
}

XSchemaLoader::State XSchemaLoader::load(XSDLoadContext *newContext, const QString &schemaURL, const bool newIsAsynch, const QString &paramFolderPath, QNetworkAccessManager *newNetworkAccessManager)
{
    reset();

    _loadContext = newContext ;
    registerLocation(schemaURL);
    _schemaURL = schemaURL ;
    _isAsynch = newIsAsynch ;
    _folderPath = paramFolderPath;

    if(NULL == newNetworkAccessManager) {
        _networkAccessManager = ownNetworkAccessManager();
    } else {
        _networkAccessManager = newNetworkAccessManager;
    }
    return processStep();
}

XSchemaLoader::State XSchemaLoader::loadDependent(XSDLoadContext *newContext, XSDSchema *theSchema, const QString &paramFolderPath, QNetworkAccessManager *newNetworkAccessManager)
{
    reset();

    _loadContext = &_internalContext;
    _internalContext.cloneSettingsFrom(newContext);
    _schemaURL = "" ;
    _isAsynch = true ;
    _externalSchema = true ;
    _schema = theSchema ;
    _folderPath = paramFolderPath;
    _state = I_STATE_LOADED ;

    if(NULL == newNetworkAccessManager) {
        _networkAccessManager = ownNetworkAccessManager();
    } else {
        _networkAccessManager = newNetworkAccessManager;
    }
    return processStep();
}

XSchemaLoader::State XSchemaLoader::loadAsChild(XSchemaLoader* theParentLoader, const QString &schemaURL, const bool newIsAsynch, const QString &paramFolderPath, QNetworkAccessManager *newNetworkAccessManager)
{
    reset();

    _loadContext = &_internalContext;
    _internalContext.cloneSettingsFrom(theParentLoader->loadContext());

    registerLocation(schemaURL);
    _parentLoader = theParentLoader;
    _schemaURL = schemaURL ;
    _isAsynch = newIsAsynch ;
    _folderPath = paramFolderPath;

    if(NULL == newNetworkAccessManager) {
        _networkAccessManager = ownNetworkAccessManager();
    } else {
        _networkAccessManager = newNetworkAccessManager;
    }
    return processStep();
}

QNetworkAccessManager* XSchemaLoader::ownNetworkAccessManager()
{
    if(NULL == _ownNetworkAccessManager) {
        _ownNetworkAccessManager = new QNetworkAccessManager();
    }
    return _ownNetworkAccessManager;
}

void XSchemaLoader::clearError()
{
    _isError = false;
    foreach(XSchemaLoaderError * error, _errorsList) {
        delete error;
    }
    _errorsList.clear();
}


FrwLogger *XSchemaLoader::logger() const
{
    return _logger;
}

void XSchemaLoader::setLogger(FrwLogger *logger)
{
    _logger = logger;
}

void XSchemaLoader::reset()
{
    _isSchemaProcessed = false;
    _currentChildSchema = NULL;
    _childLoading = NULL ;
    _parentLoader = NULL ;
    _state = I_STATE_LOADING;
    _isAsynch = false;
    _isWaiting = false;
    clearError();
    _messagesList.clear();
    _folderPath = "" ;
    if((NULL != _schema) && !_externalSchema) {
        delete _schema ;
    }
    _schema = NULL ;
    _includes.clear();
}

void XSchemaLoader::setError(const ErrorCode errorCode, const QString &error)
{
    _isError = true ;
    _errorsList += new XSchemaLoaderError(errorCode, error);
}


XSchemaLoader::State XSchemaLoader::processStep()
{
    process();
    if(isWaiting() || isEnded()) {
        end();
    }
    return externalState();
}

XSchemaLoader::State XSchemaLoader::externalState()
{
    if(isWaiting() || isEnded()) {
        return STATE_READY ;
    }
    return STATE_WAIT ;
}

void XSchemaLoader::end()
{
    _code = SCHEMA_ERROR ;
    if(isWaiting()) {
        _code = SCHEMA_WAIT ;
    } else if((NULL != _schema) && isError()) {
        _code = SCHEMA_WITHERRORS ;
    } else if((NULL != _schema) && !isError()) {
        _code = SCHEMA_READY ;
    } else {
        _code = SCHEMA_ERROR;
    }
    if((NULL != _schema) && !_isSchemaProcessed) {
        _isSchemaProcessed = true;
        _schema->postLoad();
    }
    if(!isWaiting()) {
        emit finished(this, _code);
    }
}

void XSchemaLoader::abort()
{
    if(NULL != _helper) {
        _helper->abort();
    }
    if(NULL != _childLoading) {
        _childLoading ->abort();
    }
}


XSchemaLoader::InternalState XSchemaLoader::process()
{
    bool isContinue = false;
    do {
        isContinue = false ;
        processInternal();
        if(!(isWaiting() || isEnded())) {
            isContinue = true ;
        }

    } while(isContinue);
    return _state;
}

bool XSchemaLoader::isEnded()
{
    return I_STATE_END == _state ;
}

/**
 * @brief self is waiting or there is a child schema loading
 * @return
 */
bool XSchemaLoader::isWaiting()
{
    return _isWaiting || (NULL != _childLoading);
}

bool XSchemaLoader::isAsynchMode() const
{
    return _isAsynch;
}

XSchemaLoader::InternalState XSchemaLoader::processInternal()
{
    switch(_state) {
    default:
        break;
    case I_STATE_LOADING:
        handleLoadingState();
        break;
    case I_STATE_LOADED:
        handleLoadedState();
        break;
    case I_STATE_INCLUDES:
        handleIncludeState();
        break;
    case I_STATE_INCLUDE_WAIT:
        handleIncludeStateWait();
        break;
    case I_STATE_REDEFINE:
        handleRedefineState();
        break;
    case I_STATE_REDEFINE_WAIT:
        handleRedefineStateWait();
        break;
    case I_STATE_IMPORT:
        handleImportState();
        break;
    case I_STATE_IMPORT_WAIT:
        handleImportStateWait();
        break;
    case I_STATE_END:
        // nothing
        break;
    }
    return _state ;
}

XSchemaLoader::InternalState XSchemaLoader::handleLoadedState()
{
    TRACEQ((QString("loaded XSD: '%1'").arg(_schemaURL)));
    if((NULL != _logger) && _logger->isLoggable(FrwLogger::DEBUG)) {
        _logger->debug(QString("loaded XSD: '%1'").arg(_schemaURL));
    }

    _state = I_STATE_END ;
    if(NULL != _schema) {
        _includes = _schema->getIncludes();
        _redefines = _schema->getRedefines();
        _imports = _schema->getImports();
        if(_includes.size() > 0) {
            _state = I_STATE_INCLUDES ;
        } else if(_redefines.size() > 0) {
            _state = I_STATE_REDEFINE ;
        } else if(_imports.size() > 0) {
            _state = I_STATE_IMPORT ;
        }
    }
    return _state;
}

void XSchemaLoader::registerLocation(const QString &newLocation)
{
    mainLoader()->_allFilesLocation.insert(newLocation);
}

bool XSchemaLoader::alreadyLoadedGlobally(const QString &newLocation)
{
    return mainLoader()->_allFilesLocation.contains(newLocation);
}

XSchemaLoader::InternalState XSchemaLoader::handleImportState()
{
    handleImportInternal();
    return _state ;
}

XSchemaLoader::InternalState XSchemaLoader::handleImportStateWait()
{
    processCurrentImport();
    handleImportInternal();
    return _state ;
}

XSchemaLoader::InternalState XSchemaLoader::handleIncludeInternal()
{
    while(_includes.size() > 0) {
        XSchemaInclude *include = _includes.first();
        TRACE("Now loading");
        TRACE(include->schemaLocation().toLatin1().data());
        if(alreadyLoadedGlobally(include->schemaLocation())) {
            if((NULL != _logger) && _logger->isLoggable(FrwLogger::DEBUG)) {
                _logger->debug(QString("skipping already loaded XSD include: '%1'").arg(include->schemaLocation()));
            }
            TRACE("Skipped");
            _includes.removeFirst();
            continue;
        } else {
            TRACE("  handled");
            registerLocation(include->schemaLocation());
            _includeLocations.insert(include->schemaLocation());
            if(loadChildSchema(include->schemaLocation())) {
                processCurrentInclude();
            } else {
                // wait for async load
                _state = I_STATE_INCLUDE_WAIT;
                return _state ;
            }
        }
    }
    _state = I_STATE_REDEFINE;
    return _state ;
}

XSchemaLoader::InternalState XSchemaLoader::handleRedefineInternal()
{
    while(_redefines.size() > 0) {
        XSchemaRedefine *redefine = _redefines.first();
        if(alreadyLoadedGlobally(redefine->schemaLocation())) {
            if((NULL != _logger) && _logger->isLoggable(FrwLogger::DEBUG)) {
                _logger->debug(QString("skipping already loaded XSD redefine: '%1'").arg(redefine->schemaLocation()));
            }
            _redefines.removeFirst();
            continue;
        } else {
            registerLocation(redefine->schemaLocation());
            _includeLocations.insert(redefine->schemaLocation());
            if(loadChildSchema(redefine->schemaLocation())) {
                processCurrentRedefine();
            } else {
                // wait for async load
                _state = I_STATE_REDEFINE_WAIT;
                return _state ;
            }
        }
    }
    _state = I_STATE_IMPORT;
    return _state ;
}

XSchemaLoader::InternalState XSchemaLoader::handleImportInternal()
{
    while(_imports.size() > 0) {
        XSchemaImport *import = _imports.first();
        if(alreadyLoadedGlobally(import->schemaLocation())) {
            setError(ERR_CIRCULAR_REFERENCE, tr("Circular reference detected importing a schema. The path is: %1").arg(pathFromRoot()));
            if((NULL != _logger) && _logger->isLoggable(FrwLogger::DEBUG)) {
                _logger->debug(QString("skipping already loaded XSD import: '%1'").arg(import->schemaLocation()));
            }
            _imports.removeFirst();
            continue;
        } else {
            registerLocation(import->schemaLocation());
            _importLocations.insert(import->schemaLocation());
            if(loadChildSchema(import->schemaLocation(), false)) {
                processCurrentImport();
            } else {
                // wait for async load
                _state = I_STATE_IMPORT_WAIT;
                return _state ;
            }
        }
    }
    _state = I_STATE_END;
    return _state ;
}


XSchemaLoader::InternalState XSchemaLoader::handleIncludeState()
{
    handleIncludeInternal();
    return _state ;
}

XSchemaLoader::InternalState XSchemaLoader::handleIncludeStateWait()
{
    processCurrentInclude();
    handleIncludeInternal();
    return _state ;
}

XSchemaLoader::InternalState XSchemaLoader::handleRedefineState()
{
    handleRedefineInternal();
    return _state ;
}

XSchemaLoader::InternalState XSchemaLoader::handleRedefineStateWait()
{
    processCurrentRedefine();
    handleRedefineInternal();
    return _state ;
}

XSDSchema *XSchemaLoader::mainSchema()
{
    if((NULL == _parentLoader) || !_propagateMain) {
        return _schema ;
    }
    XSchemaLoader *p = _parentLoader;
    while(NULL != p->_parentLoader) {
        p = p->_parentLoader;
    }
    return p->schema();
}

XSDSchema *XSchemaLoader::parentSchema()
{
    XSchemaLoader *p = this ;
    while((NULL != p->_parentLoader) && p->_propagateMain) {
        p = p->_parentLoader;
    }
    return p->schema();
}

XSchemaLoader *XSchemaLoader::mainLoader()
{
    XSchemaLoader *p = this;
    while(NULL != p->_parentLoader) {
        p = p->_parentLoader;
    }
    return p;
}

QString XSchemaLoader::pathFromRoot()
{
    QStringList result;
    XSchemaLoader *p = this ;
    result.prepend(p->urlString());
    while(NULL != p->_parentLoader) {
        p = p->_parentLoader;
        result.prepend(p->urlString());
    }
    return result.join(",");
}

void XSchemaLoader::processCurrentInclude()
{
    if(NULL != _currentChildSchema) {
        XSchemaInclude * currentInclude = _includes.first();
        parentSchema()->processInclude(currentInclude, _currentChildSchema);
        _currentChildSchema = NULL ;
        if((NULL != _logger) && _logger->isLoggable(FrwLogger::DEBUG)) {
            _logger->debug(QString("processing XSD include: '%1'").arg(currentInclude->schemaLocation()));
        }
    }
    if(_includes.size() > 0) {
        _includes.removeFirst();
    }
}

void XSchemaLoader::processCurrentRedefine()
{
    if(NULL != _currentChildSchema) {
        XSchemaRedefine * currentRedefine = _redefines.first();
        parentSchema()->processRedefine(currentRedefine, _currentChildSchema);
        _currentChildSchema = NULL ;
        if((NULL != _logger) && _logger->isLoggable(FrwLogger::DEBUG)) {
            _logger->debug(QString("processing XSD redefine: '%1'").arg(currentRedefine->schemaLocation()));
        }
    }
    if(_redefines.size() > 0) {
        _redefines.removeFirst();
    }
}


void XSchemaLoader::processCurrentImport()
{
    if(NULL != _currentChildSchema) {
        XSchemaImport * currentImport = _imports.first();
        // Not the main schema, but this schema.
        _schema->processImport(currentImport, _currentChildSchema);
        _currentChildSchema = NULL ;
        _importLocations.insert(currentImport->schemaLocation());
        if((NULL != _logger) && _logger->isLoggable(FrwLogger::DEBUG)) {
            _logger->debug(QString("processing XSD import: '%1'").arg(currentImport->schemaLocation()));
        }
    }
    if(_imports.size() > 0) {
        _imports.removeFirst();
    }
}

XSchemaLoader::InternalState XSchemaLoader::handleLoadingState()
{
    QUrl url = QUrl::fromUserInput(_schemaURL);
    if(handleFileAccess(_schemaURL, url)) {
        _state = I_STATE_LOADED ;
    } else {
        if(isAsynchMode()) {
            handleUrlLoadingAsynch(url);
        } else {
            if(handleUrlLoadingDirect(url)) {
                _state = I_STATE_LOADED ;
            } else {
                _state = I_STATE_END ;
            }
        }
    }
    return _state ;
}

bool XSchemaLoader::readFromNetworkReply(QNetworkReply* reply)
{
    bool result = false;
    if(NULL != reply) {
        if(reply->error() == QNetworkReply::NoError) {
            _schema = new XSDSchema(parentSchema());
            if(NULL == _schema) {
                setError(ERROR_MEMORY, tr("Unable to allocate a schema"));
            }
            if(!_isError) {
                if(_schema->readFromIoDevice(loadContext(), reply)) {
                    _schema->setLocation(_schemaURL);
                    result = true ;
                } else {
                    setError(ERROR_SCHEMA_LOADING, tr("Error reading schema data."));
                }
            }
            reply->close();
        } else {
            setError(ERROR_SCHEMA_LOADING, tr("Failed to load XML Schema, error is:'%1'").arg(reply->errorString()));
            Utils::error(reply->errorString());
        }
        reply->deleteLater();
    } else {
        setError(ERROR_SCHEMA_LOADING, tr("Failed network communications."));
    }
    return result ;
}

bool XSchemaLoader::handleUrlLoadingDirect(QUrl &url)
{
    bool result = false;
    XSchemaLoaderHelper helper;
    QNetworkReply* reply = helper.loadSchemaUsingUrl(url, _networkAccessManager, false);
    result = readFromNetworkReply(reply);
    return result ;
}

bool XSchemaLoader::handleUrlLoadingAsynch(QUrl &url)
{
    bool result = false;
    _helper = new XSchemaLoaderHelper();
    connect(_helper, SIGNAL(finished(QNetworkReply*)), this, SLOT(onHelperFinished(QNetworkReply*)));
    connect(_helper, SIGNAL(error(QNetworkReply::NetworkError, const QString &)), this, SLOT(onHelperError(QNetworkReply::NetworkError, const QString &)));
    _isWaiting = true ;
    _helper->loadSchemaUsingUrl(url, _networkAccessManager, true);
    return result ;
}

void XSchemaLoader::onHelperFinished(QNetworkReply* networkReply)
{
    _isWaiting = false ;
    disconnectHelper();
    if(!isError()) {
        if(readFromNetworkReply(networkReply)) {
            _state = I_STATE_LOADED ;
        } else {
            _state = I_STATE_END ;
        }
    } else {
        if(NULL != networkReply) {
            networkReply->deleteLater();
        }
        _state = I_STATE_END ;
    }
    process();
    end();
}

void XSchemaLoader::onHelperError(QNetworkReply::NetworkError error, const QString &errorString)
{
    _isWaiting = false ;
    disconnectHelper();
    setError(ERROR_SCHEMA_LOADING, tr("Error code from loader:%1, '%2'").arg(error).arg(errorString));
    _state = I_STATE_END ;
    process();
    end();
}

void XSchemaLoader::disconnectHelper()
{
    if(NULL != _helper) {
        disconnect(_helper, SIGNAL(finished(QNetworkReply*)), this, SLOT(onHelperFinished(QNetworkReply*)));
        disconnect(_helper, SIGNAL(error(QNetworkReply::NetworkError, const QString &)), this, SLOT(onHelperError(QNetworkReply::NetworkError, const QString &)));
        _helper->deleteLater();
        _helper = NULL ;
    }
}


bool XSchemaLoader::hasProtocol(const QUrl& url)
{
    if(!url.scheme().isEmpty()) {
        return true ;
    }
    return false;
}

bool XSchemaLoader::loadFile(const QString &fileName)
{
    QFile inputFile;
    inputFile.setFileName(fileName);
    if(inputFile.exists()) {
        if(readFromFile(&inputFile)) {
            QFileInfo info(inputFile);
            _folderPath = info.absoluteDir().absolutePath();
            return true;
        }
    }
    return false;
}

bool XSchemaLoader::handleFileAccess(const QString &schemaUrl, const QUrl &url)
{
    //QFile inputFile;
    QString fileName ;

    if(!_folderPath.isEmpty()) {
        fileName = _folderPath;
        fileName += QDir::separator();
    }
    fileName += schemaUrl ;
    if(loadFile(fileName)) {
        return true;
    }
    clearError();
    fileName = schemaUrl ;
    if(loadFile(fileName)) {
        return true;
    }
    clearError();
    if(loadFile(url.toLocalFile())) {
        return true;
    }
    // error
    return false ;
}

bool XSchemaLoader::readFromFile(QFile *inputFile)
{
    bool result = false;
    if(inputFile->open(QFile::ReadOnly)) {
        _schema = new XSDSchema(parentSchema());
        if(NULL == _schema) {
            setError(ERROR_MEMORY, tr("Unable to allocate a schema"));
        } else {
            if(_schema->readFromIoDevice(loadContext(), inputFile)) {
                _schema->setLocation(inputFile->fileName());
                result = true ;
            } else {
                setError(ERROR_SCHEMA_LOADING, tr("Error reading schema data."));
            }
        }
        inputFile->close();
        if(inputFile->error() != QFile::NoError) {
            setError(ERROR_FILE_ACCESS, tr("Error accessing file, error is:'%1'").arg(inputFile->errorString()));
            Utils::error(inputFile->errorString());
        }
    } else {
        setError(ERROR_FILE_OPEN, tr("Failed to load XML Schema, error is:'%1'").arg(inputFile->errorString()));
        Utils::error(inputFile->errorString());
    }
    return result ;
}


bool XSchemaLoader::loadChildSchema(const QString &url, const bool propagateMain)
{
    _currentChildUrl = url ;
    _currentChildSchema = NULL ;
    XSchemaLoader *newChild = new XSchemaLoader(propagateMain);
    registerLocation(url);
    State childState = newChild->loadAsChild(this, url, _isAsynch, _folderPath, _networkAccessManager);
    if(STATE_READY == childState) {
        disconnectChild(newChild);
        // the schema is loaded, process it
        resultFromChild(newChild);
        return true;
    } else {
        connect(newChild, SIGNAL(finished(XSchemaLoader *, const XSchemaLoader::Code)), this, SLOT(onFinishedChild(XSchemaLoader *, const XSchemaLoader::Code)));
        // wait for the child schema asynch
        _childLoading = newChild;
        return false;
    }
}

void XSchemaLoader::resultFromChild(XSchemaLoader *child)
{
    // the schema is loaded, process it
    if(child->code() == SCHEMA_WAIT) {
        return ;
    }
    if(child->code() != SCHEMA_READY) {
        setError(ERROR_SCHEMA_DEPENDENT, tr("Error loading dependent schema:'%1'").arg(_currentChildUrl));
    }
    _currentChildSchema = child->getSchemaAndTakeOwnership();
}

void XSchemaLoader::onFinishedChild(XSchemaLoader *child, const XSchemaLoader::Code /*code*/)
{
    // warning to threading
    disconnectChild(child);
    resultFromChild(child);
    process();
}

bool XSchemaLoader::disconnectChild(XSchemaLoader *child)
{
    if(NULL != child) {
        _childLoading = NULL ;
        disconnect(child, SIGNAL(finished(XSchemaLoader *, const XSchemaLoader::Code)), this, SLOT(onFinishedChild(XSchemaLoader *, const XSchemaLoader::Code)));
        child->deleteLater();
        return true;
    }
    return false;
}
