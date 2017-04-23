/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#ifndef XSLTEXECUTOR_H
#define XSLTEXECUTOR_H

#include "xmlEdit.h"
#include "applicationdata.h"
#include "operationresult.h"
#include <QAbstractMessageHandler>
#include <QBuffer>
#include <QTemporaryFile>

class XmlEditWidget;
class QXmlQuery;

class XSLTExecutor
{
protected:
    class MessageHandler : public QAbstractMessageHandler
    {
        QMutex _mutex;
    public:
        MessagesOperationResult &_result;
        MessageHandler(MessagesOperationResult &result);
        ~MessageHandler();
        virtual void handleMessage(QtMsgType type,
                                   const QString &description,
                                   const QUrl &identifier,
                                   const QSourceLocation &sourceLocation);
    };

    class InputHolder
    {
    public:
        InputHolder();
        virtual ~InputHolder();

        virtual QIODevice *device() = 0;
        virtual bool isFile() ;
        virtual QString fileName() ;
        virtual bool createTempFile();
        virtual bool removeTempFile();
    };

    class InputFileHolder : public InputHolder
    {
        QString _fileName;
        QFile _device;
    public:
        InputFileHolder(const QString &fileName);
        ~InputFileHolder();

        virtual QIODevice *device();
        virtual bool isFile() ;
        virtual QString fileName() ;
    };

    class InputStringHolder : public InputHolder
    {
        QString _target;
        QByteArray _data ;
        QBuffer _device;
        QTemporaryFile _tempFile;
    public:
        InputStringHolder(const QString &outString);
        ~InputStringHolder();

        virtual QIODevice *device();
        virtual bool createTempFile();
        virtual bool removeTempFile();
        virtual QString fileName() ;
    };

    class OutputHolder
    {
    public:
        OutputHolder();
        virtual ~OutputHolder();

        virtual bool openForWrite() = 0;
        virtual bool close() = 0;
        virtual bool evaluateTo(QXmlQuery &query) = 0;
        virtual bool isFile() ;
        virtual QString fileName() ;
        virtual bool createTempFilePath();
        virtual bool removeTempFile();
        virtual bool readResult();
    };

    class OutputFileHolder : public OutputHolder
    {
        QString _fileName;
        QFile _device;
    public:
        OutputFileHolder(const QString &fileName);
        ~OutputFileHolder();

        virtual bool openForWrite();
        virtual bool close() ;
        virtual bool evaluateTo(QXmlQuery &query);
        virtual bool isFile() ;
        virtual QString fileName() ;
    };

    class OutputStringHolder : public OutputHolder
    {
        QString *_target;
        QByteArray _data ;
        QBuffer _device;
        QTemporaryFile _tempFile;
        bool loadFromFile(QFile *ioDevice, QString *ptr);
    public:
        OutputStringHolder(QString *outString);
        ~OutputStringHolder();

        virtual bool openForWrite() ;
        virtual bool close() ;
        virtual bool evaluateTo(QXmlQuery &query);
        virtual bool createTempFilePath();
        virtual bool removeTempFile();
        virtual bool readResult();
        virtual QString fileName() ;
    };

    InputHolder *_sourceHolder;
    OutputHolder *_outputHolder;
    InputHolder *_xsltHolder;
    QHash<QString, QString> _parameters;
    ApplicationData *_data;

    static const int TimeoutSaxon = -1 ;

    void setInputHolder(InputHolder *newHolder, InputHolder **holder);
    void setOutputHolder(OutputHolder *newHolder, OutputHolder **holder);
    bool innerExecQt(MessagesOperationResult &result, QIODevice *inputDevice, QIODevice *xsltSheetDevice, OutputHolder *outputDevice,
                     QHash<QString, QString> parameters);
    bool innerExecSaxon(MessagesOperationResult &result,
                        const QString &inputFile, const QString &xsltSheet, const QString &outputFile,
                        QHash<QString, QString> parameters);
    void addError(MessagesOperationResult &result, const int messageCode, const QString &msgText, const qint64 line, const qint64 column);
    void addInfo(MessagesOperationResult &result, const int messageCode, const QString &msgText, const qint64 line, const qint64 column);
    bool closeDevice(QIODevice *device);
    bool closeDevice(XSLTExecutor::OutputHolder *device, MessagesOperationResult &result, const int errorCode, const QString &deviceType);
    bool execSaxon(MessagesOperationResult &result);
    bool execQt(MessagesOperationResult &result);
    QStringList saxonArguments(const QString &saxonPath, const QString &inputFile, const QString &xsltSheet,
                               const QString &outputFile,
                               QHash<QString, QString> parameters);

public:
    enum ErrorCodes {
        ErrorSourceMissing,
        ErrorXSLMissing,
        ErrorOutputMissing,
        ErrorXSLTInvalid,
        ErrorOpeningDeviceInput,
        ErrorOpeningDeviceXSL,
        ErrorOpeningDeviceOutput,
        ErrorClosingDeviceOutput,//totest, touse
        ErrorRuntime,//totest
        ErrorEngineNotConfigured,
        ErrorConfigurationMissing,
    };

    XSLTExecutor(ApplicationData *data);
    virtual ~XSLTExecutor();

    void setParameters(QHash<QString, QString> parameters);
    void setInputLiteral(const QString &data);
    void setInputFile(const QString &fileName);
    void setOutput(const QString &fileName);
    void setOutput(QString *outputStringPtr);
    void setXSLLiteral(const QString &xsl);
    void setXSLFile(const QString &fileName);
    virtual bool exec(MessagesOperationResult &result) ;

    static bool execBatch(ApplicationData *appData, MessagesOperationResult &result,
                          const QString &inputFile, const QString &xslFile,
                          const QString &outputFile,
                          QList<QPair<QString, QString> > params,
                          const bool forceSaxon);
#ifdef QXMLEDIT_TEST
    static QStringList testSaxonArguments;
    static bool stop;
    static bool saxonWasChosen;
    static void resetTest();
#endif
};

#endif // XSLTEXECUTOR_H
