/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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

#include "xsdcompare.h"
#include "utils.h"
#include "config.h"
#include "xsdeditor/xsdwindow.h"
#include "xsdeditor/xsdcomparedata.h"
#include "qxmleditdata.h"

bool isCompareAnnotationsPersistentOption()
{
    bool state = Config::getBool(Config::KEY_XSD_COMPARE_ANNOTATIONS, false) ;
    return state;
}

void setCompareAnnotationsPersistentOption(bool value)
{
    Config::saveBool(Config::KEY_XSD_COMPARE_ANNOTATIONS, value) ;
}

//---------------------------------------------------------------------------------

XSDCompareResult::XSDCompareResult()
{
    _error = false;
    _different = false;
    _schema = NULL ;
}

XSDCompareResult::~XSDCompareResult()
{

}

bool XSDCompareResult::isError()
{
    return _error ;
}

void XSDCompareResult::setError(const bool value)
{
    _error = value ;
}

void XSDCompareResult::setDifferent(const bool value)
{
    _different = value ;
}

bool XSDCompareResult::areDifferent()
{
    return _different;
}

//---------------------------------------------------------------------------------

XSDCompareOptions::XSDCompareOptions()
{
    _compareComment = false ;
}

XSDCompareOptions::~XSDCompareOptions()
{
}

bool XSDCompareOptions::isCompareComment()
{
    return _compareComment ;
}


void XSDCompareOptions::setCompareComment(const bool value)
{
    _compareComment = value ;
}

//---------------------------------------------------------------------------------

XSDCompare::XSDCompare()
{
    _window = NULL ;
}

XSDCompare::~XSDCompare()
{
}

bool XSDCompare::compare(QWidget *parent, QXmlEditData *appData, Regola *data, const QString &folderPath)
{
    _regola = data ;
    _targetXSDFileName = getTargetFile(parent, folderPath);
    if(_targetXSDFileName.isEmpty()) {
        return false;
    }

    QString xmlAsString = data->getAsText();

    try {
        XSDCompareOptions compareOptions;
        compareOptions.setCompareComment(isCompareAnnotationsPersistentOption());

        XSDCompareResult *result = innerCompare(xmlAsString, _targetXSDFileName, compareOptions, false);
        if((NULL == result) || result->isError()) {
            Utils::error(parent, tr("An error occurred comparing data"));
            delete result;
            return false ;
        }
        if(!result->areDifferent()) {
            Utils::message(tr("The schema are identical."));
        }
        // the ownership of the schema is moved to the window
        XSDSchema *schema = result->_schema ;
        delete result;

        QEventLoop eventLoop;
        XSDWindow xsdEditor(appData, parent) ;
        xsdEditor.setContextType(XsdGraphicContext::CONTEXT_DIFF);
        setXSDWindow(&xsdEditor);
        xsdEditor.setController(this);

        // the ownership of the schema is moved to the window
        setUIData(&xsdEditor, schema, _regola->fileName(), _targetXSDFileName, false);

        xsdEditor.EVENTLOOP = &eventLoop ;

        xsdEditor.setWindowModality(Qt::ApplicationModal); // changed to avoid problems on NextStep UI
        xsdEditor.show();
        xsdEditor.setFileName(data->fileName());
        xsdEditor.setTitle(data->fileName());

        if(eventLoop.exec() > 0) {
            return true ;
        }
        return false;
    } catch(XsdException *ex) {
        Utils::error(tr("Error in XSD Plugin. \n%1").arg(ex->cause()));
    } catch(...) {
        Utils::error(tr("Unknown exception."));
    }
    return false;
}


/**
 * @brief XSDCompare::getTargetFile
 * get the target XSD to use in the compare operation
 * @param parent
 * @return the path to the target file
 */
QString XSDCompare::getTargetFile(QWidget *parent, const QString &folderPath)
{
    QString filePath = QFileDialog::getOpenFileName(
                           parent, tr("Open File"),
                           QXmlEditData::sysFilePathForOperation(folderPath),
                           tr("XML Schema files (*.xsd);;All files (*);;")
                       );
    return filePath;
}


void XSDCompare::setXSDWindow(XSDWindow *newWindow)
{
    _window = newWindow;
}

XSDWindow *XSDCompare::getXSDWindow()
{
    return _window ;
}

void XSDCompare::evaluate(XSDWindow *window, XSDCompareOptions &options, const bool isSwap)
{
    QString xmlAsString = _regola->getAsText();
    XSDCompareResult *result = innerCompare(xmlAsString, _targetXSDFileName, options, isSwap);
    if((NULL == result) || result->isError()) {
        Utils::error(tr("An error occurred comparing data"));
        delete result;
        return  ;
    }
    if(!result->areDifferent()) {
        Utils::message(tr("The schema are identical."));
    }
    // the ownership of the schema is moved to the window
    XSDSchema *schema = result->_schema ;
    delete result;

    // the ownership of the schema is moved to the window
    setUIData(window, schema, _regola->fileName(), _targetXSDFileName, isSwap);
}

void XSDCompare::setSummary(XSDWindow *window, XSDSchema *schema)
{
    QList<XSchemaObject*> added;
    QList<XSchemaObject*> modified;
    QList<XSchemaObject*> deleted;
    schema->getModifiedObjects(added, modified, deleted);
    window->setXSDCompareSummary(added, modified, deleted);
}

void XSDCompare::setUIData(XSDWindow *window, XSDSchema *schema, const QString &referencePath, const QString &targetPath, const bool isSwap)
{
    window->setSchema(schema);
    if(isSwap) {
        window->setXSDCompareReferencePath(targetPath);
        window->setXSDCompareTargetPath(referencePath);
    } else {
        window->setXSDCompareReferencePath(referencePath);
        window->setXSDCompareTargetPath(targetPath);
    }
    setSummary(window, schema);
}

XSDCompareResult *XSDCompare::innerCompare(const QString &referenceString, const QString &targetFileName, XSDCompareOptions &options, const bool isSwap)
{
    XSDCompareResult *result = new XSDCompareResult();
    if(NULL == result) {
        return result;
    }
    XSDSchema *reference =  loadXSDFromString(referenceString);
    XSDSchema *target =  loadXSDFromFile(targetFileName);
    if((NULL == reference) || (NULL == target)) {
        if(NULL != reference) {
            delete reference ;
        }
        if(NULL != target) {
            delete target ;
        }
        result->setError(true);
        return result ;
    }
    if(isSwap) {
        XSDSchema *temp =  reference ;
        reference = target  ;
        target = temp ;
    }
    result->_schema = reference;
    // load reference
    compareSchema(result, reference, target, options);

    delete target;
    if(result->isError()) {
        return result;
    }
    if(result->areDifferent()) {
        Utils::message(tr("Schema are different."));
    }
    // the ownership of the schema is moved to the window
    return result;
}


XSDSchema *XSDCompare::loadXSDFromString(const QString &dataToLoad)
{
    try {
        XSDSchema *schema = new XSDSchema(NULL);
        if(NULL != schema) {
            XSDLoadContext loadContext;
            schema->readFromString(&loadContext, dataToLoad, false, NULL, NULL);
            return schema;
        } else {
            Utils::error(tr("No root item"));
        }
    } catch(XsdException *ex) {
        Utils::error(tr("Error loading schema.\n%1").arg(ex->cause()));
    } catch(...) {
        Utils::error(tr("Unknown exception."));
    }
    return NULL;
}


XSDSchema *XSDCompare::loadXSDFromFile(const QString &fileName)
{
    try {
        XSDSchema *schema = new XSDSchema(NULL);
        if(NULL != schema) {
            XSDLoadContext loadContext;
            if(schema->read(&loadContext, fileName)) {
                return schema;
            } else {
                Utils::error(tr("Error loading file"));
                delete schema ;
            }
        } else {
            Utils::error(tr("No root item"));
        }
    } catch(XsdException *ex) {
        Utils::error(tr("Error loading schema.\n%1").arg(ex->cause()));
    } catch(...) {
        Utils::error(tr("Unknown exception."));
    }
    return NULL;
}




XSDCompareResult *XSDCompare::compareSchema(XSDCompareResult *result, XSDSchema *reference, XSDSchema *target, XSDCompareOptions &compareOptions)
{
    /**
     * @brief top level compare: the elements are unordered
     */
    return reference->compare(result, target, compareOptions);
}


//------------------------------------------------------------------------------------------------------

