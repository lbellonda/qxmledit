/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2016 by Luca Bellonda and individual contributors  *
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


#ifndef EXTRACTIONOPERATION_H
#define EXTRACTIONOPERATION_H

#include <QObject>
#include <QStringList>
#include <QMutex>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QTextStream>
#include "extractresults.h"
#include "libQXmlEdit_global.h"

/**************
  TODO:
  do a file report using date and name
  ****************/

class ExtractInfo
{
public:
    int currentDocument;
    QFile outputFile;
    uint currentSubfolderDocument;
    QXmlStreamWriter xmlWriter;
    QTextStream csvWriter;
    QString currentFolderPath;
    QHash<QString, int> _mapForCSVColumns;
    //--------
    QString csvRealFilePath;
    QString csvResourcesPath;
    QFile csvTempFile;
    QString crLf;
    //--------

    ExtractInfo();
    ~ExtractInfo();
};

class LIBQXMLEDITSHARED_EXPORT ExtractionOperation : public QObject
{
    Q_OBJECT

    static const qint64 InputFileBufferSize ;

public:
    enum EXMLErrors {
        EXML_NoError,
        EXML_UnexpectedElementError,
        EXML_CustomError,
        EXML_NotWellFormedError,
        EXML_PrematureEndOfDocumentError,
        EXML_NoFile,
        EXML_OpenFile,
        EXML_Other,
        EXML_SubFolderExists,
        EXML_SubFolderError,
        EXML_WriteError,
        EXML_OpenWriteError,
        EXML_BuildingCSVError
    };

    enum ECfrOp {
        CFR_EQ = 0,
        CFR_NEQ = 1
    };

    enum EXTRTYPE {
        ET_ALLDOCS,
        ET_RANGE,
        ET_USETERM
    };

    enum ESplitType {
        SplitUsingPath,
        SplitUsingDepth
    };

    enum EOperationType {
        OperationSplit,
        OperationFilter,
        OperationExportAndGroupXML,
        OperationExportAndGroupCSV,
    };

public:
    QMutex _mutex;
    QString currentSubFolder;
    volatile int counterDocumentsFound;
    volatile int counterFoldersCreated;
    volatile int counterOperations;
    volatile int percent;

private:
    volatile bool _running ;
    //---------------------
    bool _debug;
    QString _attributeName;
    QString _comparisonTerm;
    ECfrOp _comparisonType ;
    QString _inputFile;
    QString _splitPath;
    int _splitDepth;
    ESplitType _splitType;
    bool _isExtractDocuments;
    EOperationType _operationType ;
    bool _isReverseRange ;
    unsigned int _minDoc, _maxDoc;
    QString _extractFolder;
    bool _isMakeSubFolders;
    uint _subFoldersEachNFiles;
    QStringList _subfolderNamePattern;
    QStringList _filesNamePattern;
    QString _documentEncoding ;
    QString _documentVersion;
    bool _isDocumentStandalone ;
    bool _filterTextForPath;
    QStringList _pathsForFilterText;
    EXTRTYPE _extrType;
    qint64 _size;
    //-------------------
    bool _isError ;
    volatile bool _isAborted ;
    QString _errorMessage;
    ExtractionOperation::EXMLErrors _errorCode ;
    //----------------- results ---------------
    ExtractResults *_results;
    //-----------------------------------------

    void init();
    EXMLErrors decodeError(const QXmlStreamReader::Error error);
    void handleError(QXmlStreamReader &reader);
    void handleWriteError();
    bool handleExportedElement(ExtractInfo &info, QXmlStreamReader &reader);
    void setError(const EXMLErrors code, const QString message);
    bool isExportCSV() const;
    bool isXMLFilterExport() const;
    void execute(QFile *file);
    bool checkStatus();

    bool handleNewFile(ExtractInfo &info);
    void closeAllOperations(ExtractInfo &info);
    bool handleCloseOutputFile(ExtractInfo &info);
    bool openFile(ExtractInfo &info);

    QString makeFileName(const int currentFileIndex, const int docCounter);
    QString makeSubFolderName(const int currentDocumentIndex, const int docCounter);
    bool makeASubFolderWithError(ExtractInfo &info, const int currentFolderIndex, const int docCounter);
    bool writeAToken(const bool isAFilteredExtraction, const bool insideAFragment, ExtractInfo &info, QXmlStreamReader &reader);
    void addNamePattern(QStringList &list, const QString &str);

    QString getPathArrayString();
    QVector<QString> getPathArray();
    bool isFilterTextPathAbsolute();

    //--------
    bool writeCSVHeader(ExtractInfo &info);
    bool csvError(ExtractInfo &info, const EXMLErrors errorCode, const QString &message);
    bool manageOpenCSV(ExtractInfo &info);
    bool appendCSVData(ExtractInfo &info);
    bool closeCSVDataFile(ExtractInfo &info);
    bool handleCloseCSVOutputFile(ExtractInfo &info);
    bool removeCSVTempFile(ExtractInfo &info);
    bool isCSVBothFilesError(ExtractInfo &info);
    //--------

public:
    explicit ExtractionOperation(ExtractResults *results, QObject *parent = 0);
    ~ExtractionOperation();

    //----------------------------------------------
    void setInputFile(const QString &value);
    QString inputFile();
    void setSplitPath(const QString &value);
    QString splitPath();
    void setExtractAllDocuments();
    bool isExtractAllDocuments();
    void setExtractRange();
    bool isExtractRange();
    void setExtractCfr();
    bool isExtractCfr();
    void setMinDoc(const unsigned int value);
    unsigned int minDoc();
    void setMaxDoc(const unsigned int value);
    unsigned int maxDoc();
    void setExtractFolder(const QString &value);
    QString extractFolder();
    void setIsMakeSubFolders(const bool value);
    bool isMakeSubFolders();
    void setSubFoldersEachNFiles(const uint value);
    uint subFoldersEachNFiles();
    void setSubfolderNamePattern(const QStringList value);
    QStringList subfolderNamePattern();
    void setFilesNamePattern(const QStringList value);
    QStringList filesNamePattern();
    void setMinRange(const uint value);
    uint minRange();
    void setMaxRange(const uint value);
    uint maxRange();
    void setExtractDocuments(const bool value);
    bool isExtractDocuments();
    bool isError();
    EXMLErrors error();
    QString errorMessage();
    bool isAborted();
    void setAborted(const bool value);
    EOperationType OperationType();
    void setOperationType(const EOperationType value);
    bool isReverseRange();
    void setReverseRange(const bool value);
    bool isDebug();
    void setDebug(const bool value);
    ExtractResults *results();
    // TODO: to test
    QString attributeName();
    void setAttributeName(const QString &value);
    QString comparisonTerm();
    void setComparisonTerm(const QString &value);
    ECfrOp comparisonType();
    void setComparisonType(const ECfrOp value);
    EXTRTYPE extrType();
    void setExtrType(const EXTRTYPE value);
    void setSplitDepth(const int value);
    int splitDepth();
    void setSplitType(const ESplitType value);
    ESplitType splitType();
    bool isFilterTextForPath();
    void setFilterTextForPath(const bool value);
    void setPathForDeleteText(const QString &value);
    QString pathForDeleteText();

    QString makeAName(const QString &nameBase, const int currentIndex, const QStringList &tokensList, const int seqNumber);

    static void saveSettingsForExtractionFragmentNumber(const QString &filePath, const int fragment, const int depth);
    //----------------------------------------------

    void performExtraction();
    void loadSettings();
    void saveSettings();

    enum EParamErrors {
        ParamNoError = 0,
        ParamErrorNoInputFile,
        ParamErrorNoSplitPath,
        ParamErrorNoMinRange,
        ParamErrorNoMaxRange,
        ParamErrorNoRange,
        ParamErrorNoExtractFolder,
        ParamErrorSubFolderEach,
        ParamErrorSubFolderNamePattern,
        ParamErrorFilesNamePattern,
        ParamErrorCfrType,
        ParamErrorExtractionType,
        ParamErrorCfrAttr,
        ParamErrorSplitDepth, // invalid depth
        ParamErrorSplitType, // invalid type
        ParamErrorNoDeleteTextPath,
        ParamErrorBadOperationType
    };

    EParamErrors checkParameters();
    void addFolderPattern(const QString &str);
    void addFileNamePattern(const QString &str);


#define DATE_TOKEN_PTRN  "%date%"
#define TIME_TOKEN_PTRN  "%time%"
#define COUNTER_TOKEN_PTRN  "%counter%"
#define TS_TOKEN_PTRN  "%timestamp%"
#define SPACE_TOKEN_PTRN  "%space%"
#define SEQUENCE_TOKEN_PTRN  "%sequence%"

signals:

public slots:

};

#endif // EXTRACTIONOPERATION_H
