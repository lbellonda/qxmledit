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
#include <QFile>
#include "balsamiqwork.h"
#include "balsamiqproxy.h"
#include "utils.h"
#include "qxmleditconfig.h"
#include <QFileInfo>

//---------------------------------------------------------------------------

//----------------------------------------------------------------------------
#define QB_mockup   "mockup"
#define QB_controls  "controls"
#define QB_control  "control"
#define QB_controlProperties "controlProperties"

#define QB_a_controlType  "controlTypeID"
#define QB_a_controlId  "controlID"

#define MX_APPLICATION_TAG "mx:Application"
#define NUM_PHASES  (4)
#define PHASE_SCAN  0
#define PHASE_CALCULATION  1
#define PHASE_GLOBALS  2
#define PHASE_CONTROLS  3
//----------------------------------------------------------------------------

BalsamiqWork::BalsamiqWork()
{
    _defaultControl = NULL ;
    clearError();
    _rootControl = NULL ;
}

BalsamiqWork::~BalsamiqWork()
{
    resetControls();
}

bool BalsamiqWork::exec(QWidget *parent, const bool isOverwriteFiles, const QStringList files, const QString &outputDir)
{
    bool isOk = true;
    resetControls();
    if(!registerControls()) {
        isOk = false;
    } else {
        isOk = work(parent, files, outputDir, isOverwriteFiles);
    }
    resetControls();
    return isOk ;
}

bool BalsamiqWork::execTest(QWidget *parent)
{
    bool isOk = true;
    resetControls();
    if(!registerControls()) {
        isOk = false;
    } else {
        isOk = test(parent);
    }
    resetControls();
    return isOk ;
}

void BalsamiqWork::resetControls()
{
    if(NULL != _rootControl) {
        delete _rootControl ;
        _rootControl = NULL ;
    }
    foreach(BalsamiqControl * control, _controls) {
        delete control ;
    }
    _controls.clear();
}

BalsamiqControl *BalsamiqWork::getControl(const QString &type)
{
    if(_controls.contains(type)) {
        return _controls[type];
    } else {
        return _defaultControl ;
    }
}


bool BalsamiqWork::registerControl(BalsamiqControl *ctrl)
{
    if(NULL == ctrl) {
        return false;
    }
    _controls.insert(ctrl->controlName(), ctrl);
    return true;
}

bool BalsamiqWork::registerControls()
{
    _defaultControl = new BalsamiqGeneric();
    if(!registerControl(_defaultControl)
            ||  !registerControl(new BalsamiqButton())
            ||  !registerControl(new BalsamiqLabel())
            ||  !registerControl(new BalsamiqLinkBar())
            ||  !registerControl(new BalsamiqDataGrid())
            ||  !registerControl(new BalsamiqGroup())
            ||  !registerControl(new BalsamiqList())
            ||  !registerControl(new BalsamiqTabBar())
            ||  !registerControl(new BalsamiqVRule())
            ||  !registerControl(new BalsamiqHRule())
            ||  !registerControl(new BalsamiqTextInput())
            ||  !registerControl(new BalsamiqComboBox())
            ||  !registerControl(new BalsamiqTextArea())
            ||  !registerControl(new BalsamiqParagraph())
            ||  !registerControl(new BalsamiqCheckBox())
            ||  !registerControl(new BalsamiqRadioButton())
            ||  !registerControl(new BalsamiqTree())
            ||  !registerControl(new BalsamiqApplication())
      ) {
        return false ;
    }
    return true;
}


// Recursive function to explore balsamiq tree
bool BalsamiqWork::scanData(const QDomNode &node, BalsamiqProxy *currentControl)
{
    D(printf("scandata %s \n", node.nodeName().toAscii().data()));
    int nodi = node.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = node.childNodes().item(i) ;
        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            if(!scanElement(element, currentControl)) {
                return false ;
            }
        }
    }
    return true;
}

// Recursive function to explore balsamiq tree
bool BalsamiqWork::scanElement(const QDomElement &element, BalsamiqProxy *currentControl)
{
    D(printf("scanElement %s \n", element.tagName().toAscii().data()));
    bool isOk = true ;
    if(element.tagName() == QB_mockup) {
        // Check for only one application.
        if(NULL != _rootControl) {
            setError(tr("Duplicate Application tag found"));
            return false;
        }
        if(!doApplication(element)) {
            return false;
        }

        if(NULL == _rootControl) {
            setError(tr("No root"));
            return false;
        }
        return true ;
    } else if(element.tagName() == QB_controls) {
        if(!doControls(element, currentControl)) {
            return false;
        }
    } else if(element.tagName() == QB_control) {
        if(!doControl(element, currentControl)) {
            return false;
        }
    } else if(element.tagName() == QB_controlProperties) {
        doControlProperties(element, currentControl);
    } else {
        if(!scanData(element, currentControl)) {
            return false;
        }
    }
    return isOk ;
}

bool BalsamiqWork::doApplication(const QDomElement& element)
{
    D(printf("doApplication %s \n", element.tagName().toAscii().data()));
    QString version = element.attribute("version", "") ;
    if(version != "1.0") {
        setError(tr("Version not supported: %1").arg(version)) ;
        return false ;
    }

    bool isOk = false;
    _rootControl = new BalsamiqProxy();
    if(NULL != _rootControl) {
        _rootControl->setType(BMML_APPLICATION_TAG);
        isOk = scanData(element, _rootControl);
        if(!isOk) {
            setError(tr("Application template"));
        }
    } else {
        setError(tr("Out of memory getting root"));
    }

    return isOk && !_isError ;
}

// As stated by specifications, this is can be only child of application, so we can check it.
bool BalsamiqWork::doControls(const QDomElement& element, BalsamiqProxy *currentProxy)
{
    D(printf("doControls %s \n", element.tagName().toAscii().data()));
    if((NULL == currentProxy) || (currentProxy->type() != BMML_APPLICATION_TAG)) {
        setError(tr("Controls tag found in wrong place"));
        return false;
    }
    // delve retrieving all the properties
    return scanData(element, currentProxy) ;
}

bool BalsamiqWork::doControl(const QDomElement &element, BalsamiqProxy *currentProxy)
{
    D(printf("doControl %s \n", element.tagName().toAscii().data()));
    //registra le proprieta';
    BalsamiqProxy *current =  new BalsamiqProxy();
    if(NULL == current) {
        setError(tr("Out of memory for new Proxy"));
        return false ;
    }
    currentProxy->addChild(current);
    if(element.hasAttributes()) {
        QDomNamedNodeMap attrList = element.attributes();
        int attrMax = attrList.count();
        for(int a = 0 ; a < attrMax ; a ++) {
            QDomAttr attr = attrList.item(a).toAttr();
            current->setAttribute(attr.name(), attr.value());
            if(attr.name() == QB_a_controlType) {
                current->setType(attr.value());
            } else if(attr.name() == QB_a_controlId) {
                current->setId(attr.value());
            }
        }
    }
    // delve into this control
    return scanData(element, current) ;
}

void BalsamiqWork::doControlProperties(const QDomElement &father, BalsamiqProxy *proxy)
{
    D(printf("doControlProperties %s \n", father.tagName().toAscii().data()));
    // delve retrieving all the properties
    int nodi = father.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = father.childNodes().item(i) ;
        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            proxy->setProperty(element.tagName(), element.text());
        }
    }
}

bool BalsamiqWork::read(const QString &filePath)
{
    D(printf("read %s \n", filePath.toAscii().data()));
    BalsamiqProxy proxy;
    bool isOk = false;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) {
        if(_inputData.setContent(&file)) {
            isOk = scanData(_inputData, NULL);
        }
        if(!isOk) {
            setError(tr("Unable to parse XML"));
        }
        file.close();
    } else {
        setError(QString(tr("Unable to load MXML file.\n Error code is '%1'")).arg(file.error()));
    }
    return isOk ;
}

void BalsamiqWork::setError(const QString &message)
{
    if(!_errorReason.isEmpty()) {
        _errorReason += "-[" + message + "]";
    } else {
        _errorReason = message ;
    }
    _isError = true ;
}

void BalsamiqWork::clearError()
{
    _errorReason = "" ;
    _isError = false ;
}

bool BalsamiqWork::translate(BalsamiqOpContext &context, const QString &file, const bool isOverwrite)
{
    clearError();
    if(read(file)) {
        if(NULL != _rootControl) {
            QDomDocument outputDocument;
            QDomNode node = outputDocument.createElement("x"); // hack for the document generation
            outputDocument.appendChild(node);
            outputDocument.removeChild(node);
            if(doEmitData(context, _rootControl, outputDocument)) {
                if(Utils::writeXDocumentToFile(outputDocument, _outputFilePath, isOverwrite)) {
                    return true ;
                } else {
                    setError(QString(tr("error writing output data, error: %1")).arg(context.errorCause)) ;
                }
            } else {
                setError(tr("error generating data")) ;
            }
        } else {
            setError(tr("application tag missing")) ;
        }
    } else {
        setError(tr("No data decoded"));
    }
    return false;
}

bool BalsamiqWork::test(QWidget *parent)
{
    QProgressDialog progressDialog(tr("Analyzing.."), tr("Abort"), 0, 1, parent);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setMinimumDuration(0);
    BalsamiqOpContext context ;
    QString outputFile = Config::getString(Config:: KEY_TEST_BALSAMIQ_OUTPUT, "");
    QString inputFile = Config::getString(Config::KEY_TEST_BALSAMIQ_INPUT, "") ;
    if(!setStep(progressDialog, context, 0, tr("Processing: %1").arg(inputFile))) {
        return false;
    }
    if(!doSingleWork(context, inputFile, outputFile, true)) {
        Utils::error(tr("Error :%1").arg(_errorReason));
        return false;
    }
    Utils::message(tr("Operation done"));
    return true;
}

bool BalsamiqWork::work(QWidget *parent, const QStringList &files, const QString &outputDir, const bool isOverwrite)
{
    QProgressDialog progressDialog(tr("Analyzing.."), tr("Abort"), 0, files.size(), parent);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setMinimumDuration(0);
    int index = 0 ;
    BalsamiqOpContext context ;
    foreach(QString inputFile, files) {
        //forma il path di uscita con estensione x.bmml x.mxml
        QFileInfo fileInfo(inputFile);
        QString outFileBaseName = fileInfo.baseName();
        QString finalOutputPath = outputDir ;
        finalOutputPath.append("/");
        finalOutputPath.append(outFileBaseName);
        finalOutputPath.append(".mxml");
        if(!setStep(progressDialog, context, index, tr("Processing: %1").arg(outFileBaseName))) {
            return false;
        }
        if(NULL != _rootControl) {
            delete _rootControl ;
            _rootControl = NULL ;
        }
        if(!doSingleWork(context, inputFile, finalOutputPath, isOverwrite)) {
            Utils::error(tr("Error :%1").arg(_errorReason));
            return false;
        }
        index ++ ;
    }
    Utils::message(tr("Operation done"));
    return true;
}

bool BalsamiqWork::doSingleWork(BalsamiqOpContext &context, const QString &fileIn, const QString &fileOut, const bool isOverwrite)
{
    _isError = false ;
    _outputFilePath = fileOut ;
    if(_outputFilePath.isEmpty()) {
        setError(tr("output file path is invalid"));
        return false ;
    }
    if(fileIn.isEmpty()) {
        setError(tr("input file path is invalid"));
        return false ;
    }
    D(printf("INPUT FILE:%s\n", fileIn.toAscii().data()));
    D(printf("OUTPUT FILE:%s\n", fileOut.toAscii().data()));
    context.fileName = fileIn ;
    if(!translate(context, fileIn, isOverwrite) || _isError) {
        return false;
    }
    return true;
}

bool BalsamiqWork::setStep(QProgressDialog &progressDialog, BalsamiqOpContext &context, const int step, const QString &message)
{
    QCoreApplication::processEvents();
    progressDialog.setValue(step);
    progressDialog.setLabelText(message);
    QCoreApplication::processEvents();
    if(progressDialog.wasCanceled()) {
        context.setError(tr("User Aborted"));
        return false;
    }
    QCoreApplication::processEvents();
    return true;
}
/**
 one pass for global scope, one for details generation
 **/

// first we generate childrens, so the control can find it if he nedds to examine it
bool BalsamiqWork::doEmitData(BalsamiqOpContext &context, BalsamiqProxy *proxy, QDomNode &node)
{
    if(! calculateData(context, proxy, NULL, node)) {
        return false;
    }
    if(! emitGlobalsData(context, proxy, NULL, node)) {
        return false;
    }
    if(! emitControlsData(context, proxy, NULL, node)) {
        return false;
    }
    return true;
}

//genera errore solo se il codice non e' gia riempito' mette i dati del controllo
void BalsamiqWork::setErrorContext(BalsamiqOpContext &context, BalsamiqProxy *proxy, const QString &phase)
{
    QString errorCause = context.errorCause ;
    QString origin ;
    if(NULL != proxy) {
        origin = QString(tr("control type: %1, id:%2").arg(proxy->type()).arg(proxy->id()));
    }
    context.setError(tr("error file:'%1', phase:%2, origin:%3, cause:'%4'").arg(context.fileName).arg(phase).arg(origin).arg(errorCause));
}

void BalsamiqWork::setErrorContextCalculateData(BalsamiqOpContext &context, BalsamiqProxy *proxy)
{
    setErrorContext(context, proxy, tr("calculating data"));
}

void BalsamiqWork::setErrorContextGenerateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy)
{
    setErrorContext(context, proxy, tr("generating global data"));
}

void BalsamiqWork::setErrorContextGenerateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy)
{
    setErrorContext(context, proxy, tr("emitting controls"));
}

bool BalsamiqWork::calculateData(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node)
{
    BalsamiqControl *control = getControl(proxy->type());
    proxy->houseWork(parent);
    if(!control->calculateData(context, proxy, parent, node, false)) {
        setErrorContextCalculateData(context, proxy);
        return false;
    }
    foreach(BalsamiqProxy * child, proxy->children()) {
        if(!calculateData(context, child, proxy, proxy->node)) {
            setErrorContextCalculateData(context, proxy);
            return false;
        }
    }
    if(!control->calculateData(context, proxy, parent, node, true)) {
        setErrorContextCalculateData(context, proxy);
        return false;
    }
    return true;
}

bool BalsamiqWork::emitGlobalsData(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node)
{
    BalsamiqControl *control = getControl(proxy->type());
    if(!control->generateGlobals(context, proxy, parent, node, false)) {
        setErrorContextGenerateGlobals(context, proxy);
        return false;
    }
    foreach(BalsamiqProxy * child, proxy->children()) {
        if(!emitGlobalsData(context, child, proxy, proxy->node)) {
            setErrorContextGenerateGlobals(context, proxy);
            return false;
        }
    }
    if(!control->generateGlobals(context, proxy, parent, node, true)) {
        setErrorContextGenerateGlobals(context, proxy);
        return false;
    }
    return true;
}

bool BalsamiqWork::emitControlsData(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node)
{
    BalsamiqControl *control = getControl(proxy->type());
    if(!control->generateControls(context, proxy, parent, node, false)) {
        setErrorContextGenerateControls(context, proxy);
        return false;
    }
    foreach(BalsamiqProxy * child, proxy->children()) {
        if(!emitControlsData(context, child, proxy, proxy->node)) {
            setErrorContextGenerateControls(context, proxy);
            return false;
        }
    }
    if(!control->generateControls(context, proxy, parent, node, true)) {
        setErrorContextGenerateControls(context, proxy);
        return false;
    }
    return true;
}

