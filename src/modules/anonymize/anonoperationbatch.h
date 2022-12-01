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


#ifndef ANONOPERATIONBATCH_H
#define ANONOPERATIONBATCH_H

#include <QObject>
#include <QIODevice>
#include <QMutex>
#include "libQXmlEdit_global.h"

/**
 * @brief The AnonOperationBatch class anonymize a whole file
 */

class LIBQXMLEDITSHARED_EXPORT AnonOperationResult
{
public:
    enum ECode {
        RES_OK,
        RES_ERR_UNSPECIFIED,
        RES_ERR_UNKNOWN_TOKEN,
        RES_ERR_INVALID_CONTEXT,
        RES_ERR_OPEN_INPUT_FILE,
        RES_ERR_OPEN_OUTPUT_FILE,
        RES_ERR_USERABORTED,
        RES_ERR_READING_INPUTFILE,
        RES_ERR_ANONCONTEXT
    };

private:
    bool _ok;
    QString _message;
    ECode _code;

public:
    AnonOperationResult();
    ~AnonOperationResult();
    void reset();
    void setError(const ECode newCode, const QString &msg);
    void setMessage(const ECode newCode, const QString &msg, const bool isError);
    bool isError() const ;
    QString message() const ;
    ECode code() const ;
};

class QXmlStreamReader;
class AnonContext;
class QXmlStreamAttributes;
class AnonAlgStatContext;

class LIBQXMLEDITSHARED_EXPORT AnonOperationBatchOutputFileProvider
{
public:
    AnonOperationBatchOutputFileProvider();
    virtual ~AnonOperationBatchOutputFileProvider();

    virtual QIODevice *outProviderProvide(const QString &filePath) = 0;
    virtual void outProviderDeleteIO(QIODevice *) = 0;
    virtual void outProviderAutoDelete() = 0;
    void xping();
};

class LIBQXMLEDITSHARED_EXPORT AnonOperationBatch : public QObject, public AnonOperationBatchOutputFileProvider
{
    Q_OBJECT

    AnonOperationResult  _result;
    QString _documentEncoding ;
    bool _isDocumentStandalone ;
    QString _documentVersion;
    volatile bool isAborted ;
    QMutex _mutex;
    volatile int _counterOperations;
    AnonOperationBatchOutputFileProvider *_outProvider;
    int _totalOperationCount;
public:
    explicit AnonOperationBatch(QObject *parent = 0);
    virtual ~AnonOperationBatch();

    const AnonOperationResult *scan(QIODevice *input, AnonContext *startContext);
    const AnonOperationResult *execute(QIODevice *input, QIODevice *output, AnonContext *startContext);
    const AnonOperationResult *scanAndExecute(QIODevice *input, QIODevice *output, const QString & fileInputPath, AnonContext *startContext);
    const AnonOperationResult *perform(const QString &fileInputPath, const QString &fileOutputPath, AnonContext *startContext);
    const AnonOperationResult *result();
    void setAborted();
    int operationsCount();
    int getIndent() const;
    void setIndent(int value);
    void setOutputProvider(AnonOperationBatchOutputFileProvider* newProvider);
    virtual QIODevice *outProviderProvide(const QString &filePath);
    virtual void outProviderDeleteIO(QIODevice *);
    virtual void outProviderAutoDelete();
private:
    bool checkStatus(AnonOperationResult *result);
    bool handleError(AnonOperationResult *result, QXmlStreamReader *xmlReader);
    bool handleError(AnonOperationResult *result, const AnonAlgStatContext &context, QXmlStreamReader *xmlReader);
    QString anonymizeTextOfElement(AnonContext *context, const QString &inputText);
    void scanTextOfElement(AnonContext *context, const QString &inputText);
    void handleNamespace(const QString &tag, QXmlStreamAttributes *streamAttributes, AnonContext *context);

signals:

public slots:

};

#endif // ANONOPERATIONBATCH_H
