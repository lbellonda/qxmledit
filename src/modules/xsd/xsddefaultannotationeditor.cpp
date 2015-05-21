/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#include "xsddefaultannotationeditor.h"
#include "xsdsinglecommentdialog.h"
#include "xsdfullannotationsdialog.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "utils.h"

XSDDefaultAnnotationEditor::XSDDefaultAnnotationEditor(QWidget *window)
{
    _model = new XSDAnnotationModel;
    _hasResult = false ;
    _window = window ;
    _params = NULL ;
    _origAnnot = NULL ;
}

XSDDefaultAnnotationEditor::~XSDDefaultAnnotationEditor()
{
    resetModel();
}

void XSDDefaultAnnotationEditor::resetModel()
{
    if(NULL != _model) {
        delete _model ;
        _model = NULL ;
    }
}

bool XSDDefaultAnnotationEditor::hasResult()
{
    return _hasResult ;
}

void XSDDefaultAnnotationEditor::exec(Element *origAnnot, XSDOperationParameters *params)
{
    resetModel();
    _model = new XSDAnnotationModel();
    _params = params ;
    _origAnnot = origAnnot ;
    //build info creating an item if none exists
    buildInfoAndCreateOneIfMissing(origAnnot, params);
    //edit short or extended
    if(hasOnlyOneInfo()) {
        if(editSingleAnnotation() == EditExtended) {
            editExtended();
        }
    } else {
        editExtended();
    }
}

Element *XSDDefaultAnnotationEditor::annotation()
{
    // make a copy of the annotation element
    Element *theAnnotation = NULL ;
    if(NULL != _origAnnot) {
        theAnnotation = new Element(_params->makeNameForXSDObject(IO_XSD_ANNOTATION), "", NULL, NULL);
        _origAnnot->copyTo(*theAnnotation, false);
    } else {
        theAnnotation = new Element(_params->makeNameForXSDObject(IO_XSD_ANNOTATION), "", NULL, NULL);
    }

    // insert the children, using the model
    // refactorize to share the most of code with tests
    _model->makeElementList(theAnnotation, _params);

    return theAnnotation ;
}

XInfoBase* XSDDefaultAnnotationEditor::buildInfoAndCreateOneIfMissing(Element *origAnnot, XSDOperationParameters *params)
{
    if(NULL == origAnnot) {
        return makeDocumentation(NULL);
    } else {
        XInfoBase *result = NULL ;
        XInfoBase *first = NULL ;
        foreach(Element * child, *origAnnot->getChildItems()) {
            if(isAppInfo(child, params)) {
                result = makeAppInfo(child);
                if(NULL == first) {
                    first = result ;
                }
            } else if(isDocumentation(child, params)) {
                result = makeDocumentation(child);
                if(NULL == first) {
                    first = result ;
                }
            } else {
                makeOther(child);
            }
        }
        if(NULL == first) {
            first = makeDocumentation(NULL);
        }
        return first ;
    }
}

bool XSDDefaultAnnotationEditor::isAppInfo(Element *element, XSDOperationParameters *params)
{
    QString expectedTagOnlyOneNamespace = params->makeNameForXSDObject(IO_XSD_TAGAPPINFO);
    if(element->isElement() && (element->tag() == expectedTagOnlyOneNamespace)) {
        return true ;
    }
    return false;
}

bool XSDDefaultAnnotationEditor::isDocumentation(Element *element, XSDOperationParameters *params)
{
    QString expectedTagOnlyOneNamespace = params->makeNameForXSDObject(IO_XSD_DOCUMENTATION);
    if(element->isElement() && (element->tag() == expectedTagOnlyOneNamespace)) {
        return true ;
    }
    return false;
}

XAppInfo *XSDDefaultAnnotationEditor::makeAppInfo(Element *child)
{
    XAppInfo *appInfo = new XAppInfo(NULL, NULL);
    appInfo->setSource(child->getAttributeValue(IO_APPINFO_ATTR_SOURCE));
    appInfo->setContentString(innerContent(child->getStringRepresentationForClipboard()));
    _model->addChild(appInfo);
    return appInfo ;
}

XDocumentation* XSDDefaultAnnotationEditor::makeDocumentation(Element *element)
{
    XDocumentation *documentation = new XDocumentation(NULL, NULL);
    if(NULL != element) {
        documentation->setSource(element->getAttributeValue(IO_DOCUMENTATION_ATTR_SOURCE));
        documentation->setLanguage(element->getAttributeValue(XML_LANGUAGE));
        documentation->setContentString(innerContent(element->getStringRepresentationForClipboard()));
    }
    _model->addChild(documentation);
    return documentation;
}

void XSDDefaultAnnotationEditor::makeOther(Element *child)
{
    XSchemaOther *other = new XSchemaOther(NULL, NULL);
    other->setElement(child);
    _model->addChild(other);
}

/**
 * @brief XSDDefaultAnnotationEditor::innerContent
 * @param inputString
 * @param startTag
 * @return
 */
QString XSDDefaultAnnotationEditor::innerContent(const QString &inputString)
{
    int indexOfEnd = inputString.indexOf(">");
    if(indexOfEnd > 0) {
        if(inputString.at(indexOfEnd - 1) == '/') {
            return "";
        }
        // else
        int lastOpenPar = inputString.lastIndexOf("<");
        if(lastOpenPar >= 0) {
            return inputString.mid(indexOfEnd + 1, lastOpenPar - indexOfEnd - 1);
        }

    }
    // impossible to be here
    return inputString ;
}

bool XSDDefaultAnnotationEditor::hasOnlyOneInfo()
{
    return _model->hasOnlyOneInfo();
}

XSDDefaultAnnotationEditor::EditMode XSDDefaultAnnotationEditor::editSingleAnnotation()
{
    XInfoBase *firstAnnotation = _model->firstAnnotation();
    XSDSingleCommentDialog *editSingle = singleEditDialog(firstAnnotation);
    if(editSingle->runExec() == QDialog::Accepted) {
        Utils::TODO_THIS_RELEASE("this is not fully correct: it is ok and edit extended, but the data can be unchanged");
        _hasResult = true ;
        _model->replaceOrAddChild(firstAnnotation, editSingle->createModel());
        if(editSingle->isCallExtended()) {
            return EditExtended ;
        } else {
            return End ;
        }
    } else {
        return End ;
    }
    delete editSingle;
}

void XSDDefaultAnnotationEditor::editExtended()
{
    XSDFullAnnotationsDialog *dlg = fullEditDialog(_model);
    if(dlg->runExec() == QDialog::Accepted) {
        resetModel();
        _model = dlg->data();
        _hasResult = true ;
    } else {
        // a cancel here is a stop word
        _hasResult = false;
    }
    delete dlg ;
}

XSDFullAnnotationsDialog *XSDDefaultAnnotationEditor::fullEditDialog(XSDAnnotationModel *newModel)
{
    return new XSDFullAnnotationsDialog(newModel, _window);
}

XSDSingleCommentDialog *XSDDefaultAnnotationEditor::singleEditDialog(XInfoBase *model)
{
    return new XSDSingleCommentDialog(true, model, _window);
}

