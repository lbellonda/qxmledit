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


#ifndef XSDDEFAULTANNOTATIONEDITOR_H
#define XSDDEFAULTANNOTATIONEDITOR_H

#include "modules/xsd/xsdhelper.h"
#include "modules/xsd/xsdannotationmodel.h"

class XSDFullAnnotationsDialog;
class XSDSingleCommentDialog;

class XSDDefaultAnnotationEditor : public XSDAnnotationEditor
{
protected:
    bool _hasResult;
    QWidget *_window;
    XSDAnnotationModel *_model;
    XSDOperationParameters *_params;
    Element *_origAnnot;
    XSchemaAnnotation *_origAnnotSchema;

    enum EditMode {
        End,
        EditExtended,
        EditSimple
    };

    void resetModel();
    XInfoBase* buildInfoAndCreateOneIfMissing(Element *origAnnot, XSDOperationParameters *params);
    XInfoBase* buildInfoAndCreateOneIfMissing(XSchemaAnnotation *origAnnot);
    bool hasOnlyOneInfo();
    EditMode editSingleAnnotation();
    void editExtended();

    XAppInfo *makeAppInfo(Element *child);
    XDocumentation *makeDocumentation(Element *element);
    void makeOther(Element *child);

    bool isAppInfo(Element *element, XSDOperationParameters *params);
    bool isDocumentation(Element *element, XSDOperationParameters *params);

    virtual XSDFullAnnotationsDialog *fullEditDialog(XSDAnnotationModel *newModel);
    virtual XSDSingleCommentDialog *singleEditDialog(XInfoBase *model);

public:
    XSDDefaultAnnotationEditor(QWidget *window);
    virtual ~XSDDefaultAnnotationEditor();

    bool hasResult() ;
    void exec(Element *origAnnot, XSDOperationParameters *params);
    void exec(XSchemaAnnotation *origAnnot);
    virtual Element *annotation() ;
    virtual XSchemaAnnotation *annotationAsSchema();
};

#endif // XSDDEFAULTANNOTATIONEDITOR_H
