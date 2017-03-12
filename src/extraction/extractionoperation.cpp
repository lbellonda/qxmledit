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

#include "xmlEdit.h"
#include "extractionoperation.h"
#include "utils.h"
#include <QXmlStreamReader>
#include <QDir>
#include <QDateTime>
#include <QTextCodec>
#include "qxmleditconfig.h"

const qint64 ExtractionOperation::InputFileBufferSize = 512000;

ExtractionOperation::ExtractionOperation(ExtractResults *results, QObject *parent) :
    QObject(parent)
{
    _results = results;
    init();
}

ExtractionOperation::~ExtractionOperation()
{
}

void ExtractionOperation::init()
{
    _running = false;
    _debug = false;

    _extrType = ET_ALLDOCS ;
    _minDoc = 0 ;
    _maxDoc = 0 ;
    _isMakeSubFolders = false ;
    _subFoldersEachNFiles = 100 ;
    _subfolderNamePattern.clear();
    _filesNamePattern.clear();
    _isExtractDocuments = true ;
    _operationType = OperationSplit ;
    _isReverseRange = false;
    _comparisonType = CFR_EQ ;
    _isDocumentStandalone = false ;
    counterDocumentsFound = 0;
    counterFoldersCreated = 0;
    counterOperations = 0 ;
    _splitDepth = 1;
    _splitType = SplitUsingPath;
    _filterTextForPath = false;

    //---------------------
    _isError = false ;
    _isAborted = false;
    _errorCode = EXML_NoError;
}

void ExtractionOperation::setError(const EXMLErrors code, const QString message)
{
    _isError = true ;
    _errorCode = code ;
    _errorMessage = message ;
}

void ExtractionOperation::performExtraction()
{
    _running = true;
    QFile inputFile(_inputFile);
    if(!QFile::exists(_inputFile)) {
        setError(EXML_NoFile, tr("File \"%1\" is not accessible").arg(_inputFile));
    } else {
        if(! inputFile.open(QIODevice::ReadOnly)) {
            setError(EXML_OpenFile, tr("Unable to open file \"%1\" ").arg(_inputFile));
        } else {
            _results->_fileName = _inputFile ;
            execute(&inputFile);
            inputFile.close();
        }
    }
    _results->setError(isError());
    _running = false;
}

QString ExtractionOperation::getPathArrayString()
{
    if(_pathsForFilterText.size() > 0) {
        QString first = _pathsForFilterText.at(0);
        return first ;
    }
    return "" ;
}

QVector<QString> ExtractionOperation::getPathArray()
{
    QVector<QString> result;
    QString first = getPathArrayString();
    QStringList lst = first.split("/");
    foreach(QString s, lst) {
        result.append(s);
    }
    return result ;
}

bool ExtractionOperation::isFilterTextPathAbsolute()
{
    if(getPathArrayString().startsWith("/")) {
        return true;
    }
    return false;
}

bool ExtractionOperation::isExportCSV() const
{
    return OperationExportAndGroupCSV == _operationType ;
}

bool ExtractionOperation::isXMLFilterExport() const
{
    return OperationExportAndGroupXML == _operationType ;
}

void ExtractionOperation::execute(QFile *file)
{
    bool debugIO = _debug ;
    int operationCount = 0;
    //bool inDocument = false; TODO
    bool insideAFragment = false;
    bool isWriting = false;
    QXmlStreamReader xmlReader;
    QString path = "";
    ExtractInfo info;
    int level = 0;
    bool isDepth = (SplitUsingDepth  == _splitType) ? true : false ;
    //------
    bool isFilterText = _filterTextForPath;
    QString absolutePathForFilter = getPathArrayString();
    QString relativePathForFilter = QString("/%1").arg(getPathArrayString());
    bool isTheFilterTextPathAbsolute = isFilterTextPathAbsolute();
    bool isCalcPathForFilterTextForElement = false;
    bool isCurrentElementFilterText = false;
    //------

    /***************************************************************
    qint64 previousTokenLine = 0 ;
    qint64 previousTokenColumn = 0;
    qint64 previousTokenFileLinePos = 0;
    QXmlStreamReader::TokenType oldtk ;
    QString oldtks;
    ***************************************************************/

    file->seek(0);
    info._mapForCSVColumns.clear();
    if(!_isMakeSubFolders) {
        info.currentFolderPath = _extractFolder ;
    }
    xmlReader.clear();
    xmlReader.setDevice(file);
    Utils::TODO_THIS_RELEASE("questa var risulta non usata check nei commit isInterDocument");
    Utils::TODO_THIS_RELEASE("//bool isInterDocument = false;");
    bool isAFilteredExtraction =  _isExtractDocuments && !isExtractAllDocuments() && (OperationFilter == _operationType);
    bool isAnExportExtraction =  _isExtractDocuments && ((OperationExportAndGroupCSV == _operationType) || (OperationExportAndGroupXML == _operationType));
    qint64 previousPos = 0;
    while(!xmlReader.atEnd()) {
        operationCount ++;
        xmlReader.readNext();
        bool dontWrite = false;
        bool writeThis = false;
        bool isStillInFragment = false;
        bool isStartDocument = false;

        /********************/
        if(debugIO) {
            QString msg = QString("Read Token: %1, [ %2, %3] line:%4 column:%5, offset:%6")
                          .arg(xmlReader.name().toString())
                          .arg(xmlReader.tokenType())
                          .arg(xmlReader.tokenString())
                          .arg(xmlReader.lineNumber())
                          .arg(xmlReader.columnNumber())
                          .arg(xmlReader.characterOffset());
            printf("%s\n",  msg.toLatin1().data());
            fflush(stdout);
        }
        /*******************************/


        switch(xmlReader.tokenType()) {
        default:
            break;// no-op
        case QXmlStreamReader::Invalid:
            dontWrite = true;
            if(!xmlReader.atEnd()) {
                handleError(xmlReader);
                return ;
            }
            break;
        case QXmlStreamReader::Characters:
            if(isFilterText) {
                if(!isCalcPathForFilterTextForElement) {
                    isCalcPathForFilterTextForElement = true;
                    if(isTheFilterTextPathAbsolute) {
                        isCurrentElementFilterText = (path == absolutePathForFilter);
                    } else {
                        // relative path
                        if(path.endsWith(relativePathForFilter)) {
                            isCurrentElementFilterText = true ;
                        }
                    }
                }
                if(isCurrentElementFilterText) {
                    dontWrite = true ;
                }
            }
            break;
        case QXmlStreamReader::StartDocument:
            //inDocument = true ;
            isStartDocument = true ;
            _documentEncoding = xmlReader.documentEncoding().toString();
            _results->_encoding = _documentEncoding ;
            _isDocumentStandalone = xmlReader.isStandaloneDocument();
            _documentVersion = xmlReader.documentVersion().toString();
            Utils::TODO_THIS_RELEASE("//isInterDocument = true;");
            // if it is a filter, open the file or fail
            if(isAFilteredExtraction || isAnExportExtraction) {
                dontWrite = true ;
                if(!handleNewFile(info)) {
                    handleCloseOutputFile(info);
                    return ;
                }
            }
            break;
        case QXmlStreamReader::EndDocument:
            //inDocument = false ;
            break;
        case QXmlStreamReader::StartElement: {  // The reader reports the start of an element with namespaceUri() and name(). Empty elements are also reported as StartElement, followed directly by EndElement. The convenience function readElementText() can be called to concatenate all content until the corresponding EndElement. Attributes are reported in attributes(), namespace declarations in namespaceDeclarations().
            bool isError = false;
            level++;
            isCalcPathForFilterTextForElement = false;
            isCurrentElementFilterText = false;
            QString actualPath = path + "/" + xmlReader.name().toString();
            if(!insideAFragment) {
                if((isDepth && (level == _splitDepth)) || (!isDepth && (actualPath == _splitPath))) {
                    if(debugIO) {
                        printf("***Start fragment\n");
                        fflush(stdout);
                    }
                    Utils::TODO_THIS_RELEASE("//isInterDocument = false;");
                    _results->incrementFragment(previousPos - 1);
                    bool registerDocument = false ;
                    if(_isExtractDocuments) {
                        if(!isExtractAllDocuments()) {
                            if(isExtractCfr()) {
                                QXmlStreamAttributes streamAttributes = xmlReader.attributes();
                                QStringRef valueRef = streamAttributes.value(_attributeName);
                                QString attributeValue = valueRef.toString();
                                if(CFR_EQ == _comparisonType) {
                                    if(attributeValue == _comparisonTerm) {
                                        registerDocument = true ;
                                    }
                                } else {
                                    if(attributeValue != _comparisonTerm) {
                                        registerDocument = true ;
                                    }
                                }
                            } else {
                                uint inputDocumentCount = _results->currentFragment();
                                if((_minDoc <= inputDocumentCount) && (_maxDoc >= inputDocumentCount)) {
                                    registerDocument = true ;
                                }
                                if(_isReverseRange) {
                                    registerDocument = !registerDocument ;
                                }
                            }
                        } else {
                            registerDocument = true ;
                        }
                    }
                    insideAFragment = true ;
                    if(registerDocument) {
                        if(isAnExportExtraction) {
                            if(!handleExportedElement(info, xmlReader)) {
                                isError = true ;
                            }
                        } else {
                            isWriting = true ;
                            if(!isAFilteredExtraction) {
                                if(!handleNewFile(info)) {
                                    isError = true ;
                                }
                            } // check new file iff it is not a filtered op.
                        }
                    }
                }
            }
            if(isError) {
                handleCloseOutputFile(info);
                return ;
            }
            path = actualPath ;
            /********************
            QString msg = QString("Start Element: %1, pos:%2 line:%3 column:%4 prev:%5").arg(xmlReader.name().toString())
                    .arg(xmlReader.characterOffset()).arg(xmlReader.lineNumber()).arg(xmlReader.columnNumber()).arg(previousPos);
            printf( "%s\n",  msg.toAscii().data() );
            *******************************/
        }
        break;
        case QXmlStreamReader::EndElement: {
            isCalcPathForFilterTextForElement = false;
            isCurrentElementFilterText = false;
            if(insideAFragment) {
                isStillInFragment = true ;
                if((isDepth && (level == _splitDepth)) || (!isDepth && (path == _splitPath))) {
                    Utils::TODO_THIS_RELEASE("//isInterDocument = true;");
                    _results->endFragment(xmlReader.characterOffset());
                    if(debugIO) {
                        printf("***Closing fragment\n");
                        fflush(stdout);
                    }
                    if(_isExtractDocuments) {
                        // iif this is not a filter operation.
                        if(isWriting) {
                            if(isAFilteredExtraction) {
                                writeThis = true ;
                            } else {
                                if(!writeAToken(false, true, info, xmlReader)) {
                                    return ;
                                }
                                if(!handleCloseOutputFile(info)) {
                                    return ;
                                }
                            }
                            isWriting = false;
                        } // if is writing
                    } // if extract document
                    insideAFragment = false;
                } // if path
            }
            int index = path.lastIndexOf('/');
            if(-1 == index) {
                setError(EXML_Other, tr("Bad XML state at offset:%1").arg(xmlReader.characterOffset()));
                return ;
            }
            path = path.mid(0, index) ;

            /*QString msg = QString("End Element %1, pos %2 ").arg(xmlReader.name().toString()).arg(xmlReader.characterOffset());
            printf( "%s",  msg.toAscii().data() );*/

            level--;
            break;
        }
        } // switch

        if(isStartDocument) {
            previousPos = 1 ;
        } else {
            previousPos = xmlReader.characterOffset();
        }
        /*****************************************************
        previousTokenLine = xmlReader.lineNumber();
        previousTokenColumn = xmlReader.columnNumber();
        previousTokenFileLinePos = linePos ;
        oldtk = xmlReader.tokenType(); //TODO:delete
        oldtks = xmlReader.text().toString() ; //TODO:delete
        ******************************************************/
        // check write conditions
        if(isAFilteredExtraction) {
            if((!insideAFragment && !isStillInFragment) || (insideAFragment && isWriting) || writeThis) {
                if(!dontWrite) {

                    if(debugIO) {
                        QString msg = QString(">>>Write Token: %1, [ %2, %3]")
                                      .arg(xmlReader.name().toString())
                                      .arg(xmlReader.tokenType())
                                      .arg(xmlReader.tokenString());
                        printf("%s\n",  msg.toLatin1().data());
                        fflush(stdout);
                    }

                    if(!writeAToken(true, insideAFragment, info, xmlReader)) {
                        return ;
                    }
                }
            }
        } else {
            if(isWriting && !dontWrite) {
                if(!writeAToken(false, insideAFragment, info, xmlReader)) {
                    return ;
                }
            }
        } // check for write
        if(xmlReader.hasError() && (xmlReader.error() != QXmlStreamReader::PrematureEndOfDocumentError)) {
            handleError(xmlReader);
            return ;
        }
        if(0x100 == (operationCount & 0x0100)) {
            _mutex.lock();
            currentSubFolder = info.currentFolderPath ;
            counterDocumentsFound = _results->currentFragment();
            counterFoldersCreated = _results->currentFolderCount();
            counterOperations = operationCount ;
            _mutex.unlock();
            if(!checkStatus()) {
                return ;
            }
        }
    }// while at end
    handleCloseOutputFile(info);
}

bool ExtractionOperation::writeAToken(const bool isAFilteredExtraction, const bool insideAFragment, ExtractInfo &info, QXmlStreamReader &reader)
{
    if((insideAFragment && _isExtractDocuments) || isAFilteredExtraction) {
        info.xmlWriter.writeCurrentToken(reader);
        if(info.outputFile.error() != QFile::NoError) {
            handleWriteError();
            return false ;
        }
    }
    return true ;
}


QString ExtractionOperation::makeAName(const QString &nameBase, const int currentIndex, const QStringList &tokensList, const int seqNumber)
{
    QString name = nameBase;
    if(!name.isEmpty()) {
        name.append(QDir::separator());
    }
    foreach(QString str, tokensList) {
        if(str == SEQUENCE_TOKEN_PTRN) {
            name.append(QString::number(seqNumber));
        } else if(str == COUNTER_TOKEN_PTRN) {
            name.append(QString::number(currentIndex));
        } else if(str == DATE_TOKEN_PTRN) {
            QDateTime dt = QDateTime::currentDateTime();
            name.append(dt.toString("yyyy'_'MM'_'dd"));
        } else if(str == TIME_TOKEN_PTRN) {
            QDateTime dt = QDateTime::currentDateTime();
            name.append(dt.toString("hh'_'mm'_'ss'_'zzz"));
        } else if(str == TS_TOKEN_PTRN) {
            QDateTime dt = QDateTime::currentDateTime();
            name.append(QString::number(dt.toMSecsSinceEpoch()));
        } else if(str == SPACE_TOKEN_PTRN) {
            name.append(' ');
        } else {
            name.append(str);
        }
    }
    return name ;
}

QString ExtractionOperation::makeSubFolderName(const int currentFolderIndex, const int docCounter)
{
    return makeAName(_extractFolder, currentFolderIndex, _subfolderNamePattern, docCounter);
}

QString ExtractionOperation::makeFileName(const int currentFileIndex, const int docCounter)
{
    return makeAName("", currentFileIndex, _filesNamePattern, docCounter);
}

bool ExtractionOperation::makeASubFolderWithError(ExtractInfo &info, const int currentFolderIndex, const int docCounter)
{
    QString folderName;
    folderName = makeSubFolderName(currentFolderIndex, docCounter);
    QDir subFolder(folderName);
    if(subFolder.exists()) {
        setError(EXML_SubFolderExists, tr("The folder '%1' already exists").arg(folderName));
        return false;
    }
    if(!subFolder.mkpath(folderName)) {
        setError(EXML_SubFolderError, tr("Error creating folder '%1'").arg(folderName));
        return false ;
    }
    info.currentFolderPath = folderName;
    return true ;
}

/**
  makes subfolders, if needed
  */

bool ExtractionOperation::handleNewFile(ExtractInfo &info)
{
    if(_isMakeSubFolders) {
        if((0 == info.currentSubfolderDocument)
                || ((info.currentSubfolderDocument + 1) > _subFoldersEachNFiles)) {
            _results->_numFoldersCreated ++ ;
            if(!makeASubFolderWithError(info, _results->_numFoldersCreated, _results->numFragments())) {
                return false;
            }
            info.currentSubfolderDocument = 0 ;
        }
    }
    if(!handleCloseOutputFile(info)) {
        return false;
    }
    if(!openFile(info)) {
        return false;
    }
    return true ;
}

bool ExtractionOperation::manageOpenCSV(ExtractInfo &info)
{
    info.csvRealFilePath = info.outputFile.fileName();
    info.csvResourcesPath = info.outputFile.fileName() + ".temp";
    info.csvTempFile.setFileName(info.csvResourcesPath);
    if(!info.csvTempFile.open(QIODevice::WriteOnly)) {
        if(info.outputFile.isOpen()) {
            info.outputFile.close();
        }
        return csvError(info, EXML_OpenWriteError, tr("Unable to open for the CSV file '%1'").arg(info.csvResourcesPath));
    }
    info.csvWriter.setDevice(&info.csvTempFile);
    info.csvWriter.setCodec(QTextCodec::codecForName(_documentEncoding.toLatin1().data()));
    // section 4.1.1. of RFC 2046 states explicitly that
    info.crLf = "\r\n" ;
    return true ;
}

bool ExtractionOperation::openFile(ExtractInfo &info)
{
    if(info.outputFile.isOpen()) {
        info.outputFile.close();
        setError(EXML_OpenWriteError, tr("Invalid internal state while opening a file"));
        return false;
    }
    info.currentSubfolderDocument++;
    info.currentDocument++;
    _results->_numDocumentsCreated ++;
    QString fileName = makeFileName(info.currentDocument, _results->numFragments());
    QString filePath = info.currentFolderPath;
    filePath.append(QDir::separator());
    filePath.append(fileName);
    filePath.append(isExportCSV() ? ".csv" : ".xml");
    info.outputFile.setFileName(filePath);
    if(!info.outputFile.open(QIODevice::WriteOnly)) {
        setError(EXML_OpenWriteError, tr("Unable to open for writing the file '%1'").arg(filePath));
        return false;
    }
    if(isExportCSV()) {
        if(!manageOpenCSV(info)) {
            return false ;
        }
    } else {
        info.xmlWriter.setCodec(QTextCodec::codecForName(_documentEncoding.toLatin1().data()));
        info.xmlWriter.setDevice(&info.outputFile);
        info.xmlWriter.setAutoFormatting(true);
        if(_isDocumentStandalone) {
            info.xmlWriter.writeStartDocument(_documentVersion, _isDocumentStandalone);
        } else {
            if(!_documentVersion.isEmpty()) {
                info.xmlWriter.writeStartDocument(_documentVersion);
            } else {
                info.xmlWriter.writeStartDocument();
            }
        }
    }
    // se filtro XML, inserisci radice
    if(isXMLFilterExport()) {
        info.xmlWriter.writeStartElement("root");
    }
    if(info.outputFile.error() != QFile::NoError) {
        setError(EXML_OpenWriteError, tr("Unable to open document for file '%1'").arg(filePath));
        return false;
    }
    return true;
}

bool ExtractionOperation::csvError(ExtractInfo &info, const EXMLErrors errorCode, const QString &message)
{
    if(info.csvTempFile.isOpen()) {
        info.csvTempFile.close();
    }
    if(info.outputFile.isOpen()) {
        info.csvTempFile.close();
    }
    setError(errorCode, message);
    return false;
}

bool ExtractionOperation::writeCSVHeader(ExtractInfo &info)
{
    QTextStream headerStream;
    headerStream.setDevice(&info.outputFile);
    headerStream.setCodec(QTextCodec::codecForName(_documentEncoding.toLatin1().data()));

    QHash<int, QString> columnsForName;
    // insert columns
    const int  columnsCount = info._mapForCSVColumns.size();
    foreach(const QString & name, info._mapForCSVColumns.keys()) {
        const int colPos = info._mapForCSVColumns[name];
        columnsForName.insert(colPos, name);
    }
    FORINT(index, columnsCount) {
        const QString &name = columnsForName[index];
        headerStream << Utils::valueStringCSV(name, (0 == index));
    }
    headerStream << info.crLf ;
    headerStream.flush();
    if(QTextStream::Ok != headerStream.status()) {
        return false;
    }
    info.outputFile.close();
    if(info.outputFile.error() != QFile::NoError) {
        return false;
    }
    return true ;
}

bool ExtractionOperation::closeCSVDataFile(ExtractInfo &info)
{
    info.csvWriter.flush();
    if(QTextStream::Ok != info.csvWriter.status()) {
        return false;
    }
    info.csvTempFile.close();
    if(info.csvTempFile.error() != QFile::NoError) {
        return false;
    }
    return true ;
}

bool ExtractionOperation::isCSVBothFilesError(ExtractInfo &info)
{
    return (info.outputFile.error() != QFile::NoError) || (info.csvTempFile.error() != QFile::NoError);
}

bool ExtractionOperation::appendCSVData(ExtractInfo &info)
{
    bool status = false;
    if(info.outputFile.isOpen() || info.csvTempFile.isOpen()) {
        setError(EXML_BuildingCSVError, tr("Error unexpected state opening file CSV final: '%1'").arg(info.outputFile.fileName()));
        return false;
    }

    if(info.outputFile.open(QFile::Append | QFile::WriteOnly)) {
        if(info.csvTempFile.open(QFile::ReadOnly)) {
            // copy data
            const int BufferSize = 8000;
            char buffer[BufferSize + 50];
            qint64 bytes = 0;
            bool isError = false;
            do {
                bytes = info.csvTempFile.read(buffer, BufferSize);
                if(bytes > 0) {
                    if(info.outputFile.write(buffer, bytes) != bytes) {
                        isError = true ;
                    }
                }
                if((-1 == bytes) || isCSVBothFilesError(info)) {
                    setError(EXML_BuildingCSVError, tr("Error writing CSV final: '%1'").arg(info.outputFile.fileName()));
                    isError = true ;
                }
            } while(!isError && (bytes > 0));
            info.outputFile.flush();
            // close files
            info.csvTempFile.close();
            // check errors
            // set error message
            if(!isError && !isCSVBothFilesError(info)) {
                status = true;
            }
        } else {
            setError(EXML_BuildingCSVError, tr("Error opening CSV 2 for rewind: '%1'").arg(info.outputFile.fileName()));
            info.outputFile.close();
        }

    } else {

        setError(EXML_BuildingCSVError, tr("Error opening CSV 1 for rewind: '%1'").arg(info.outputFile.fileName()));
    }
    // check errors in files
    if(status) {
        if(isCSVBothFilesError(info)) {
            setError(EXML_BuildingCSVError, tr("Error writing CSV after final check: '%1'").arg(info.outputFile.fileName()));
            status = false;
        }
    }
    return status;
}


bool ExtractionOperation::handleCloseCSVOutputFile(ExtractInfo &info)
{
    info.csvWriter.flush();
    // close data file
    if(!closeCSVDataFile(info)) {
        return csvError(info, EXML_BuildingCSVError, tr("Error closing CSV data phase 1"));
    }
    // write header in the real file
    if(!writeCSVHeader(info)) {
        return csvError(info, EXML_BuildingCSVError, tr("Error writing CSV header"));
    }
    // now both the files are closed
    // close file and reopen it
    if(!appendCSVData(info)) {
        return csvError(info, EXML_BuildingCSVError, tr("Error appending CSV data"));
    }
    // remove temporary file
    if(!removeCSVTempFile(info)) {
        // not a real error
        _errorMessage = tr("Error removing temporary CSV file");
    }
    return true ;
}

bool ExtractionOperation::removeCSVTempFile(ExtractInfo & info)
{
    return info.csvTempFile.remove();
}

bool ExtractionOperation::handleCloseOutputFile(ExtractInfo &info)
{
    if(info.outputFile.isOpen()) {
        if(isExportCSV()) {
            return handleCloseCSVOutputFile(info);
        } else {
            if(isXMLFilterExport()) {
                info.xmlWriter.writeEndElement();
            }
            info.xmlWriter.writeEndDocument();
        }
        info.outputFile.close();
        if(info.outputFile.error() != QFile::NoError) {
            setError(EXML_WriteError, tr("Error while closing output file"));
            return false;
        }
    }
    return true;
}

void ExtractionOperation::handleWriteError()
{
    setError(EXML_WriteError, tr("Error writing output file"));
}

bool ExtractionOperation::checkStatus()
{
    bool checkAbort = false;

    // /_mutex.lock(); ?
    if(_isAborted) {
        checkAbort = _isAborted ;
    }
    // //_mutex.unlock(); ?
    if(checkAbort) {
        _isError = true ; // to mark the results
    }
    return !checkAbort ;
}

void ExtractionOperation::handleError(QXmlStreamReader &reader)
{
    setError(decodeError(reader.error()), reader.errorString());
}

ExtractionOperation::EXMLErrors ExtractionOperation::decodeError(const QXmlStreamReader::Error error)
{
    switch(error) {
    default: return EXML_Other;
    case QXmlStreamReader::NoError: return EXML_NoError;
    case QXmlStreamReader::UnexpectedElementError: return EXML_UnexpectedElementError;
    case QXmlStreamReader::CustomError: return EXML_CustomError;
    case QXmlStreamReader::NotWellFormedError: return EXML_NotWellFormedError;
    case QXmlStreamReader::PrematureEndOfDocumentError: return EXML_PrematureEndOfDocumentError;

    }
    return EXML_Other ;
}

void ExtractionOperation::loadSettings()
{
    _inputFile = Config::getString(Config::KEY_FRAGMENTS_INPUTFILE, "");
    _splitPath = Config::getString(Config::KEY_FRAGMENTS_SPLITPATH, "");
    _extrType = (EXTRTYPE)Config::getInt(Config::KEY_FRAGMENTS_EXTRACTIONTYPE, ET_ALLDOCS);
    _minDoc = Config::getInt(Config::KEY_FRAGMENTS_MINDOC, 1);
    _maxDoc = Config::getInt(Config::KEY_FRAGMENTS_MAXDOC, 1000);
    _extractFolder = Config::getString(Config::KEY_FRAGMENTS_EXTRACTFOLDER, "");
    _isMakeSubFolders = Config::getBool(Config::KEY_FRAGMENTS_MAKESUBFOLDERS, true);
    _subFoldersEachNFiles = Config::getInt(Config::KEY_FRAGMENTS_SUBFOLDERSEACH, 1000);
    Config::loadStringArray(Config::KEY_FRAGMENTS_SUBFOLDERSNAMEPATTERN, _subfolderNamePattern);
    Config::loadStringArray(Config::KEY_FRAGMENTS_FILESNAMEPATTERN, _filesNamePattern);
    _isReverseRange = Config::getBool(Config::KEY_FRAGMENTS_REVERSERANGE, false);
    _operationType = static_cast<EOperationType>(Config::getInt(Config::KEY_FRAGMENTS_OPERATION_TYPE, OperationSplit));
    //-------------------
    _attributeName = Config::getString(Config::KEY_FRAGMENTS_ATTRIBUTENAME, "");
    _comparisonTerm = Config::getString(Config::KEY_FRAGMENTS_COMPARISONTERM, "");
    _comparisonType = (ECfrOp) Config::getInt(Config::KEY_FRAGMENTS_COMPARISONTYPE, CFR_EQ);
    if(CFR_EQ != _comparisonType) {
        _comparisonType = CFR_NEQ ;
    }
    _filterTextForPath = Config::getBool(Config::KEY_FRAGMENTS_FILTERTEXTFORPATH, false);
    Config::loadStringArray(Config::KEY_FRAGMENTS_FILTERTEXT_PATH, _pathsForFilterText);

    //-------------------
    _splitDepth = Config::getInt(Config::KEY_FRAGMENTS_DEPTH, 1);
    _splitType = (ESplitType)Config::getInt(Config::KEY_FRAGMENTS_SPLITTYPE, SplitUsingPath);
    //-------------------
}

void ExtractionOperation::saveSettings()
{
    if(!_inputFile.isEmpty()) {
        Config::saveString(Config::KEY_FRAGMENTS_INPUTFILE, _inputFile);
    }
    if(!_splitPath.isEmpty()) {
        Config::saveString(Config::KEY_FRAGMENTS_SPLITPATH, _splitPath);
    }
    Config::saveInt(Config::KEY_FRAGMENTS_EXTRACTIONTYPE, _extrType);
    Config::saveInt(Config::KEY_FRAGMENTS_MINDOC, _minDoc);
    Config::saveInt(Config::KEY_FRAGMENTS_MAXDOC, _maxDoc);
    Config::saveString(Config::KEY_FRAGMENTS_EXTRACTFOLDER, _extractFolder);
    Config::saveBool(Config::KEY_FRAGMENTS_MAKESUBFOLDERS, _isMakeSubFolders);
    Config::saveInt(Config::KEY_FRAGMENTS_SUBFOLDERSEACH, _subFoldersEachNFiles);
    Config::saveStringArray(Config::KEY_FRAGMENTS_SUBFOLDERSNAMEPATTERN, _subfolderNamePattern);
    Config::saveStringArray(Config::KEY_FRAGMENTS_FILESNAMEPATTERN, _filesNamePattern);
    Config::saveBool(Config::KEY_FRAGMENTS_REVERSERANGE, _isReverseRange);
    Config::saveInt(Config::KEY_FRAGMENTS_OPERATION_TYPE, _operationType);
    Config::saveBool(Config::KEY_FRAGMENTS_FILTERTEXTFORPATH, _filterTextForPath);
    Config::saveStringArray(Config::KEY_FRAGMENTS_FILTERTEXT_PATH, _pathsForFilterText);

    //------------
    Config::saveString(Config::KEY_FRAGMENTS_ATTRIBUTENAME, _attributeName);
    Config::saveString(Config::KEY_FRAGMENTS_COMPARISONTERM, _comparisonTerm);
    Config::saveInt(Config::KEY_FRAGMENTS_COMPARISONTYPE,  _comparisonType);
    //----------------
    Config::saveInt(Config::KEY_FRAGMENTS_DEPTH,  _splitDepth);
    Config::saveInt(Config::KEY_FRAGMENTS_SPLITTYPE,  _splitType);
}

void ExtractionOperation::saveSettingsForExtractionFragmentNumber(const QString &filePath, const int fragment, const int depth)
{
    Config::saveInt(Config::KEY_FRAGMENTS_DEPTH, depth);
    Config::saveInt(Config::KEY_FRAGMENTS_SPLITTYPE,  SplitUsingDepth);
    Config::saveString(Config::KEY_FRAGMENTS_INPUTFILE, filePath);
    Config::saveInt(Config::KEY_FRAGMENTS_EXTRACTIONTYPE, ET_RANGE);
    Config::saveInt(Config::KEY_FRAGMENTS_MINDOC, fragment);
    Config::saveInt(Config::KEY_FRAGMENTS_MAXDOC, fragment);
    Config::saveInt(Config::KEY_FRAGMENTS_OPERATION_TYPE, OperationSplit);
}


ExtractionOperation::EParamErrors ExtractionOperation::checkParameters()
{
    if(_inputFile.isEmpty()) {
        return ParamErrorNoInputFile;
    }

    if(_splitType == SplitUsingPath) {
        if(_splitPath.isEmpty()) {
            return ParamErrorNoSplitPath;
        }
    } else if(_splitType == SplitUsingDepth) {
        if(_splitDepth <= 0) {
            return ParamErrorSplitDepth;
        }
    } else {
        return ParamErrorSplitType;
    }

    if(_isExtractDocuments) {
        if(isExtractRange()) {
            if(0 == _minDoc) {
                return ParamErrorNoMinRange ;
            }
            if(0 == _maxDoc) {
                return ParamErrorNoMaxRange ;
            }
            if(_maxDoc < _minDoc) {
                return ParamErrorNoRange ;
            }
        } else if(isExtractCfr()) {
            switch(_comparisonType) {
            default:
                return ParamErrorCfrType;
                break;
            case CFR_EQ:
            case CFR_NEQ:
                break;
            }
            if(_attributeName.isEmpty()) {
                return ParamErrorCfrAttr ;
            }
        } else if(! isExtractAllDocuments()) {
            return ParamErrorCfrType ;
        }
        if(_extractFolder.isEmpty()) {
            return ParamErrorNoExtractFolder ;
        }
        if(_isMakeSubFolders) {
            if(0 == _subFoldersEachNFiles) {
                return ParamErrorSubFolderEach ;
            }
            if(_subfolderNamePattern.isEmpty() || _subfolderNamePattern.contains("")) {
                return ParamErrorSubFolderNamePattern ;
            }
        }
        if(_filesNamePattern.empty() || _filesNamePattern.contains("")) {
            return ParamErrorFilesNamePattern ;
        }
    }
    if(_filterTextForPath) {
        QString path = pathForDeleteText();
        if(path.isEmpty()) {
            return ParamErrorNoDeleteTextPath;
        }
        // discard the first slash
        QString testPath = path ;
        if(testPath.startsWith("/")) {
            testPath = testPath.mid(1);
        }
        QStringList tags = testPath.split("/");
        bool exists = false;
        foreach(QString tag, tags) {
            if(!Utils::checkXMLName(tag)) {
                return ParamErrorNoDeleteTextPath;
            }
            exists = true ;
        }
        if(!exists) {
            return ParamErrorNoDeleteTextPath;
        }
    }
    switch(_operationType) {
    case OperationSplit:
    case OperationFilter:
    case OperationExportAndGroupXML:
    case OperationExportAndGroupCSV:
        break;
    default:
        return ParamErrorBadOperationType ;
    }

    //---------------------------------
    return ParamNoError ;
}

void ExtractionOperation::addFolderPattern(const QString &str)
{
    if(!str.isEmpty()) {
        _subfolderNamePattern.append(str);
    }
}

void ExtractionOperation::addFileNamePattern(const QString &str)
{
    if(!str.isEmpty()) {
        _filesNamePattern.append(str);
    }
}

//----------------------- getters and setters --------------------

void ExtractionOperation::setInputFile(const QString &value)
{
    _inputFile = value ;
}

QString ExtractionOperation::inputFile()
{
    return _inputFile ;
}

void ExtractionOperation::setSplitPath(const QString &value)
{
    _splitPath = value;
}

QString ExtractionOperation::splitPath()
{
    return _splitPath ;
}

void ExtractionOperation::setExtractAllDocuments()
{
    _extrType = ET_ALLDOCS ;
}

bool ExtractionOperation::isExtractAllDocuments()
{
    return (ET_ALLDOCS == _extrType) ;
}

void ExtractionOperation::setExtractRange()
{
    _extrType = ET_RANGE ;
}

bool ExtractionOperation::isExtractRange()
{
    return (ET_RANGE == _extrType) ;
}

void ExtractionOperation::setExtractCfr()
{
    _extrType = ET_USETERM ;
}

bool ExtractionOperation::isExtractCfr()
{
    return (ET_USETERM == _extrType) ;
}

void ExtractionOperation::setMinDoc(const unsigned int value)
{
    _minDoc = value;
}

unsigned int ExtractionOperation::minDoc()
{
    return _minDoc ;
}

void ExtractionOperation::setMaxDoc(const unsigned int value)
{
    _maxDoc = value;
}

unsigned int ExtractionOperation::maxDoc()
{
    return _maxDoc ;
}

void ExtractionOperation::setExtractFolder(const QString &value)
{
    _extractFolder = value;
}

QString ExtractionOperation::extractFolder()
{
    return _extractFolder ;
}

void ExtractionOperation::setIsMakeSubFolders(const bool value)
{
    _isMakeSubFolders = value;
}

bool ExtractionOperation::isMakeSubFolders()
{
    return _isMakeSubFolders ;
}

void ExtractionOperation::setSubFoldersEachNFiles(const uint value)
{
    _subFoldersEachNFiles = value;
}

uint ExtractionOperation::subFoldersEachNFiles()
{
    return _subFoldersEachNFiles ;
}

void ExtractionOperation::setSubfolderNamePattern(const QStringList value)
{
    _subfolderNamePattern = value;
}

QStringList ExtractionOperation::subfolderNamePattern()
{
    return _subfolderNamePattern ;
}

void ExtractionOperation::setFilesNamePattern(const QStringList value)
{
    _filesNamePattern = value;
}

QStringList ExtractionOperation::filesNamePattern()
{
    return _filesNamePattern ;
}

void ExtractionOperation::setExtractDocuments(const bool value)
{
    _isExtractDocuments = value ;
}

bool ExtractionOperation::isExtractDocuments()
{
    return _isExtractDocuments;
}

bool ExtractionOperation::isError()
{
    return _isError ;
}

ExtractionOperation::EXMLErrors ExtractionOperation::error()
{
    return _errorCode;
}

QString ExtractionOperation::errorMessage()
{
    return _errorMessage ;
}

bool ExtractionOperation::isAborted()
{
    return _isAborted;
}

void ExtractionOperation::setAborted(const bool value)
{
    _isAborted = value ;
    if(value) {
        if(NULL != _results) {
            _results->setAborted(true);
        }
    }
}

ExtractionOperation::EOperationType ExtractionOperation::OperationType()
{
    return _operationType;
}

void ExtractionOperation::setOperationType(const EOperationType value)
{
    _operationType = value ;
}

bool ExtractionOperation::isReverseRange()
{
    return _isReverseRange ;
}

void ExtractionOperation::setReverseRange(const bool value)
{
    _isReverseRange = value ;
}

bool ExtractionOperation::isDebug()
{
    return _debug ;
}

void ExtractionOperation::setDebug(const bool value)
{
    _debug = value ;
}


ExtractResults *ExtractionOperation::results()
{
    return _results ;
}

QString ExtractionOperation::attributeName()
{
    return _attributeName;
}

void ExtractionOperation::setAttributeName(const QString &value)
{
    _attributeName = value ;
}

QString ExtractionOperation::comparisonTerm()
{
    return _comparisonTerm;
}

void ExtractionOperation::setComparisonTerm(const QString &value)
{
    _comparisonTerm = value ;
}

ExtractionOperation::ECfrOp ExtractionOperation::comparisonType()
{
    return _comparisonType;
}

void ExtractionOperation::setComparisonType(const ECfrOp value)
{
    _comparisonType = value ;
}

ExtractionOperation::EXTRTYPE ExtractionOperation::extrType()
{
    return _extrType ;
}

void ExtractionOperation::setExtrType(const EXTRTYPE value)
{
    _extrType = value ;
}

ExtractionOperation::ESplitType ExtractionOperation::splitType()
{
    return _splitType ;
}

void ExtractionOperation::setSplitType(const ESplitType value)
{
    _splitType = value ;
}

int ExtractionOperation::splitDepth()
{
    return _splitDepth ;
}

void ExtractionOperation::setSplitDepth(const int value)
{
    _splitDepth = value ;
}

bool ExtractionOperation::isFilterTextForPath()
{
    return _filterTextForPath ;
}

void ExtractionOperation::setFilterTextForPath(const bool value)
{
    _filterTextForPath = value ;
}

void ExtractionOperation::setPathForDeleteText(const QString &value)
{
    _pathsForFilterText.clear();
    _pathsForFilterText.append(value);
}

QString ExtractionOperation::pathForDeleteText()
{
    if(_pathsForFilterText.size()) {
        return _pathsForFilterText.at(0);
    } else {
        return "";
    }
}

bool ExtractionOperation::handleExportedElement(ExtractInfo &info, QXmlStreamReader &reader)
{
    // startElement, verify, please
    if(!reader.isStartElement()) {
        setError(EXML_UnexpectedElementError, tr("Unexpected object in input stream type is: %1, waiting for: %2").arg(reader.tokenType()).arg(QXmlStreamReader::StartElement));
        return false ;
    }
    if(isExportCSV()) {
        QString separator = Utils::separatorStringCSV();

        QXmlStreamAttributes streamAttributes = reader.attributes();
        QHash<int, QString> columnsForAttributes;
        QList<int> columnPositions;
        // insert columns
        foreach(const QXmlStreamAttribute & attr, streamAttributes) {
            QString attrName = attr.qualifiedName().toString();
            if(!info._mapForCSVColumns.contains(attrName)) {
                info._mapForCSVColumns.insert(attrName, info._mapForCSVColumns.size());
            }
            const int position = info._mapForCSVColumns[attrName] ;
            columnsForAttributes.insert(position, attrName);
            columnPositions << position ;
        }
        // sort existing attributes for column
        qSort(columnPositions);
        // write attributes, with empty columns in between.
        int index = 0 ;
        foreach(const int pos, columnPositions) {
            for(int fromLast = index ; fromLast < pos ; fromLast ++) {
                if(0 != fromLast) {
                    info.csvWriter << separator;
                }
                index++;
            }
            const QString &attrName = columnsForAttributes[pos];
            const QString &attrValue = streamAttributes.value(attrName).toString();
            info.csvWriter << Utils::valueStringCSV(attrValue, (0 == index));
            index ++ ;
        }
        // rfc 4180
        info.csvWriter << info.crLf ;
        if(info.csvWriter.status() != QTextStream::Ok) {
            handleWriteError();
            return false ;
        }
        if(info.csvTempFile.error() != QFile::NoError) {
            handleWriteError();
            return false ;
        }
    } else {
        info.xmlWriter.writeCurrentToken(reader);
        info.xmlWriter.writeEndElement();
    }
    if(info.outputFile.error() != QFile::NoError) {
        handleWriteError();
        return false ;
    }
    return true ;
}

//--------------------------------------------------------------------------

ExtractInfo::ExtractInfo() : xmlWriter(&outputFile)
{
    currentDocument = 0 ;
    currentSubfolderDocument = 0 ;
}

ExtractInfo::~ExtractInfo()
{
    if(outputFile.isOpen()) {
        outputFile.close();
    }
    if(csvTempFile.isOpen()) {
        csvTempFile.close();
    }
}

//
