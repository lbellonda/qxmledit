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

#include "comparexml.h"
#include "app.h"
#include "mainwindow.h"
#include "qxmleditconfig.h"
#include "utils.h"

#define TEST_FILE1   "../test/data/test1.xml"
#define TEST_FILE2  "../test/data/test2.xml"
#define TEST_FILE_MIXED  "../test/data/test_mixed_base.xml"
#define TEST_FILE_MIXED_DELTA  "../test/data/test_mixed_base_delta.xml"

CompareXML::CompareXML()
{
    _showMessages = false ;
}

void CompareXML::debug(const QString & msg)
{
    if(_showMessages) {
        Utils::error(msg);
    }
}

QString CompareXML::doc1String()
{
    return _doc1String;
}

QString CompareXML::doc2String()
{
    return _doc2String;
}

void CompareXML::dumpErrorCause()
{
    fprintf(stderr, "Error is:'%s'\n", errorString().toLatin1().data());
    fprintf(stderr, "DOC1 is:\n%s\n", doc1String().toLatin1().data());
    fprintf(stderr, "\n\nDOC2 is \n%s\n", doc2String().toLatin1().data());
    QString pathString ;
    foreach (int i, _path) {
        pathString += QString("%1,").arg(i);
    }
    pathString += "-";
    fprintf(stderr, "\nPath is:%s\n", pathString.toLatin1().data());
}

bool CompareXML::comparePlainDocuments()
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
    MainWindow mainWindow(false, app.data());

    mainWindow.loadFile(QString(TEST_FILE1));
    QByteArray resultData = mainWindow.getRegola()->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    if(!loadFileIntoDocument(TEST_FILE1, document1)) {
        return false;
    }
    QBuffer outputData(&resultData);
    if(!loadFileIntoDocument(&outputData, document2)) {
        return false;
    }
    bool result = compareDomDocuments(document1, document2);

    return result ;
}

bool CompareXML::comparePlainDocumentsFalse()
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
    MainWindow mainWindow(false, app.data());

    mainWindow.loadFile(QString(TEST_FILE1));
    QByteArray resultData = mainWindow.getRegola()->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    if(!loadFileIntoDocument(TEST_FILE2, document1)) {
        return false;
    }
    QBuffer outputData(&resultData);
    if(!loadFileIntoDocument(&outputData, document2)) {
        return false;
    }
    bool result = compareDomDocuments(document1, document2);

    return result ;
}

bool CompareXML::compareMixedDocuments()
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
    MainWindow mainWindow(false, app.data());

    mainWindow.loadFile(QString(TEST_FILE_MIXED));
    QByteArray resultData = mainWindow.getRegola()->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    if(!loadFileIntoDocument(TEST_FILE_MIXED, document1)) {
        return false;
    }
    QBuffer outputData(&resultData);
    if(!loadFileIntoDocument(&outputData, document2)) {
        return false;
    }
    bool result = compareDomDocuments(document1, document2);

    return result ;
}

bool CompareXML::compareMixedDocumentsFalse()
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
    MainWindow mainWindow(false, app.data());

    mainWindow.loadFile(QString(TEST_FILE_MIXED));
    QByteArray resultData = mainWindow.getRegola()->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    if(!loadFileIntoDocument(TEST_FILE_MIXED_DELTA, document1)) {
        return false;
    }
    QBuffer outputData(&resultData);
    if(!loadFileIntoDocument(&outputData, document2)) {
        return false;
    }
    bool result = compareDomDocuments(document1, document2);

    return result ;
}

bool CompareXML::loadFileIntoDocument(const QString &filePath, QDomDocument &document)
{
    if(!filePath.isEmpty()) {
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly)) {
            if(document.setContent(&file)) {
                file.close();
                return true;
            }
            file.close();
        }
    }
    return false;
}

bool CompareXML::loadFileIntoDocument(QIODevice *ioDevice, QDomDocument &document)
{
    bool isOk = false;
    if(ioDevice->open(QIODevice::ReadOnly)) {
        QString errorMsg;
        int errorLine=0;
        int errorColumn=0;
        if(document.setContent(ioDevice, &errorMsg, &errorLine, &errorColumn)) {
            isOk = true;
        } else {
            ioDevice->seek(0);
            ioDevice->reset();
            QString str = QString(ioDevice->readAll());
            str = str.left(100);
            error(QString("error:'%1', line:%2, col:%3, data:'%4'").arg(errorMsg).arg(errorLine).arg(errorColumn).arg(str));
        }
        ioDevice->close();
    }
    return isOk;
}

bool CompareXML::loadStringIntoDocument(const QString &string, QDomDocument &document)
{
    bool isOk = false;
    QString errorMsg;
    int errorLine=0;
    int errorColumn=0;
    if(document.setContent(string, &errorMsg, &errorLine, &errorColumn)) {
        isOk = true;
    } else {
        QString str = string.left(100);
        error(QString("error:'%1', line:%2, col:%3, data:'%4'").arg(errorMsg).arg(errorLine).arg(errorColumn).arg(str));
    }
    return isOk;
}


void CompareXML::reset()
{
    _path.clear();
    _doc1String = "" ;
    _doc2String = "" ;
}

bool CompareXML::compareDomDocuments(QDomDocument &doc1, QDomDocument &doc2)
{
    reset();
    if(comparaFigli(doc1, doc2)) {
        return true ;
    }
    _doc1String = doc1.toString(1);
    _doc2String = doc2.toString(1);
    return false;
}

bool CompareXML::compareAFileWithRegola(const QString &file, Regola *regola)
{
    return compareFileWithRegola(regola, file );
}

bool CompareXML::compareFileWithRegola(Regola *regola, const QString &file)
{
    QByteArray resultData = regola->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    if(!loadFileIntoDocument(file, document1)) {
        return error(QString("load file step 1"));
    }
    QBuffer outputData(&resultData);
    if(!loadFileIntoDocument(&outputData, document2)) {
        return error("load file step 2");
    }
    bool result = compareDomDocuments(document1, document2);
    if( !result ) {
        QString strDoc(resultData);
        return error(QString("comparing files, doc 2 is:'%1'").arg(strDoc));
    }
    return true ;
}

bool CompareXML::compareRegolaWithRegola(Regola *regola1, Regola *regola2)
{
    QByteArray resultData1 = regola1->writeMemory();
    QByteArray resultData2 = regola2->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    QBuffer outputData1(&resultData1);
    if(!loadFileIntoDocument(&outputData1, document1)) {
        return error("load regola 2");
    }
    QBuffer outputData2(&resultData2);
    if(!loadFileIntoDocument(&outputData2, document2)) {
        return error("load regola step 2");
    }
    bool result = compareDomDocuments(document1, document2);
    if( !result ) {
        QString strDoc(resultData2);
        return error(QString("comparing regola, doc 2 is:'%1'").arg(strDoc));
    }
    return true ;
}

bool CompareXML::compareXMLAsStringAsUtf8(const QString &s1, const QString & s2)
{
    QByteArray resultData1 = s1.toUtf8();
    QByteArray resultData2 = s2.toUtf8();
    QDomDocument document1;
    QDomDocument document2;
    QBuffer outputData1(&resultData1);
    if(!loadFileIntoDocument(&outputData1, document1)) {
        return error("load regola 2");
    }
    QBuffer outputData2(&resultData2);
    if(!loadFileIntoDocument(&outputData2, document2)) {
        return error("load regola step 2");
    }
    bool result = compareDomDocuments(document1, document2);
    if( !result ) {
        return error(QString("comparing regola, msg:'%1'\ndoc 1:\n%2\ndoc 2 is:'%3'\n").arg(errorString()).arg(s1).arg(s2));
    }
    return true ;
}

bool CompareXML::compareFileWithRegola(Regola *regola, QIODevice *device)
{
    QByteArray resultData = regola->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    if(!loadFileIntoDocument(device, document1)) {
        return error(QString("load file step 1"));
    }
    QBuffer outputData(&resultData);
    if(!loadFileIntoDocument(&outputData, document2)) {
        return error("load file step 2");
    }
    bool result = compareDomDocuments(document1, document2);
    if( !result ) {
        QString strDoc(resultData);
        return error(QString("comparing files, doc 2 is:'%1'").arg(strDoc));
    }
    return true ;
}

bool CompareXML::compareBufferWithFile(QIODevice *regola, const QString &file)
{
    QDomDocument document1;
    QDomDocument document2;
    if(!loadFileIntoDocument(file, document1)) {
        return error(QString("load file step 1"));
    }
    if(!loadFileIntoDocument(regola, document2)) {
        return error("load file step 2");
    }
    bool result = compareDomDocuments(document1, document2);
    if( !result ) {
        return error(QString("comparing files, file is :'%1'").arg(file));
    }
    return true ;
}

bool CompareXML::compareStringWithFile(const QString &text, const QString &file)
{
    QDomDocument document1;
    QDomDocument document2;
    if(!loadStringIntoDocument(text, document1)) {
        return error(QString("load file step 1"));
    }
    if(!loadFileIntoDocument(file, document2)) {
        return error("load file step 2");
    }
    bool result = compareDomDocuments(document1, document2);
    if( !result ) {
        return error(QString("comparing string and files, file is :'%1'").arg(file));
    }
    return true ;
}

bool CompareXML::compareFiles(const QString &file1, const QString &file2)
{
    QDomDocument document1;
    QDomDocument document2;
    if(!loadFileIntoDocument(file1, document1)) {
        return error(QString("Unable to load %1").arg(file1));
    }
    if(!loadFileIntoDocument(file2, document2)) {
        return error(QString("Unable to load %1").arg(file2));
    }
    bool result = compareDomDocuments(document1, document2);

    return result ;
}


bool CompareXML::compareFiles(const QString &file1, QIODevice *file2)
{
    QDomDocument document1;
    QDomDocument document2;
    if(!loadFileIntoDocument(file1, document1)) {
        return error("error loading file 1");
    }
    if(!loadFileIntoDocument(file2, document2)) {
        return error("error loading file 2");
    }
    bool result = compareDomDocuments(document1, document2);

    return result ;
}
QString CompareXML::getStringData(const QString &str)
{
    int len = str.length();
    QString data = QString("len:%1, ").arg(len);
    for(int i = 0 ; i < len ; i ++) {
        QChar ch = str.at(i);
        QString s ;
        s.setNum(ch.unicode(), 16);
        data += s ;
    }
    data += ":" ;
    data += str ;
    data += ":\n" ;
    return data ;
}

void CompareXML::compareDifferentStrings(const QString &str1, const QString &str2)
{
    if(!_showMessages) {
        return ;
    }
    QString one = getStringData(str1);
    QString two = getStringData(str2);
    QString msg = QString("Strings comparison:\n%1 %2").arg(one).arg(two);
    Utils::message(msg);
}


bool CompareXML::compareAttributes(QDomElement element1, QDomElement element2)
{
    if(!element1.hasAttributes() && !element2.hasAttributes()) {
        return true ;
    }
    QDomNamedNodeMap attrList1 = element1.attributes();
    QDomNamedNodeMap attrList2 = element2.attributes();
    int attrMax1 = attrList1.count();
    int attrMax2 = attrList2.count();
    if(attrMax1 != attrMax2) {
        debug("attr1");
        return errorFlag(element1, QString("Attributes count 1:%1 2:%2").arg(attrMax1).arg(attrMax2)) ;
    }
    QMap<QString, QString> am1, am2;
    for(int a = 0 ; a < attrMax1 ; a ++) {
        QDomAttr attr1 = attrList1.item(a).toAttr();
        QDomAttr attr2 = attrList2.item(a).toAttr();
        am1[attr1.name()] = attr1.value();
        am2[attr2.name()] = attr2.value();
    }
    foreach(QString key, am1.keys()) {
        if(!am2.contains(key)) {
            debug(QString("attr2: '%1'").arg(key));
            return errorFlag(element1, QString("2 does not contain attribute '%1'").arg(key)) ;
        }
        if(am1[key] != am2[key]) {
            debug("attr3");
            return errorFlag(element1, QString("attributes value differs element: '%4' name:'%1', val 1:'%2', val 2:'%3'").arg(key).arg(am1[key]).arg(am2[key]).arg(element1.tagName())) ;
        }
    }
    return true ;
}

void CompareXML::dumpNodes(QDomNodeList &childNodes)
{
    int nodiCount = childNodes.size();
    for(int i = 0 ; i < nodiCount ; i ++) {
        QString dumpText ;
        QDomNode childNode = childNodes.item(i) ;

        if(childNode.isElement()) {
            dumpText = QString("%1 element %2").arg(i).arg(childNode.toElement().tagName());
        } else if(childNode.isCDATASection()) {
            dumpText = QString("%1 CDATA %2").arg(i).arg(childNode.toCDATASection().data());
        } else if(childNode.isText()) {
            dumpText = QString("%1 text %2").arg(i).arg(childNode.toText().data());
        } else if(childNode.isProcessingInstruction()) {
            dumpText = QString("%1 ProcInstr %2-%3").arg(i).arg(childNode.toProcessingInstruction().data()).arg(childNode.toProcessingInstruction().target());
        } else if(childNode.isComment()) {
            dumpText = QString("%1 comment %2").arg(i).arg(childNode.toComment().data());
        } else if(childNode.isEntity()) {
            dumpText = QString("%1 entity ").arg(i);
        } else if(childNode.isEntityReference()) {
            dumpText = QString("%1 entity Ref").arg(i);
        } else if(childNode.isDocumentType()) {
            QDomDocumentType dtype = childNode.toDocumentType();
            dumpText = QString("%1 Document Type %2, system %3, public %4").arg(i).arg(dtype.name()).arg(dtype.systemId()).arg(dtype.publicId());
        } //if
        fprintf(stderr, "%s\n", dumpText.toLatin1().data());
    }//for
    fflush(stdout);
}

void CompareXML::dumpTwoNodes(QDomNodeList &childNodes1, QDomNodeList &childNodes2)
{
    if(!_showMessages) {
        return ;
    }
    fprintf(stderr, "Nodes dump:\n");
    fprintf(stderr, "Nodes 1:\n");
    dumpNodes(childNodes1);
    fprintf(stderr, "Nodes 2:\n");
    dumpNodes(childNodes2);
    fprintf(stderr, "------------\n");
}

bool CompareXML::errorFlag(QDomNode errorNode, const QString &msg)
{
    int maxItems = 5000;
    // path of the error
    QDomNode parent = errorNode.parentNode();
    while( !parent.isNull() ) {
        QDomNodeList parentChildrens = parent.childNodes();
        int maxIndex = parentChildrens.count();
        maxItems--;
        if( maxItems < 0 ) {
            goto exit1;
        }
        for( int index = 0; index < maxIndex ; index ++ ) {
            maxItems--;
            if( maxItems < 0 ) {
                goto exit1 ;
            }
            QDomNode nd = parentChildrens.at(index);
            if( nd == errorNode ) {
                _path.insert(0, index);
                errorNode = parent ;
                parent = parent.parentNode() ;
                break;
            }
        }
    }
    //------
exit1:
    return error(msg);
}

bool CompareXML::comparaFigli(QDomNode &node1, QDomNode &node2)
{
    int nodi1Count = node1.childNodes().count();
    int nodi2Count = node2.childNodes().count();

    if(nodi1Count != nodi2Count) {
        debug("elem1");
        QDomNodeList childNodes1 = node1.childNodes();
        QDomNodeList childNodes2 = node2.childNodes();
        dumpTwoNodes(childNodes1, childNodes2);
        return errorFlag(node1, QString("nodes count a:%1, b:%2 at").arg(nodi1Count).arg(nodi2Count) );
    }
    QDomNodeList childNodes1 = node1.childNodes();
    QDomNodeList childNodes2 = node2.childNodes();

    for(int i = 0 ; i < nodi1Count ; i ++) {
        QDomNode childNode1 = childNodes1.item(i) ;
        QDomNode childNode2 = childNodes2.item(i) ;

        if(childNode1.isElement()) {
            if(!childNode2.isElement()) {
                debug("elem2");
                return errorFlag(childNode1, QString("1 is element, 2 no: '%1', '%2'").arg(childNode1.nodeName()).arg(childNode2.nodeName()));
            }
            QDomElement element1 = childNode1.toElement();
            QDomElement element2 = childNode2.toElement();
            if(element1.tagName() != element2.tagName()) {
                return errorFlag( childNode1, QString("elements: '%1' ' %2'").arg(element1.tagName()).arg(element2.tagName()));
            }
            if(! compareAttributes(element1, element2)) {
                debug("elem4");
                return false;
            }
        } else if(childNode1.isCDATASection()) {
            if(!childNode2.isCDATASection()) {
                debug("elem5");
                dumpTwoNodes(childNodes1, childNodes2);
                return errorFlag(childNode1, QString("1 is CData, 2 no: '%1', '%2'").arg(childNode1.nodeName()).arg(childNode2.nodeName()));
            }
            QDomCDATASection text1 = childNode1.toCDATASection();
            QDomCDATASection text2 = childNode2.toCDATASection();
            QString s1 = text1.data() ;
            QString s2 = text2.data() ;
            s1.replace("\r\n", "\n");
            s2.replace("\r\n", "\n");
            if(s1 != s2) {
                debug("elem6");
                compareDifferentStrings(text1.data(), text2.data());
                return errorFlag(childNode1, QString("CData differs: '%1', '%2'").arg(text1.data()).arg(text2.data()));
            }
        } else if(childNode1.isText()) {
            if(!childNode2.isText()) {
                debug("elem7");
                return errorFlag(childNode1, QString("1 is text, 2 no: '%1', '%2'").arg(childNode1.nodeName()).arg(childNode2.nodeName()));
            }
            QDomText text1 = childNode1.toText();
            QDomText text2 = childNode2.toText();
            QString s1 = text1.data() ;
            QString s2 = text2.data() ;
            s1.replace("\r\n", "\n");
            s2.replace("\r\n", "\n");
            s1.replace("\r", "\n");
            s2.replace("\r", "\n");
            s1 = s1.trimmed();
            s2 = s2.trimmed();
            if(s1 != s2) {
                compareDifferentStrings(text1.data(), text2.data());
                debug(QString("elem8: %1, %2").arg(text1.data()).arg(text2.data()));
                return errorFlag(childNode1, QString("text differs: on element '%3' %1', '%2'").arg(text1.data()).arg(text2.data()).arg(node1.nodeName()));
            }
        } else if(childNode1.isProcessingInstruction()) {
            if(!childNode2.isProcessingInstruction()) {
                debug("elem9");
                return errorFlag(childNode1, QString("1 is isProcessingInstruction, 2 no: '%1', '%2'").arg(childNode1.nodeName()).arg(childNode2.nodeName()));
            }
            QDomProcessingInstruction procInstrNode1 = childNode1.toProcessingInstruction();
            QDomProcessingInstruction procInstrNode2 = childNode2.toProcessingInstruction();
            if(procInstrNode1.data() != procInstrNode2.data()) {
                debug("elem10");
                debug(QString("elem10: %1, %2").arg(procInstrNode1.data()).arg(procInstrNode2.data()));
                return errorFlag(childNode1, QString("PI differ: '%1', '%2'").arg(procInstrNode1.data()).arg(procInstrNode2.data()));
            }
            if(procInstrNode1.target() != procInstrNode2.target()) {
                debug("elem11");
                return errorFlag(childNode1, QString("PI target differ: '%1', '%2'").arg(procInstrNode1.target()).arg(procInstrNode2.target()));
            }
        } else if(childNode1.isComment()) {
            if(!childNode2.isComment()) {
                debug("elem12");
                return errorFlag(childNode1, QString("1 is comment, 2 no: '%1', '%2'").arg(childNode1.nodeName()).arg(childNode2.nodeName()));
            }
            QDomComment commentNode1 = childNode1.toComment();
            QDomComment commentNode2 = childNode2.toComment();
            QString s1 = commentNode1.data() ;
            QString s2 = commentNode2.data() ;
            s1.replace("\r\n", "\n");
            s2.replace("\r\n", "\n");

            if(s1 != s2) {
                compareDifferentStrings(commentNode1.data(), commentNode2.data());
                debug("elem13");
                return errorFlag(childNode1, QString("comments differ: '%1', '%2'").arg(commentNode1.data()).arg(commentNode2.data()));
            }
        } else if(childNode1.isEntity()) {
            //TODO
        } else if(childNode1.isEntityReference()) {
            //TODO
        } else if(childNode1.isDocumentType()) {
            QDomDocumentType dtype1 = childNode1.toDocumentType();
            QDomDocumentType dtype2 = childNode2.toDocumentType();
            if( dtype1.name() != dtype2.name() ) {
                debug("elem14");
                return errorFlag(childNode1, "");
            }
            if( dtype1.systemId() != dtype2.systemId() ) {
                debug("elem15");
                return errorFlag(childNode1, "");
            }
            if( dtype1.publicId() != dtype2.publicId() ) {
                debug("elem16");
                return errorFlag(childNode1, "");
            }
        } //if
        if(!comparaFigli(childNode1, childNode2)) {
            debug("elem14");
            return false;
        }
    }//for
    return true;
}//

