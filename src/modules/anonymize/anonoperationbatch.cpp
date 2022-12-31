/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2022 by Luca Bellonda and individual contributors  *
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

#include "anonoperationbatch.h"
#include <QXmlReader>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include "utils.h"
#include "anoncontext.h"
#include "element.h"
#include "xmlutils.h"
#include "modules/anonymize/xmlanonutils.h"
#include "modules/anonymize/algstat/anonalgstatcontext.h"
#include "utils.h"

AnonOperationResult::AnonOperationResult()
{
    reset();
}

void AnonOperationResult::reset()
{
    _ok = true ;
    _code = RES_OK ;
}

AnonOperationResult::~AnonOperationResult()
{
}

void AnonOperationResult::setError(const ECode newCode, const QString &msg)
{
    _ok = false;
    _code = newCode ;
    if(RES_OK == _code) {
        _code = RES_ERR_UNSPECIFIED ;
    }
    _message = msg ;
}

void AnonOperationResult::setMessage(const ECode newCode, const QString &msg, const bool isError)
{
    if(!_ok) {
        // do not overwrite errors
        return ;
    }
    if((RES_OK != newCode) || isError) {
        setError(newCode, msg);
    } else {
        _code = newCode ;
        _message = msg ;
    }
}

bool AnonOperationResult::isError() const
{
    return !_ok ;
}

QString AnonOperationResult::message()  const
{
    return _message;
}

AnonOperationResult::ECode AnonOperationResult::code()  const
{
    return _code ;
}

//--------------------------------------------------------------

AnonOperationBatch::AnonOperationBatch(QObject *parent) :
    QObject(parent)
{
    _outProvider = this ;
    _isDocumentStandalone = false;
    isAborted = false ;
    _counterOperations = 0 ;
    _totalOperationCount = 0;
}

AnonOperationBatch::~AnonOperationBatch()
{
    _outProvider->outProviderAutoDelete();
}

const AnonOperationResult *AnonOperationBatch::result()
{
    return &_result ;
}

QIODevice *AnonOperationBatch::outProviderProvide(const QString &fileOutputPath)
{
    return new QFile(fileOutputPath);
}

void AnonOperationBatch::outProviderDeleteIO(QIODevice *outFile)
{
    delete outFile;
}

void AnonOperationBatch::outProviderAutoDelete()
{
    // do nothing
}

void AnonOperationBatch::setOutputProvider(AnonOperationBatchOutputFileProvider* newProvider)
{
    if(NULL != newProvider) {
        _outProvider = newProvider ;
    }
}

const AnonOperationResult * AnonOperationBatch::perform(const QString & fileInputPath, const QString & fileOutputPath, AnonContext * startContext)
{
    _totalOperationCount = 0 ;
    isAborted = false;
    _result.reset();
    if(NULL == startContext) {
        _result.setError(AnonOperationResult::RES_ERR_INVALID_CONTEXT, tr("Invalid start context"));
        return result();
    }
    QFile fileInput(fileInputPath);
    QIODevice *fileOutput = _outProvider->outProviderProvide(fileOutputPath);
    if(NULL == fileOutput) {
        _result.setError(AnonOperationResult::RES_ERR_OPEN_OUTPUT_FILE, tr("Unable to get output file:'%1'").arg(fileOutputPath));
    } else {
        if(!fileInput.open(QFile::ReadOnly)) {
            _result.setError(AnonOperationResult::RES_ERR_OPEN_INPUT_FILE, tr("Unable to open input file:'%1'").arg(fileInputPath));
        } else {
            if(!fileOutput->open(QIODevice::WriteOnly)) {
                _result.setError(AnonOperationResult::RES_ERR_OPEN_OUTPUT_FILE, tr("Unable to open output file:'%1'").arg(fileOutputPath));
            } else {
                scanAndExecute(&fileInput, fileOutput, fileInputPath, startContext);
                fileOutput->close();
            }
            fileInput.close();
        }
        _outProvider->outProviderDeleteIO(fileOutput);
    }
    return result() ;
}

const AnonOperationResult * AnonOperationBatch::scanAndExecute(QIODevice *input, QIODevice *output, const QString & fileInputPath, AnonContext *startContext)
{
    scan(input, startContext);
    if(!input->reset()) {
        _result.setError(AnonOperationResult::RES_ERR_READING_INPUTFILE, tr("Unable to reset input file:'%1'").arg(fileInputPath));
    } else {
        execute(input, output, startContext);
    }
    return result();
}

const AnonOperationResult *AnonOperationBatch::execute(QIODevice *input, QIODevice *output, AnonContext *startContext)
{
    isAborted = false ;
    _result.reset();
    bool debugIO = false ;
    int operationCount = _totalOperationCount ;

    //------ current position processing the file
    /*QString currentElementXPath;
    QString currentNsElementXPath;
    QStringList elementsStack;
    QStringList elementsNsStack;*/
    QList<AnonContext*> contexts;
    AnonContext* currentContext = startContext;
    //------
    /***************************************************************
    qint64 previousTokenLine = 0 ;
    qint64 previousTokenColumn = 0;
    qint64 previousTokenFileLinePos = 0;
    QXmlStreamReader::TokenType oldtk ;
    QString oldtks;
    ***************************************************************/

    QXmlStreamReader xmlReader;
    QXmlStreamWriter xmlWriter;

    xmlReader.clear();
    xmlReader.setDevice(input);
    xmlWriter.setDevice(output);
    //Let's do the same namespace processing of the editor.
    xmlReader.setNamespaceProcessing(false);
    while(!xmlReader.atEnd()) {

        /***********************************************
        QString msg = QString("Line %1 pos= %2").arg(line).arg(file->pos());
        printf( "%s\n",  msg.toAscii().data() );
        ************************************************/
        operationCount ++;
        xmlReader.readNext();
        /********************/
        if(debugIO) {
            QString msg = QString("Read Token: %1, [ %2, %3] line:%4 column:%5")
                          .arg(xmlReader.name().toString())
                          .arg(xmlReader.tokenType())
                          .arg(xmlReader.tokenString())
                          .arg(xmlReader.lineNumber())
                          .arg(xmlReader.columnNumber());
            printf("%s\n",  msg.toLatin1().data());
            fflush(stdout);
        }
        /*******************************/

        switch(xmlReader.tokenType()) {
        default:
            _result.setMessage(AnonOperationResult::RES_ERR_UNKNOWN_TOKEN,
                               QString("Unknown token '%1' at line: %2 col:%3")
                               .arg(xmlReader.tokenType()).arg(xmlReader.lineNumber()).arg(xmlReader.columnNumber()), false);
            break;// no-op
        case QXmlStreamReader::NoToken:
            break;
        //-- pass through ---------------------------------------------------
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            //default:
            xmlWriter.writeCurrentToken(xmlReader);
            break;
        //-------------------------------------------------------------------
        case QXmlStreamReader::Invalid:
            //if(!xmlReader.atEnd()) {
            handleError(&_result, &xmlReader);
            return result() ;
            //}
            break;
        case QXmlStreamReader::Characters: {

            if(NULL != currentContext) {
                currentContext->setExceptionForElement();
            }
            QString text = xmlReader.text().toString();
            QString anonText = anonymizeTextOfElement(currentContext, text);
            if(currentContext->isAlgStatError()) {
                handleError(&_result, currentContext->algStatContextRef(), &xmlReader);
                return result();
            }
            if(xmlReader.isCDATA()) {
                xmlWriter.writeCDATA(anonText);
            } else {
                xmlWriter.writeCharacters(anonText);
            }
            if(NULL != currentContext) {
                currentContext->restoreContext();
            }
        }
        break;
        case QXmlStreamReader::StartDocument: {
            _documentEncoding = xmlReader.documentEncoding().toString();
            _isDocumentStandalone = xmlReader.isStandaloneDocument();
            _documentVersion = xmlReader.documentVersion().toString();
            if(!_documentEncoding.isEmpty()) {
                xmlWriter.setCodec(_documentEncoding.toLatin1().data());
            }
            if(_isDocumentStandalone) {
                xmlWriter.writeStartDocument(_documentVersion, _isDocumentStandalone);
            } else {
                xmlWriter.writeStartDocument(_documentVersion);
            }
        }
        break;
        case QXmlStreamReader::EndDocument:
            xmlWriter.writeEndDocument();
            break;
        case QXmlStreamReader::StartElement: {
            // The reader reports the start of an element with namespaceUri() and name().
            //Attributes are reported in attributes(), namespace declarations in namespaceDeclarations().
            QString tag = xmlReader.qualifiedName().toString();
            AnonContext *thisContext = new AnonContext(currentContext, tag);
            contexts.append(thisContext);
            currentContext = thisContext;
            QXmlStreamAttributes streamAttributes = xmlReader.attributes();
            handleNamespace(tag, &streamAttributes, thisContext);
            thisContext->setExceptionForElement();

            xmlWriter.writeStartElement(tag);

            foreach(QXmlStreamAttribute oAttribute, streamAttributes) {
                Attribute attribute(oAttribute.qualifiedName().toString(), oAttribute.value().toString());
                attribute.anonymize(currentContext);
                if(currentContext->isAlgStatError()) {
                    handleError(&_result, currentContext->algStatContextRef(), &xmlReader);
                    return result();
                }
                xmlWriter.writeAttribute(attribute.name, attribute.value);
            }
            thisContext->restoreContext();
            /********************
            QString msg = QString("Start Element: %1, pos:%2 line:%3 column:%4 prev:%5").arg(xmlReader.name().toString())
                    .arg(xmlReader.characterOffset()).arg(xmlReader.lineNumber()).arg(xmlReader.columnNumber()).arg(previousPos);
            printf( "%s\n",  msg.toAscii().data() );
            *******************************/
        }
        break;
        case QXmlStreamReader::EndElement: {
            xmlWriter.writeEndElement();
            AnonContext *lastContext  = contexts.last();
            delete lastContext ;
            contexts.removeLast();
            if(!contexts.isEmpty()) {
                currentContext = contexts.last();
            } else {
                currentContext = startContext ;
            }
            break;
        }
        } // switch
        // check write conditions
        if(xmlReader.hasError() && (xmlReader.error() != QXmlStreamReader::PrematureEndOfDocumentError)) {
            if(!handleError(&_result, &xmlReader)) {
                return result() ;
            }
        }
        if(0x100 == (operationCount & 0x0100)) {
            bool isOk = false;
            _mutex.lock();
            _counterOperations = operationCount ;
            isOk = checkStatus(&_result);
            _mutex.unlock();
            if(!isOk) {
                return result() ;
            }
        }
    }// while at end
    return result();
}

const AnonOperationResult *AnonOperationBatch::scan(QIODevice *input, AnonContext *startContext)
{
    isAborted = false ;
    _result.reset();
    if(!startContext->alg()->needScan()) {
        return result();
    }

    bool debugIO = false ;
    int operationCount = 0;

    //------ current position processing the file
    QList<AnonContext*> contexts;
    AnonContext* currentContext = startContext;
    //------
    QXmlStreamReader xmlReader;

    xmlReader.clear();
    xmlReader.setDevice(input);
    //Let's do the same namespace processing of the editor.
    xmlReader.setNamespaceProcessing(false);
    while(!xmlReader.atEnd()) {

        /***********************************************
        QString msg = QString("Line %1 pos= %2").arg(line).arg(file->pos());
        printf( "%s\n",  msg.toAscii().data() );
        ************************************************/
        operationCount ++;
        xmlReader.readNext();
        /********************/
        if(debugIO) {
            QString msg = QString("Read Token: %1, [ %2, %3] line:%4 column:%5")
                          .arg(xmlReader.name().toString())
                          .arg(xmlReader.tokenType())
                          .arg(xmlReader.tokenString())
                          .arg(xmlReader.lineNumber())
                          .arg(xmlReader.columnNumber());
            printf("%s\n",  msg.toLatin1().data());
            fflush(stdout);
        }
        /*******************************/

        switch(xmlReader.tokenType()) {
        default:
            _result.setMessage(AnonOperationResult::RES_ERR_UNKNOWN_TOKEN,
                               QString("Unknown token '%1' at line: %2 col:%3")
                               .arg(xmlReader.tokenType()).arg(xmlReader.lineNumber()).arg(xmlReader.columnNumber()), false);
            break;// no-op
        case QXmlStreamReader::NoToken:
            break;
        //-- pass through ---------------------------------------------------
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            //default:
            break;
        //-------------------------------------------------------------------
        case QXmlStreamReader::Invalid:
            handleError(&_result, &xmlReader);
            return result() ;
            break;
        case QXmlStreamReader::Characters: {

            if(NULL != currentContext) {
                currentContext->setExceptionForElement();
            }
            const QString text = xmlReader.text().toString();
            scanTextOfElement(currentContext, text);
            if(currentContext->isAlgStatError()) {
                handleError(&_result, currentContext->algStatContextRef(), &xmlReader);
                return result();
            }
            if(NULL != currentContext) {
                currentContext->restoreContext();
            }
        }
        break;
        case QXmlStreamReader::StartDocument: {
            _documentEncoding = xmlReader.documentEncoding().toString();
            _isDocumentStandalone = xmlReader.isStandaloneDocument();
            _documentVersion = xmlReader.documentVersion().toString();
        }
        break;
        case QXmlStreamReader::EndDocument:
            break;
        case QXmlStreamReader::StartElement: {
            // The reader reports the start of an element with namespaceUri() and name().
            //Attributes are reported in attributes(), namespace declarations in namespaceDeclarations().
            QString tag = xmlReader.qualifiedName().toString();
            AnonContext *thisContext = new AnonContext(currentContext, tag);
            contexts.append(thisContext);
            currentContext = thisContext;
            QXmlStreamAttributes streamAttributes = xmlReader.attributes();
            handleNamespace(tag, &streamAttributes, thisContext);
            thisContext->setExceptionForElement();

            foreach(QXmlStreamAttribute oAttribute, streamAttributes) {
                Attribute attribute(oAttribute.qualifiedName().toString(), oAttribute.value().toString());
                attribute.scanAnonymize(currentContext);
                if(currentContext->isAlgStatError()) {
                    handleError(&_result, currentContext->algStatContextRef(), &xmlReader);
                    return result();
                }
            }
            thisContext->restoreContext();
            /********************
            QString msg = QString("Start Element: %1, pos:%2 line:%3 column:%4 prev:%5").arg(xmlReader.name().toString())
                    .arg(xmlReader.characterOffset()).arg(xmlReader.lineNumber()).arg(xmlReader.columnNumber()).arg(previousPos);
            printf( "%s\n",  msg.toAscii().data() );
            *******************************/
        }
        break;
        case QXmlStreamReader::EndElement: {
            AnonContext *lastContext  = contexts.last();
            delete lastContext ;
            contexts.removeLast();
            if(!contexts.isEmpty()) {
                currentContext = contexts.last();
            } else {
                currentContext = startContext ;
            }
            break;
        }
        } // switch
        // check write conditions
        if(xmlReader.hasError() && (xmlReader.error() != QXmlStreamReader::PrematureEndOfDocumentError)) {
            if(!handleError(&_result, &xmlReader)) {
                return result() ;
            }
        }
        if(0x100 == (operationCount & 0x0100)) {
            bool isOk = false;
            _mutex.lock();
            _counterOperations = operationCount ;
            isOk = checkStatus(&_result);
            _mutex.unlock();
            if(!isOk) {
                return result() ;
            }
        }
    }// while at end
    _totalOperationCount = operationCount;
    return result();
}

void AnonOperationBatch::setAborted()
{
    _mutex.lock();
    isAborted = true ;
    _mutex.unlock();
}

int AnonOperationBatch::operationsCount()
{
    int result = 0 ;
    _mutex.lock();
    result = _counterOperations ;
    _mutex.unlock();
    return result;
}

bool AnonOperationBatch::checkStatus(AnonOperationResult * result)
{
    bool checkAbort = false;

    if(isAborted) {
        checkAbort = isAborted ;
    }
    if(checkAbort) {
        result->setMessage(AnonOperationResult::RES_ERR_USERABORTED,
                           QString("User aborted."), true);
    }
    return !checkAbort ;
}

bool AnonOperationBatch::handleError(AnonOperationResult *result, QXmlStreamReader *xmlReader)
{
    result->setError(AnonOperationResult::RES_ERR_UNSPECIFIED, tr("Error code:%1 '%2' at line:%3 col:%4")
                     .arg(xmlReader->error()).arg(xmlReader->errorString()).arg(xmlReader->lineNumber()).arg(xmlReader->columnNumber()));
    return false;
}

bool AnonOperationBatch::handleError(AnonOperationResult *result, const AnonAlgStatContext &context, QXmlStreamReader *xmlReader)
{
    result->setError(AnonOperationResult::RES_ERR_ANONCONTEXT, tr("Error code:%1 at line:%2 col:%3\nDetails:\n%4")
                     .arg(context.errorMessage()).arg(xmlReader->errorString())
                     .arg(xmlReader->lineNumber()).arg(xmlReader->columnNumber())
                     .arg(context.errorMessage()));
    return false;
}

QString AnonOperationBatch::anonymizeTextOfElement(AnonContext *context, const QString &inputText)
{
    return XmlAnonUtils::anonymizeTextOfElement(context, inputText, NULL);
}

void AnonOperationBatch::scanTextOfElement(AnonContext *context, const QString &inputText)
{
    XmlAnonUtils::scanTextOfElement(context, inputText);
}

void AnonOperationBatch::handleNamespace(const QString & tag, QXmlStreamAttributes *streamAttributes, AnonContext *context)
{
    foreach(QXmlStreamAttribute oAttribute, *streamAttributes) {
        context->addNamespaceAttribute(oAttribute.qualifiedName().toString(), oAttribute.value().toString());
    }
    context->setContextElement(tag);
}
