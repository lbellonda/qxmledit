/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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
#include <QScopedPointer>
#include "xmlutils.h"
#include "utils.h"

XSDDefaultAnnotationEditor::XSDDefaultAnnotationEditor(QWidget *window)
{
    _model = new XSDAnnotationModel;
    _hasResult = false ;
    _window = window ;
    _params = NULL ;
    _origAnnot = NULL ;
    _origAnnotSchema = NULL ;
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

void XSDDefaultAnnotationEditor::exec(Element *origAnnot, XSDOperationParameters * params)
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

void XSDDefaultAnnotationEditor::exec(XSchemaAnnotation *origAnnot)
{
    resetModel();
    _model = new XSDAnnotationModel();
    _params = NULL ;
    _origAnnotSchema = origAnnot ;
    //build info creating an item if none exists
    buildInfoAndCreateOneIfMissing(origAnnot);
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

XSchemaAnnotation *XSDDefaultAnnotationEditor::annotationAsSchema()
{
    // make a copy of the annotation element
    XSchemaAnnotation *theAnnotation = new XSchemaAnnotation(NULL, NULL) ;
    // insert the children, using the model
    // refactorize to share the most of code with tests
    _model->makeElementList(theAnnotation);

    return theAnnotation ;
}

XInfoBase* XSDDefaultAnnotationEditor::buildInfoAndCreateOneIfMissing(XSchemaAnnotation *origAnnot)
{
    if(NULL == origAnnot) {
        return makeDocumentation(NULL);
    } else {
        XInfoBase *result = NULL ;
        XInfoBase *first = NULL ;
        foreach(XInfoBase * info, origAnnot->infos()) {
            result = info->clone();
            _model->addChild(result);
            if(NULL == first) {
                first = result ;
            }
        }
        if(NULL == first) {
            first = makeDocumentation(NULL);
            _model->addChild(result);
        }
        return first ;
    }
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
    appInfo->setContentString(XmlUtils::innerContent(child->getStringRepresentationForClipboard()));
    _model->addChild(appInfo);
    return appInfo ;
}

XDocumentation* XSDDefaultAnnotationEditor::makeDocumentation(Element *element)
{
    XDocumentation *documentation = new XDocumentation(NULL, NULL);
    if(NULL != element) {
        documentation->setSource(element->getAttributeValue(IO_DOCUMENTATION_ATTR_SOURCE));
        documentation->setLanguage(element->getAttributeValue(XML_LANGUAGE));
        documentation->setContentString(XmlUtils::innerContent(element->getStringRepresentationForClipboard()));
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

bool XSDDefaultAnnotationEditor::hasOnlyOneInfo()
{
    return _model->hasOnlyOneInfo();
}

XSDDefaultAnnotationEditor::EditMode XSDDefaultAnnotationEditor::editSingleAnnotation()
{
    XInfoBase *firstAnnotation = _model->firstAnnotation();
    QScopedPointer<XSDSingleCommentDialog> editSingle(singleEditDialog(firstAnnotation));
    Utils::TEST_ME("");
    Utils::TODO_THIS_RELEASE("");
    if(editSingle->runExec() == QDialog::Accepted) {
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

