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

#ifndef XSCHEMAOUTLINEELEMENT_H
#define XSCHEMAOUTLINEELEMENT_H

#include "xsdeditor/xschema.h"

class XSchemaOutlineElement : public XSchemaObject
{
    Q_OBJECT
    /*Q_ENUMS(ElementCategory)
    Q_PROPERTY(XEnums::XBool abstract READ abstract WRITE setAbstract NOTIFY abstractChanged)
    Q_PROPERTY(XEnums::XBool nillable READ nillable WRITE setNillable NOTIFY nillableChanged)
    Q_PROPERTY(XEnums::XBool mixed READ mixed WRITE setMixed NOTIFY mixedChanged)
    Q_PROPERTY(QString fixed READ fixed WRITE setFixed NOTIFY fixedChanged)
    Q_PROPERTY(QString defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
    Q_PROPERTY(QString xsdType READ xsdType WRITE setXsdType NOTIFY xsdTypeChanged)
    Q_PROPERTY(XSchemaElement::ElementCategory category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(QString ref READ ref WRITE setRef NOTIFY refChanged)*/


public:

    /*enum ElementCategory {
        EES_EMPTY = 1,      // tag only
        EES_REFERENCE = 2,      // true elements only can be references
        EES_SIMPLETYPE_ONLY = 3, //child text simpletype
        EES_SIMPLETYPE_WITHATTRIBUTES = 4, // child text complexType simpleContent
        EES_COMPLEX_DERIVED = 5, // complexType -  complexContent
        EES_COMPLEX_DEFINITION = 6 // Complex Type sequence/choice/group/all
    };*/

    friend class TestXSDDiff;
private:

    // working mode
    /*bool _isTypeOrElement;
    ElementCategory _elementCategory;

    DECL_XSD_ATTR(XEnums::XBool, _abstract);
    DECL_XSD_ATTR(QList<EFinalType>, _block);
    DECL_XSD_ATTR(QString, _defaultValue);
    DECL_XSD_ATTR(QString, _fixed);
    QList<EFinalType> _final;
    DECL_XSD_ATTR(EQualified, _form) ;
    XOccurrence _maxOccurs;
    XOccurrence _minOccurs;
    DECL_XSD_ATTR(XEnums::XBool, _nillable);
    DECL_XSD_ATTR(QString, _ref);
    DECL_XSD_ATTR(QString, _xsdType);
    XSchemaAnnotation *_innerAnnotation;*/
    /**
     * @brief _isPredefinite: xsd predefined type
     */
    /*bool _isPredefined;

    //----- types specific
    XEnums::XBool   _mixed;
    QString _containedId;
    QMap<QString, QString> _innerTypeAttributes;

    QList<XSchemaObject*> _attributes;*/
    XSchemaElement *_element;
protected:
    virtual void reset();
    /*virtual void resetAttributes();*/
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    /*virtual bool findSchemaChildComponents(XValidationContext *context, XElementContent *content);
    bool elabTypeForCollect(XValidationContext * context, XElementContent * content);
    void copyAttributesListInGeneric(QList<XSchemaObject*> &list);
    void restoreAttributesListInGeneric(QList<XSchemaObject*> &list);*/
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);
    /*void validateAfterReadType(XSDLoadContext *loadContext, QDomElement &node, void *context);
    void validateAfterReadElement(XSDLoadContext *loadContext, QDomElement &node, void *context);*/
    void calculateElement();

public:

    XSchemaOutlineElement(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaOutlineElement();

    virtual QString tagName();
    /*virtual QString nameOrReference();
    //TODO: a seconda della categoria?*/
    virtual ESchemaType getType()
    {
        return SchemaTypeOutlineElement ;
    }
    /*virtual void reparent(XSchemaObject *newParent);

    virtual void loadFromDom(XSDLoadContext *loadContext, QDomElement &element);
    virtual QString description();
    bool isTypeOrElement();

    virtual XReferenceType referencedObjectType();
    virtual QString referencedObjectName();
    virtual bool isSimpleType() ;
    bool isComplexType();

    QList<XSchemaObject*> &attributes()
    {
        return _attributes;
    }

    virtual int minOccurrences() ;
    virtual int maxOccurrences() ;

    bool isExtension();
    bool isRestriction();
    bool isSimpleExtension();
    bool isSimpleRestiction();

    virtual XSDCompareResult *compare(XSDCompareResult *result, XSchemaObject *target, XSDCompareOptions &options);

    virtual bool findBaseObjects(XSchemaInquiryContext &context, QList<XSchemaObject*> &baseElements, QList<XSchemaObject*> & baseAttributes);

    //--------------------------------------
    DECL_QPROP(XEnums::XBool, abstract, setAbstract)
    DECL_QPROPS(fixed, setFixed)
    DECL_QPROP(XEnums::XBool, nillable, setNillable)
    DECL_QPROP(XEnums::XBool, mixed, setMixed)
    DECL_QPROPS(defaultValue, setDefaultValue)
    DECL_QPROPS(ref, setRef)
    DECL_QPROPS(xsdType, setXsdType)
    DECL_QPROP(XSchemaElement::ElementCategory, category, setCategory)
    //---------------------------------------

    virtual XSchemaAttribute* addAttribute();
    virtual XSchemaChoice* addChoice();
    virtual XSchemaElement* addElement();
    virtual XSchemaSequence* addSequence();
    virtual XSchemaObject* addRestriction();

    D_ADD_A()
    D_ADD_S()
    D_ADD_C()
    bool canAddRestriction();

    QString simpleComplexDescr();
    QString nameDescr();
    QString occurrencesDescr();

    virtual bool hasAReference();
    virtual bool examineType(XValidationContext *context, XSingleElementContent *parent);
    virtual bool collect(XValidationContext *context, XSingleElementContent *content);

signals:
    void abstractChanged(const XEnums::XBool newValue);
    void nillableChanged(const XEnums::XBool newValue);
    void fixedChanged(const XEnums::XBool newValue);
    void mixedChanged(const XEnums::XBool newValue);
    void defaultValueChanged(const XEnums::XBool newValue);

    void maxOccursChanged(const int);
    void minOccursChanged(const int);
    void xsdTypeChanged(const QString & xsd);
    void refChanged(const QString & xsd);
    void categoryChanged(const XSchemaElement::ElementCategory);*/

protected:
    // IO handling
    /*void writeAppendElementAttributes(QDomDocument &document, QDomElement &node);
    void writeHandleSimpleType(QDomDocument &document, QDomElement &typeSpecifier);
    void writeHandleComplexType(QDomDocument &document, QDomElement &typeSpecifier);
    void writeAppendAttributes(QDomDocument &document, QDomElement &node);
    void writeAppendNotAttributes(QDomDocument &document, QDomElement &node);

    bool readHandleSimpleType(XSDLoadContext *loadContext, QDomElement &element, const bool isTopLevel);
    bool readHandleComplexType(XSDLoadContext *loadContext, QDomElement &element, const bool isTopLevel);
    void readHandleAnnotation(XSDLoadContext *loadContext, QDomElement &element, const bool isInner);
    XSchemaSimpleContent *readSimpleContentChild(XSDLoadContext *loadContext, QDomElement &element);
    XSchemaComplexContent *readComplexContentChild(XSDLoadContext *loadContext, QDomElement &element);

    void validateComplexType(QDomElement &element, const bool isInner);
    void collectElementsOfComplexDerived(XSchemaInquiryContext &context, QList<XSchemaObject*> &result);
    bool collectElements(QList<XSchemaObject*> &result);
    bool collectAllBaseTypeElements(XSchemaInquiryContext &context, QList<XSchemaObject*> &result, QList<XSchemaObject*> &attributes);
    bool collectAllElementsOfBaseTypes(XSchemaInquiryContext &context, QList<XSchemaObject*> &result);
    //---
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);
    void raiseErrorTypesME(XSDLoadContext *loadContext, QDomElement &node);*/

public:
    /*virtual void markCompareStateRecursive(const XSDCompareState::EXSDCompareState newState);
    void getModifiedObjects(QList<XSchemaObject*> &added, QList<XSchemaObject*> &modified, QList<XSchemaObject*> &deleted);
    QString xmlRepresentation();
    XSchemaAttributesCollection *attributesRepresentation();
    XSchemaElement *resolveType(XSchemaElement *base);
    XSchemaElement *resolveReference(XSchemaElement *base);
    XSchemaElement *resolveType(XSchemaFindReferenceContext &context, XSchemaElement *base);
    XSchemaElement *resolveReference(XSchemaFindReferenceContext &context, XSchemaElement *base);
    bool collectAttributes(XSchemaAttributesCollection &attributesCollection);
    void collectAttributesCollection(XSchemaAttributesCollection &attributesCollection);
    void collectAttributeOfComplexDerived(XSchemaAttributesCollection & attributesCollection);
    void collectAttributeOfSimpleDerived(XSchemaAttributesCollection & attributesCollection);
    bool collectAllElements(QList<XSchemaObject*> &result);

    XSchemaElement *getReferencedElement();
    XSchemaElement *getReferencedType();
    XSchemaComplexContentExtension *getExtension();
    XSchemaComplexContentRestriction *getRestriction();
    XSchemaSimpleContentExtension *getSimpleContentExtension();
    XSchemaSimpleContentRestriction *getSimpleContentRestriction();
    XSchemaSimpleTypeRestriction *getSimpleTypeRestriction();
    bool isPredefined() const;
    void setIsPredefined(bool value);*/
    XSchemaElement *getElement() const;
    void setElement(XSchemaElement *element);
};

#endif // XSCHEMAOUTLINEELEMENT_H
