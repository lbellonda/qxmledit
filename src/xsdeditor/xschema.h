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
#ifndef XSCHEMA_H
#define XSCHEMA_H

#define XSD_READONLY

#include "xsdeditor/xsdtypes.h"
#include "xsdeditor/xproperties.h"
#include "xsdeditor/xsdcomparedata.h"
#include "xsdeditor/xsdcompare.h"
#include "xsdeditor/xschemaoperationcontext.h"
#include <QMap>
#include <QMultiHash>
#include <QStringList>
#include <QPixmap>
#include <QNetworkAccessManager>

// macro to define an attrbute
#define DECL_XSD_ATTR(type, name) type name; bool name##_used
#define SET_XSD_ATTR(name, value) do{name = value; name##_used = true ;}while(false)
#define INIT_XSD_ATTR(name, value) do{name = value; name##_used = false;}while(false)
#define INIT_XSD_ATTRD(name) do{name##_used = false;}while(false)

#include "xsdeditor/validator/xvalidationcontext.h"

#include "xsdloadcontext.h"

#define DECLARE_PROP_S_ALL(getter, setter)  private:\
    QString _##getter;\
    public:\
    QString getter(); void setter( const QString &newValue );

#define DECLARE_PROP_S(getter, setter)  QString getter(); void setter( const QString &newValue );


#define DECLARE_PROP(className, prop, type)  type prop(); void set##prop( const type &prop );
#define IMPL_PROP(className, prop, type) type className::prop(){ return m_##prop;} void className::set##prop( const type &new#prop ) { m_##prop = new#prop ; }
#define IMPL_PROPS(className, getter, setter) QString className::getter(){ return _##getter;} void className::setter( const QString &newValue ) { _##getter = newValue ; }
//#define IS_TYPE(xtype)  int type() const { return xtype; }

#define ADD_E(x)   virtual bool canAddElement() { return x ;}
#define ADD_A(x)   virtual bool canAddAttribute() { return x ;}
#define ADD_S(x)   virtual bool canAddSequence()  { return x ;}
#define ADD_C(x)   virtual bool canAddChoice()  { return x ;}

#define D_ADD_E()   virtual bool canAddElement() ;
#define D_ADD_A()   virtual bool canAddAttribute() ;
#define D_ADD_S()   virtual bool canAddSequence() ;
#define D_ADD_C()   virtual bool canAddChoice() ;

class XSDCompareObject
{
public:
    enum EXSDCompareObject {
        XSDOBJECT_UNCHANGED,
        XSDOBJECT_MODIFIED,
        XSDOBJECT_DIFFERENT // this is a mark
    };
};

class XSDCompareState
{
public:
    enum EXSDCompareState {
        COMPARE_NOT_EXAMINED,
        COMPARE_UNCHANGED,
        COMPARE_MODIFIED,
        COMPARE_ADDED,
        COMPARE_DELETED
    };
};

class XEnums : public QObject
{
    Q_OBJECT
private:
    XEnums();
    ~XEnums();
public:
    enum XBool { XBOOL_UNSET, XBOOL_TRUE, XBOOL_FALSE} ;
    static XBool parseBool(const QString input);
    static QString boolToString(const XBool value);

    // other general purpose enums

};

#include <QObject>
#include <QDomDocument>
#include <exception>

class XSchemaAttribute;
class XSchemaAttributeGroup;
class XSchemaComplexContentExtension;
class XSchemaComplexContentRestriction;
class XSchemaSimpleContentExtension ;
class XSchemaSimpleContentRestriction ;
class XSchemaSimpleType;

class AttrCollectInfo
{
public:
    XSchemaAttribute* attribute;
    XSchemaAttributeGroup* attributeGroup;
    XSchemaAttributeGroup* originalAttributeGroup;
    XSchemaAttribute* originalAttribute;
    QStringList enums;
    QString type;
    bool isReference;
    bool isTypeRestriction;
    bool isTypeExtension;
    bool isSimpleUnion;
    bool isSimpleList;
    QString listValue;
    QString unionValue;
    QString defaultValue;
    bool isGroup();
    AttrCollectInfo();
    ~AttrCollectInfo();
};


class XSchemaAttributesCollection
{
public:
    bool collectGroups;
    QMap<QString, AttrCollectInfo*> attributes;
    void insert(const QString &name, XSchemaAttribute* finalAttribute, const QString &parTypeName, QStringList parEnums, const QString &defaultValue, XSchemaAttribute *originalAttribute);
    void insertGroup(const QString &name, XSchemaAttributeGroup* originalAttribute, XSchemaAttributeGroup* finalAttribute);
    XSchemaAttributesCollection();
    ~XSchemaAttributesCollection();
};

class XSchemaXmlRepresentationInfo
{
public:
    QString tag;
    QMap < QString /*name*/, QString /*value*/ > attributes;
    void setAttributes(XSchemaAttributesCollection &attributesCollection);
    QString buildXmlRepresentation();
};



class XsdException : public std::exception
{
public:
    QString _cause;
    XsdException(const QString &theCause);
    virtual const char* what() const throw() ;
    virtual QString cause() const throw() ;
    virtual ~XsdException() throw() ;
};

void XsdError(const QString &message);
class XSchemaAnnotation;

class XSDFacet
{
public:
    enum EType {
        MinExclusive,
        MinInclusive,
        MaxExclusive,
        MaxInclusive,
        TotalDigits,
        FractionDigits,
        Length,
        MinLength,
        MaxLength,
        Enumeration,
        WhiteSpace,
        Pattern
    };
private:
    QString _id;
    bool _idPresent;
    EType   _type;
    QString _value ;
    XEnums::XBool _fixed;
    XSchemaAnnotation *_annotation;
    QMap<QString, QString> _otherAttributes;
public:
    XSDFacet(const EType type, const QString &value);
    XSDFacet(const QString &tagName, const QString &value);
    ~XSDFacet();

    QString typeString();
    QString value();
    void setValue(const QString &newValue);
    QString annotationString();
    QMap<QString, QString> &otherAttributes();

    XSDFacet *clone();
    bool compareTo(const XSDFacet *other);
    QString toString();
    XSchemaAnnotation *annotation() const;
    void setAnnotation(XSchemaAnnotation *annotation);
    EType type() const;
    void setType(const EType type);
    bool hasFixed();
    XEnums::XBool fixed();
    void setFixed(const XEnums::XBool newFixed);
    void setFixedString(const QString &value);

    //---
    static QSet<QString> allTags();
    //--
    QString id() const;
    void setId(const QString &id);
    bool idPresent() const;
    void setIdPresent(bool value);
};

class InfoFacet;

class RestrictionFacets
{
    bool _isExaminedDiff ;

    InfoFacet* getFacetAsInfo(const QString &name, const QString &value);
    void addFacet(QList<InfoFacet *> *list, const QString &name, const QString &value);
public:
    // TODO: WRONG!!!! THESE ARE ELEMENTS WITH INNER ANNOTATIONS
    QString _minExclusive, _minInclusive ;
    QString _maxExclusive, _maxInclusive ;
    QString _totalDigits, _fractionDigits, _length;
    QString _minLength, _maxLength ;
    QStringList _enumeration ;
    QString _whiteSpace; // TODO: WRONG!!!!
    QString _pattern; //e' una lista, non uno solo
    //---- compare only -------------
    QStringList _enumerationAdded ;
    QStringList _enumerationDeleted ;
    QStringList _enumerationEquals ;
    //--------------------------------

    RestrictionFacets();
    QString toString();
    QString toStringDiffHtml();
    //void generateFacets( QDomElement &node );
    bool scanForFacets(const QString &name, const QString &value);
    QString getFacet(const QString &name, const QString &value);
    void addFacetIfNotEmpty(QDomElement &element, const QString &elementTag, const QString &value);
    XSDCompareObject::EXSDCompareObject compareTo(RestrictionFacets *other);
    void addEnumsToList(QStringList &lst);
    void otherFacets(QList<InfoFacet *> *list);
    void addOtherFacets(QList<InfoFacet *> *list);
};


class XOccurrence
{
public:
    bool isSet;
    unsigned int occurrences;
    bool isUnbounded ;
    int defaultValue ; // -1 : unbounded
    XOccurrence();
    ~XOccurrence();
    XOccurrence(const XOccurrence &ref);
    void addAttrToNode(QDomElement &element, const QString &attributeName);
    bool setValueFromAttribute(const QString &attributeValue);
    QString toString();
    QString getFacet(const QString &name, const QString &value);
    XSDCompareObject::EXSDCompareObject compareTo(XOccurrence *other);
};

class XSDSchema ;

class XSchemaRoot
{

public:
    //TODO put here root methods
    virtual QString xsdNS() = 0 ;
    virtual QDomElement createElementWithNamespace(QDomDocument &document, const QString &tagName) = 0 ;
    virtual QString namespacePrefix() = 0 ;
    virtual QString namespaceURI() = 0 ;
    virtual bool existsNamespace(const QString &namespaceToSearch) = 0;
    virtual XSDSchema* schema() = 0;
};

enum XReferenceType {
    XRT_NONE,
    XRT_ELEMENT,
    XRT_TYPE,
    XRT_ATTRIBUTE
};


enum ESchemaType {
    SchemaTypeSchema,
    SchemaTypeElement,
    SchemaTypeAttribute,
    SchemaTypeChoice,
    SchemaTypeSequence,
    SchemaTypeSimpleContent,
    SchemaTypeSimpleContentRestriction,
    SchemaTypeSimpleContentExtension,
    SchemaTypeComplexContent,
    SchemaTypeComplexContentRestriction,
    SchemaTypeComplexContentExtension,
    SchemaTypeSimpleType,
    SchemaTypeSimpleTypeRestriction,
    SchemaTypeSimpleTypeUnion,
    SchemaTypeSimpleTypeList,

    SchemaTypeAnnotation,
    SchemaTypeBaseDocumentation,
    SchemaTypeDocumentation,
    SchemaTypeAppInfo,
    SchemaTypeAll,
    SchemaTypeAny,
    SchemaTypeAnyAttribute,
    SchemaTypeAttributeGroup,
    SchemaTypeField,
    SchemaTypeGroup,
    SchemaTypeImport,
    SchemaTypeInclude,
    SchemaTypeKey,
    SchemaTypeKeyRef,
    SchemaTypeNotation,
    SchemaTypeRedefine,
    SchemaTypeSelector,
    SchemaTypeUnique,

    //-------
    SchemaContainer,
    //--- as a generic specifier
    SchemaGenericType,
    SchemaGenericElement,
    SchemaOtherElement,
    // Outline
    SchemaTypeOutlineElement,
    SchemaTypeOutlineContainer,
    SchemaTypeOutlineGroup,
    SchemaTypeOutlineSequence,
    SchemaTypeOutlineChoice,
    SchemaTypeOutlineAll,
    SchemaTypeOutlineAny
};


class XSchemaChoice ;
class XSchemaSequence ;
class XSchemaElement ;
class XSchemaAttribute ;
class XSchemaSimpleTypeRestriction;
class XSchemaSimpleTypeList;
class XSchemaSimpleTypeUnion;
class XSchemaAnnotation ;
class XInfoBase;
class XSchemaSimpleContent;
class XSchemaComplexContent;

class XAttribute
{
    QString key;
    QString value;
};

class SchemaSearchContext;

#define TAG(x) virtual QString tagName() { return x ; }


#define CMP_DATA(field,other)    do{if( field != (other)->field ) { return XSDCompareObject::XSDOBJECT_MODIFIED ; }}while(0)
#define CMP_OBJ(field,other)    do{if( XSDCompareObject::XSDOBJECT_MODIFIED == field.compareTo((other)->field ) ) { return XSDCompareObject::XSDOBJECT_MODIFIED ; }}while(0)
#define CMP_OBJP(field,other)    do{if( XSDCompareObject::XSDOBJECT_MODIFIED == field.compareTo( &(other)->field ) ) { return XSDCompareObject::XSDOBJECT_MODIFIED ; }}while(0)
#define CMP_LIST(field,other)    do{if( field.size() != (other)->field.size() ) { return XSDCompareObject::XSDOBJECT_MODIFIED; }\
        for( int i = 0 ; i < field.size() ; i ++ ) {\
            if( field.at(i) != (other)->field.at(i) ) {\
                return XSDCompareObject::XSDOBJECT_MODIFIED ;\
            }\
        }\
    }while(0)

#define CMP_LIST_O(field,other,options)    do{if( field.size() != (other)->field.size() ) { return XSDCompareObject::XSDOBJECT_MODIFIED; }\
        for( int i = 0 ; i < field.size() ; i ++ ) {\
            if( field.at(i)->innerCompareTo((other)->field.at(i), options) == XSDCompareObject::XSDOBJECT_MODIFIED ) {\
                return XSDCompareObject::XSDOBJECT_MODIFIED ;\
            }\
        }\
    }while(0)

class XSchemaObject : public QObject
{
    Q_OBJECT
    Q_ENUMS(EFinalType)

public:
    enum EFinalType {
        FINALTYPE_NONE = 0,
        FINALTYPE_ALL = 1,
        FINALTYPE_RESTRICTION = 2,
        FINALTYPE_EXTENSION = 3,
        FINALTYPE_LIST = 4,
        FINALTYPE_UNION = 5,
        FINALTYPE_SUBSTITUTION = 6
    };

    enum EProcessContent { missing, none, lax, skip, strict };

    enum EQualified {
        EQ_MISSING,
        EQ_UNQUALIFIED,
        EQ_QUALIFIED
    };


protected:
    static int instances;
    int _instanceId;
    QString _id;
    QString _name;
    bool _name_used;
    QList<XSchemaObject*> _children;
    QMap<QString, QString> _otherAttributes;
    XSchemaObject *_parent;
    XSchemaRoot *_root;
    XSchemaAnnotation *_annotation;
    XSDCompareState::EXSDCompareState _compareStatus;
    bool _isRedefinition;
    QString _key;

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    virtual void reset();
    XSchemaObject* addChild(XSchemaObject *child);

public:
    QString name();
    virtual QString nameOrReference();
    void setName(const QString &name);
    QString id();
    void setId(const QString &id_);
    int getUUID();
    QString key();

    XSchemaRoot *root();
    /**
     * @brief tagName
     * @return  the tag literal of the class
     */
    virtual QString tagName() = 0 ;
    QString typeString();

    //virtual bool operator< ( const XSchemaObject & other ) const ;

    XSchemaObject(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaObject();

    virtual void reparent(XSchemaObject *newParent);
    virtual void reparentRoot(XSchemaRoot *newRoot);

    QList<XSchemaObject*> &getChildren();

    XSchemaAnnotation *annotation();

    static QString fromFinalTypeToString(const EFinalType fType);
    static QString finalListToString(QList<EFinalType> &types);
    QList<EFinalType> decodeFinalListToString(const QString &encodedData);

    virtual bool findBaseObjects(XSchemaInquiryContext &context, QList<XSchemaObject*> &baseElements, QList<XSchemaObject*> &baseAttributes);

    QList<XSchemaObject*> topLevelListByName(const ESchemaType expected, const bool onlyThisSchemaOrAll);

    EQualified decodeForm(const QString &value);
    //-------------------- region(compare) --------------------
public:
    virtual XSDCompareObject::EXSDCompareObject compareTo(XSchemaObject* target, XSDCompareOptions &options);
    XSDCompareState::EXSDCompareState compareState();
    virtual void markCompareStateRecursive(const XSDCompareState::EXSDCompareState newState);
    virtual void getModifiedObjects(QList<XSchemaObject*> &added, QList<XSchemaObject*> &modified, QList<XSchemaObject*> &deleted);
    QString occurrencesDescrString(XOccurrence &minOccurs, XOccurrence &maxOccurs);
    static QList<XSchemaObject*> sortObjectsByName(const QList<XSchemaObject*> &objects);

protected:
    void setCompareState(const XSDCompareState::EXSDCompareState newState);

    virtual XSDCompareResult *compare(XSDCompareResult *result, XSchemaObject *target, XSDCompareOptions &options);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) = 0; // abstract
    XSDCompareObject::EXSDCompareObject innerBaseCompareTo(XSchemaObject *target, XSDCompareOptions &options);
    bool baseInnerCompareTo(XSchemaObject *target, XSDCompareOptions &options);
    XSDCompareResult *compareOrdered(XSDCompareResult *result, QList<XSchemaObject*> &referenceList, QList<XSchemaObject*> &targetList, XSDCompareOptions &options);
    void compareDifferentObjects(XSDCompareResult *result, XSDCompareData &data, QList<XSchemaObject*> &targetList, XSDCompareOptions &options);
    void advanceChild(XSDCompareData &data, const XSDCompareState::EXSDCompareState newState);
    QString hashNameOfObject(XSchemaObject* target);
    XSDCompareResult *compareUnordered(XSDCompareResult *result, QList<XSchemaObject*>&referenceList, QList<XSchemaObject*>&targetList, XSDCompareOptions &options);
    void addDeletedTarget(QList<XSchemaObject *> &finalCollection, XSchemaObject *referenceFather, QList<XSchemaObject *> &targetList);
    void insertElementFromOtherCollection(QList<XSchemaObject *> &finalCollection, XSchemaObject *referenceFather);
    void addChildBranch(XSDCompareData &data, const XSDCompareState::EXSDCompareState newState);
    XSDCompareObject::EXSDCompareObject compareAnnotations(XSchemaAnnotation *reference, XSchemaAnnotation *target, XSDCompareOptions &options);
    //-------------------- endregion(compare) --------------------
public:
    virtual ESchemaType getType() = 0;

    virtual bool canAddElement() ;
    virtual bool canAddAttribute() ;
    virtual bool canAddSequence() ;
    virtual bool canAddChoice() ;
    virtual bool canAddRestriction() ;
    virtual bool canAddExtension() ;
    virtual bool canAddUnion() ;
    virtual bool canAddList() ;

    virtual bool isSimpleType() ;

    virtual bool canAddChild(const ESchemaType newType);
    virtual XSchemaObject* addNewChild(const ESchemaType newType);

    virtual XSchemaElement *addElement() ;
    virtual XSchemaAttribute *addAttribute();
    virtual XSchemaChoice* addChoice();
    virtual XSchemaSequence* addSequence();
    virtual XSchemaObject* addRestriction();
    virtual XSchemaObject* addExtension();
    virtual XSchemaSimpleTypeUnion* addUnion();
    virtual XSchemaSimpleTypeList* addList();

    virtual void deleteObject();
    virtual void deleteChild(XSchemaObject *theChild);

    virtual XReferenceType referencedObjectType();
    virtual QString referencedObjectName();
    virtual XSchemaObject *findComponentInValidationPath(SchemaSearchContext *context);
    virtual bool findSchemaChildComponents(XValidationContext *context, XElementContent *content);
    virtual QString descriptionForProposal();

    QString getOhterAttrsAsTextList();
    QStringList getOhterAttrsNames();
    bool hasOtherAttributes();
    bool hasOtherAttribute(const QString &attrName);
    QString getOtherAttribute(const QString &attrName);
    virtual bool isAnnotationElement();

protected:
    virtual XSchemaElement *realAddElement() ;
    virtual XSchemaAttribute *realAddAttribute();
    virtual XSchemaChoice* realAddChoice();
    virtual XSchemaSequence* realAddSequence();
    virtual XSchemaSimpleTypeRestriction* realAddSimpleTypeRestriction();
    virtual XSchemaSimpleTypeUnion* realAddUnion();
    virtual XSchemaSimpleTypeList* realAddList();
    bool hasSimpleType();

public: //TODO: protected?
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) = 0 ;
    virtual bool generateInnerDom(QDomDocument &document, QDomNode &parent);
    virtual bool containsSimpleType();
    virtual QString description();
    virtual QColor color();
    virtual QPixmap icon();
    XSchemaObject *xsdParent();
    virtual bool hasAReference();
    virtual bool collect(XValidationContext *context, XSingleElementContent *content);
    virtual int minOccurrences() ;
    virtual int maxOccurrences() ;

    bool addAttributeToCollection(const QString & name, XSchemaAttributesCollection & attributesCollection, XSchemaAttribute * inputAttribute);
    bool addAttributeToCollection(const QString & name, XSchemaAttributesCollection & attributesCollection, XSchemaAttribute * inputAttribute, XSchemaAttribute * originalAttribute);
    bool addAttributeGroupToCollection(XSchemaAttributesCollection & attributesCollection, XSchemaAttributeGroup * inputAttributeGroup);
    XSchemaElement *getReferencedType(const QString &typeName);
    bool hasOtherAttributeHavingValue(const QString &attrName, const QString &attrValue);
    XSchemaObject *findChildWithOtherAttributeHavingValue(const QString &attrName, const QString &attrValue);
    bool isRedefinition() const;
    void setIsRedefinition(bool value);
    virtual void loadFromDom(XSDLoadContext *loadContext, QDomElement & element);

    XSchemaObject* hasChildTypeOf(const ESchemaType expectedType);
    bool hasKey() ;
    bool hasKeyRef() ;
    bool hasUnique() ;

protected:
    static void addAttrNotEmpty(QDomElement element, const QString &attributeName, const QString &value);
    QDomElement createElement(QDomDocument &document, const QString &tagName) ;
    void addOtherAttributesToDom(QDomElement & node);
    bool readOtherAttributes(QDomAttr & attribute);
    bool readOtherAttributes(Attribute *attribute, XSDOperationParameters * params);
    void addElementIfNotEmpty(QDomElement &element, const QString &elementTag, const QString &value);
    void addFacetIfNotEmpty(QDomElement &element, const QString &elementTag, const QString &value);

    void readHandleObject(XSDLoadContext *loadContext, QDomElement &element, XSchemaObject *newObject);
    void readHandleObject(XSDLoadContext *loadContext, QDomElement &element, XSchemaObject *parent, XSchemaObject *newObject);
    bool scanDom(XSDLoadContext *loadContext, QDomElement &elementToExamine, void *context);
    void addObjectToLoadContext(XSDLoadContext *loadContext, QDomElement &elementToExamine);
    QString getLoadKey(QDomNode &elementToExamine);
    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void *context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);

    void readHandleAnnotation(XSDLoadContext * loadContext, QDomElement &element);
    void raiseError(XSDLoadContext *loadContext, XSchemaObject *origin, QDomNode &node, const bool isElement);
    void raiseError(XSDLoadContext * loadContext, XSchemaObject *origin, Element *element, const QString &name, const bool isElement);
    void raiseErrorForObject(XSDLoadContext *loadContext, QDomNode &node);
    void raiseError(XSDLoadContext *loadContext, XSchemaObject *origin, QDomNode &node, const QString &message);
    void raiseError(XSDLoadContext *loadContext, const EXSDLoadError errorCode, XSchemaObject *origin, QDomNode &node, const QString &message);
    void raiseError(XSDLoadContext *loadContext, const EXSDLoadError errorCode, XSchemaObject *origin, Element *element, const QString &message);
    void raiseError(XSDLoadContext *loadContext, XSchemaObject *origin, Element *node, const QString &message);
    void invalidObjectState(XSDLoadContext *loadContext, XSchemaObject *origin, QDomNode &node, const QString &fieldName);
    QStringList readFinalAttribute(const QString &value);
    EProcessContent decodeProcessContents(const QString &value);
    QString processContentstoString(const EProcessContent pc);
    void scanForContentAnnotation(XSDLoadContext *loadContext, QDomElement &element, void * context);
    void generateFacets(QDomElement &node, RestrictionFacets &facets);
    QString localName(const QString &name);
    bool isTopLevel();
    bool isInRedefine();

    static const QString _xsdXML;

signals:
    void childAdded(XSchemaObject *newChild);
    void childRemoved(XSchemaObject *theChild);
    void deleted(XSchemaObject *theObject);
    void nameChanged(const QString &newName);
    void idChanged(const QString &newId);
    void changed();
    void propertyChanged(const QString &propertyName);

};

class XSchemaAttribute : public XSchemaObject
{
    Q_OBJECT

public:
    enum EUse { Missing, Optional, Prohibited, Required };
private:
    DECL_XSD_ATTR(QString, _defaultValue);
    DECL_XSD_ATTR(QString, _fixed);
    DECL_XSD_ATTR(EQualified, _form) ;
    DECL_XSD_ATTR(QString, _ref);
    DECL_XSD_ATTR(QString, _xsdType);
    DECL_XSD_ATTR(EUse, _use);

    friend class TestXSDDiff;
protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void *context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    bool handleReference(XSchemaInquiryContext &context, QList<XSchemaObject*> &result);

    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

    bool readHandleSimpleType(XSDLoadContext *loadContext, QDomElement &element);

public:
    XSchemaAttribute(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaAttribute();

    TAG("attribute")

    virtual QString description();

    virtual ESchemaType getType()
    {
        return SchemaTypeAttribute ;
    }
    static QString toStringUse(const EUse theUse);
    static QString toStringForSave(const EUse theUse);

    virtual bool hasAReference();
    bool hasType();
    XSchemaAttribute* resolveReference();

    virtual XReferenceType referencedObjectType();
    virtual QString referencedObjectName();

    DECL_QPROPS(xsdType, setXsdType)
    DECL_QPROPS(defaultValue, setDefaultValue)
    DECL_QPROPS(fixed, setFixed)
    DECL_QPROP(XSchemaAttribute::EUse, use, setUse)

    bool hasDefaultValue();
    bool isFixed();

    XSchemaSimpleTypeRestriction *getSimpleTypeRestriction();
    virtual bool findBaseObjects(XSchemaInquiryContext &context, QList<XSchemaObject*> &baseChildren, QList<XSchemaObject*> & baseAttributes);

    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);

    //TODO Q_PROPERTY(QString fixed READ fixed WRITE setFixed NOTIFY fixedChanged )
    //TODO Q_PROPERTY(QString defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged )

};

class InfoFacet;

#include "infofacet.h"

class XTypeQueryInfo
{
    enum ETypeInfo {
        Pure,
        Restriction,
        Extension
    };

    ETypeInfo _typeInfo;
    QString _typeName;
    QString _qualifiedTypeName;
    QStringList _enums;
    bool _isComplex ;
    bool _isSimpleTypeUnion;
    bool _isSimpleTypeList;
    QString _listValue;
    QString _unionValue;
    QList<InfoFacet*> _facets;
    //---
public:
    bool isRestriction();
    bool isExtension();
    bool hasEnumOrFacets();
    bool hasEnum();
    void setEnums(const QStringList &newEnums);
    QStringList enums();
    void setComplexRestriction(const QString &newType);
    void setComplexExtension(const QString &newType);
    void setSimpleRestriction(const QString &newType);
    void setSimpleExtension(const QString &newType);
    QString name();
    void setName(const QString &newName);
    XTypeQueryInfo();
    ~XTypeQueryInfo();
    QString qualifiedTypeName() const;
    void setQualifiedTypeName(const QString &qualifiedTypeName);
    bool isSimpleTypeUnion() const;
    void setIsSimpleTypeUnion(bool isSimpleTypeUnion);
    bool isSimpleTypeList() const;
    void setIsSimpleTypeList(bool isSimpleTypeList);
    QString listValue() const;
    void setListValue(const QString &listValue);
    QString unionValue() const;
    void setUnionValue(const QString &unionValue);
    bool hasOtherFacets();
    QList<InfoFacet *> *otherFacets();
};

// Limitations:
// annotations are handled only for main type
class XSchemaElement : public XSchemaObject
{
    Q_OBJECT
    Q_ENUMS(ElementCategory)
    Q_PROPERTY(XEnums::XBool abstract READ abstract WRITE setAbstract NOTIFY abstractChanged)
    Q_PROPERTY(XEnums::XBool nillable READ nillable WRITE setNillable NOTIFY nillableChanged)
    Q_PROPERTY(XEnums::XBool mixed READ mixed WRITE setMixed NOTIFY mixedChanged)
    Q_PROPERTY(QString fixed READ fixed WRITE setFixed NOTIFY fixedChanged)
    Q_PROPERTY(QString defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
    //TODO Q_PROPERTY(XOccurrence maxOccurs READ maxOccurs WRITE setMaxOccurs NOTIFY maxOccursChanged )
    //TODO Q_PROPERTY(XOccurrence minOccurs READ minOccurs WRITE setMinOccurs NOTIFY minOccursChanged )
    //TODO Q_PROPERTY(XSchemaObject::EFinalType final READ final WRITE setFinal NOTIFY finalChanged)
    //TODO Q_PROPERTY(XSchemaObject::EFinalType block READ block WRITE setBlock NOTIFY blockChanged)
    Q_PROPERTY(QString xsdType READ xsdType WRITE setXsdType NOTIFY xsdTypeChanged)
    Q_PROPERTY(XSchemaElement::ElementCategory category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(QString ref READ ref WRITE setRef NOTIFY refChanged)


public:

    enum ElementCategory {
        EES_EMPTY = 1,      // tag only
        EES_REFERENCE = 2,      // true elements only can be references
        EES_SIMPLETYPE_ONLY = 3, //child text simpletype
        EES_SIMPLETYPE_WITHATTRIBUTES = 4, // child text complexType simpleContent
        EES_COMPLEX_DERIVED = 5, // complexType -  complexContent
        EES_COMPLEX_DEFINITION = 6 // Complex Type sequence/choice/group/all
    };

    friend class TestXSDDiff;
private:

    // working mode
    bool _isTypeOrElement;
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
    ///QString _substitutionGroup;TODO
    DECL_XSD_ATTR(QString, _xsdType);
    //EXType _type; TODO: fare funzione calcolata
    XSchemaAnnotation *_innerAnnotation;
    /**
     * @brief _isPredefinite: xsd predefined type
     */
    bool _isPredefined;

    //----- types specific
    XEnums::XBool   _mixed;
    QString _containedId;
    QMap<QString, QString> _innerTypeAttributes;

    QList<XSchemaObject*> _attributes;
protected:
    virtual void reset();
    virtual void resetAttributes();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual bool findSchemaChildComponents(XValidationContext *context, XElementContent *content);
    bool elabTypeForCollect(XValidationContext * context, XElementContent * content);
    void copyAttributesListInGeneric(QList<XSchemaObject*> &list);
    void restoreAttributesListInGeneric(QList<XSchemaObject*> &list);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);
    void validateAfterReadType(XSDLoadContext *loadContext, QDomElement &node, void *context);
    void validateAfterReadElement(XSDLoadContext *loadContext, QDomElement &node, void *context);

public:

    XSchemaElement(XSchemaObject *newParent, XSchemaRoot *newRoot, const ElementCategory category = EES_EMPTY);
    ~XSchemaElement();

    virtual QString tagName();
    virtual QString nameOrReference();
    //TODO: a seconda della categoria?
    virtual ESchemaType getType()
    {
        return SchemaTypeElement ;
    }
    virtual void reparent(XSchemaObject *newParent);

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

    bool isReferencingItself();
    /*

    Alla fine

            l'utente sceglie se semplice o complesso simpletype o complextype
            si abilita un altro checkbox che dice se
            contiene figli o no
    il complexcontent lo si fa con una estensione o restrizione.
    */

    //--------------------------------------
    DECL_QPROP(XEnums::XBool, abstract, setAbstract)
    DECL_QPROPS(fixed, setFixed)
    DECL_QPROP(XEnums::XBool, nillable, setNillable)
    DECL_QPROP(XEnums::XBool, mixed, setMixed)
    DECL_QPROPS(defaultValue, setDefaultValue)
    DECL_QPROPS(ref, setRef)
    /*DECL_QPROPS(substitutionGroup,setSubstitutionGroup);*/
    // TODO DECL_QPROP(XOccurrence,maxOccurs,setMaxOccurs);
    // TODO DECL_QPROP(XOccurrence,minOccurs,setMinOccurs);
    //TODO DECL_QPROP(XSchemaElement::EFinalType, final, setFinal);
    //TODO DECL_QPROP(XSchemaElement::EFinalType, block, setBlock);
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
    void collectAllAttributes(XSchemaInquiryContext &context, QList<XSchemaObject*> & baseAttributes);

    /*
    TODO
    serve un qualcosa che mi validi in modo semplice
    voglio un set simple type che elimini i figli non coerenti ecc

    XSchemaSimpleType *getSimpleType();
    */
signals:
    //void childAdded(XSchemaObject *newChild);
    void abstractChanged(const XEnums::XBool newValue);
    void nillableChanged(const XEnums::XBool newValue);
    void fixedChanged(const XEnums::XBool newValue);
    void mixedChanged(const XEnums::XBool newValue);
    void defaultValueChanged(const XEnums::XBool newValue);

    void maxOccursChanged(const int);
    void minOccursChanged(const int);
    //TODO void finalChanged(const XSchemaElement::EFinalType);
    //TODO void blockChanged(const XSchemaElement::EFinalType);
    void xsdTypeChanged(const QString & xsd);
    void refChanged(const QString & xsd);
    void categoryChanged(const XSchemaElement::ElementCategory);

protected:
    // IO handling
    void writeAppendElementAttributes(QDomDocument &document, QDomElement &node);
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
    void collectAttributesOfComplexDerived(XSchemaInquiryContext &context, QList<XSchemaObject*> &result);
    void collectAttributesOfSimpleDerived(XSchemaInquiryContext & /*context*/, QList<XSchemaObject*> &result);
    bool collectElements(QList<XSchemaObject*> &result);
    bool collectAllBaseTypeElements(XSchemaInquiryContext &context, QList<XSchemaObject*> &result, QList<XSchemaObject*> &attributes);
    bool collectAllElementsOfBaseTypes(XSchemaInquiryContext &context, QList<XSchemaObject*> &result);
    bool collectAllAttributesOfBaseTypes(XSchemaInquiryContext &context, QList<XSchemaObject*> &result);
    void collectTypeInfoOfSimpleDerived(XTypeQueryInfo &typeInfo);
    void collectTypeInfoOfSimpleType(XTypeQueryInfo &typeInfo);
    void collectTypeInfoOfComplexDerived(XTypeQueryInfo &typeInfo);
    XSchemaElement *finalTypeOrElement();
    void qualifiedTypeOrElementName(XTypeQueryInfo &typeInfo);
    //---
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);
    void raiseErrorTypesME(XSDLoadContext *loadContext, QDomElement &node);

public:
    virtual void markCompareStateRecursive(const XSDCompareState::EXSDCompareState newState);
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
    bool getTypeInfoAndRestrictions(XTypeQueryInfo &typeInfo);

    XSchemaElement *getReferencedElement();
    XSchemaElement *getReferencedType();
    XSchemaComplexContentExtension *getExtension();
    XSchemaComplexContentRestriction *getRestriction();
    XSchemaSimpleContentExtension *getSimpleContentExtension();
    XSchemaSimpleContentRestriction *getSimpleContentRestriction();
    XSchemaSimpleTypeRestriction *getSimpleTypeRestriction();
    XSchemaSimpleTypeList *getSimpleTypeList();
    XSchemaSimpleTypeUnion *getSimpleTypeUnion();
    bool isPredefined() const;
    void setIsPredefined(bool value);
};


class XSchemaChoice : public XSchemaObject
{
    Q_OBJECT

    XOccurrence _maxOccurs;
    XOccurrence _minOccurs;

    friend class TestXSDDiff; //TODO: until accessors
protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaChoice(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaChoice();

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);

    virtual ESchemaType getType()
    {
        return SchemaTypeChoice ;
    }
    TAG("choice")
    XOccurrence & maxOccurs();
    XOccurrence & minOccurs();
    virtual XSchemaElement* addElement();
    virtual XSchemaChoice* addChoice();
    virtual XSchemaSequence* addSequence();
    virtual XSchemaAttribute* addAttribute();

    ADD_E(true)
    ADD_S(true)
    ADD_C(true)

    using XSchemaObject::findComponentInValidationPath;
    virtual XSchemaObject *findComponentInValidationPath(const QString &component);
    virtual bool findSchemaChildComponents(XValidationContext *context, XElementContent *content);
    virtual bool collect(XValidationContext *context, XSingleElementContent *content);
    virtual QString description();

signals:
    //void childAdded(XSchemaObject *newChild);

};

class XSchemaSequence : public XSchemaObject
{
    Q_OBJECT

    XOccurrence _maxOccurs;
    XOccurrence _minOccurs;

    friend class TestXSDDiff;
protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void * context);

    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaSequence(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaSequence();

    virtual ESchemaType getType()
    {
        return SchemaTypeSequence ;
    }
    TAG("sequence")

    XOccurrence & maxOccurs();
    XOccurrence & minOccurs();

    virtual XSchemaElement* addElement();
    virtual XSchemaChoice* addChoice();
    virtual XSchemaSequence* addSequence();
    virtual XSchemaAttribute* addAttribute();

    virtual QString description();
    virtual QString descriptionForProposal();

    ADD_E(true)
    ADD_A(true)
    ADD_S(true)
    ADD_C(true)

    virtual bool collect(XValidationContext *context, XSingleElementContent *parent);

signals:
    //void childAdded(XSchemaObject *newChild);

};

class XSchemaAnnotation : public XSchemaObject
{
    Q_OBJECT
protected:

    QList<XInfoBase*> _infos;

    friend class TestXSDDiff;
    virtual void reset();
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);
    Element *makeElementList(Element *parent, XSDOperationParameters *params);
public:
    XSchemaAnnotation(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaAnnotation();

    virtual void loadFromDom(XSDLoadContext *loadContext, QDomElement &annotation);
    virtual void loadFromElement(XSDLoadContext *loadContext, Element *annotation, XSDOperationParameters * params);
    virtual Element *toElement(XSDOperationParameters * params);
    bool generateDom(QDomDocument &document, QDomNode & parent);
    virtual QString description();
    QString text();
    virtual void reparent(XSchemaObject *newParent);
    bool compareToSimple(const XSchemaAnnotation* other);
    XSchemaAnnotation* clone();
    void addXInfo(XInfoBase *newInfo);
    QList<XInfoBase*> infos();

    virtual ESchemaType getType()
    {
        return SchemaTypeAnnotation ;
    }
    TAG("annotation")

signals:
};


class XSchemaAll : public XSchemaObject
{
    Q_OBJECT

    XOccurrence _maxOccursOne;
    XOccurrence _minOccurs;
protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void * context);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaAll(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaAll();

    virtual ESchemaType getType()
    {
        return SchemaTypeAll ;
    }
    TAG("all")

    XOccurrence & maxOccurs();
    XOccurrence & minOccurs();
    virtual bool collect(XValidationContext * context, XSingleElementContent *parent);
    ADD_E(true)

signals:

};

class XSchemaAny : public XSchemaObject
{
    Q_OBJECT
public:

private:
    XOccurrence _maxOccurs;
    XOccurrence _minOccurs;
    QString _namespaces; //(##any,##local,##other,...)
    EProcessContent _processContent; // default: strict
protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaAny(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaAny();

    virtual ESchemaType getType()
    {
        return SchemaTypeAny ;
    }
    TAG("any")


    virtual QString description();
    XOccurrence & maxOccurs();
    XOccurrence & minOccurs();
    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);
};


class XSchemaAnyAttribute : public XSchemaObject
{
    Q_OBJECT
private:
    QStringList _namespaces; //(##any,##local,##other,...)
    EProcessContent _processContent;
protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaAnyAttribute(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaAnyAttribute();

    virtual ESchemaType getType()
    {
        return SchemaTypeAnyAttribute ;
    }
    TAG("anyAttribute")

    ADD_E(true)

signals:

};

class XSchemaAttributeGroup : public XSchemaObject
{
    Q_OBJECT

    QString _ref;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);
    bool handleReference(XSchemaInquiryContext &context, QList<XSchemaObject*> &result);

public:
    XSchemaAttributeGroup(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaAttributeGroup();

    virtual QString nameOrReference();

    virtual ESchemaType getType()
    {
        return SchemaTypeAttributeGroup ;
    }
    TAG("attributeGroup")

    bool hasAReference();
    QString referencedObjectName();
    XSchemaAttributeGroup *resolveReference(XSchemaAttributeGroup *base);

    virtual bool findBaseObjects(XSchemaInquiryContext &context, QList<XSchemaObject*> &baseElements, QList<XSchemaObject*> &baseAttributes);

    virtual QString description();
signals:

};


class XSchemaField : public XSchemaObject
{
    Q_OBJECT

    QString _xpath;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaField(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaField();

    virtual ESchemaType getType()
    {
        return SchemaTypeField ;
    }
    TAG("field")

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);

};

class XSchemaGroup : public XSchemaObject
{
    Q_OBJECT

    // if the parent is schema or redefine
    XOccurrence _maxOccurs;
    XOccurrence _minOccurs;
    QString _ref;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);
    bool handleReference(XSchemaInquiryContext &context, QList<XSchemaObject*> &result);

public:
    XSchemaGroup(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaGroup();
    virtual QString description();

    virtual ESchemaType getType()
    {
        return SchemaTypeGroup ;
    }
    TAG("group")

    virtual QString referencedObjectName();
    bool hasAReference();
    XSchemaGroup *resolveReference(XSchemaGroup *base);

    XOccurrence & maxOccurs();
    XOccurrence & minOccurs();
    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);

    virtual bool findBaseObjects(XSchemaInquiryContext &context, QList<XSchemaObject*> &baseElements, QList<XSchemaObject*> &baseAttributes);

    virtual bool collect(XValidationContext * context, XSingleElementContent *parent);

};

class XSchemaImport : public XSchemaObject
{
    Q_OBJECT

    QString _namespace;
    QString _schemaLocation;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaImport(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaImport();

    virtual ESchemaType getType()
    {
        return SchemaTypeImport ;
    }

    QString schemaLocation() ;
    QString targetNamespace() ;

    TAG("import")

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual QString description();
};

class XSchemaInclude : public XSchemaObject
{
    Q_OBJECT

    QString _schemaLocation;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaInclude(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaInclude();

    virtual ESchemaType getType()
    {
        return SchemaTypeInclude ;
    }

    QString schemaLocation() ;

    TAG("include")

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual QString description();

signals:

};

class XSchemaKey : public XSchemaObject
{
    Q_OBJECT

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaKey(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaKey();

    virtual ESchemaType getType()
    {
        return SchemaTypeKey ;
    }
    TAG("key")

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);

};

class XSchemaKeyRef : public XSchemaObject
{
    Q_OBJECT

    QString _refer;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaKeyRef(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaKeyRef();

    virtual ESchemaType getType()
    {
        return SchemaTypeKeyRef ;
    }
    TAG("keyref")

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);

};

class XSchemaUnique : public XSchemaObject
{
    Q_OBJECT

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaUnique(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaUnique();

    virtual ESchemaType getType()
    {
        return SchemaTypeUnique ;
    }
    TAG("unique")

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);

};

class XSchemaNotation : public XSchemaObject
{
    Q_OBJECT

    QString _publicIdentifier;
    QString _system;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaNotation(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaNotation();

    virtual ESchemaType getType()
    {
        return SchemaTypeNotation ;
    }
    TAG("notation")

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);
    virtual QString description();

};


class XSchemaRedefine : public XSchemaObject
{
    Q_OBJECT

    QString _schemaLocation;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaRedefine(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaRedefine();

    QString schemaLocation() ;

    virtual ESchemaType getType()
    {
        return SchemaTypeRedefine ;
    }
    TAG("redefine")

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);

};

class XSchemaSelector : public XSchemaObject
{
    Q_OBJECT

    QString _xpath;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaSelector(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaSelector();

    virtual ESchemaType getType()
    {
        return SchemaTypeSelector ;
    }
    TAG("selector")

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual void validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * context);

};

#include "xsdeditor/xsdcontent.h"
#include "xsdeditor/xsdsctypes.h"

class XInfoBase : public XSchemaObject
{
    QString _source;
    QString _contentString;
    QDomNode _content;

protected:

public:
    XInfoBase(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XInfoBase();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) = 0 ;
    virtual ESchemaType getType()
    {
        return SchemaTypeBaseDocumentation ;
    }
    void generateInnerNodes(QDomNode &parent);

    void setContent(QDomNode node)
    {
        _content = node ;
    }
    QDomNode &content()
    {
        return _content ;
    }
    void setSource(const QString &newSource)
    {
        _source = newSource;
    }
    QString source()
    {
        return _source ;
    }
    QString contentString()
    {
        return _contentString ;
    }
    void setContentString(const QString &newValue)
    {
        _contentString = newValue;
    }
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) ;
    void copyTo(XInfoBase *other);
    virtual XInfoBase* clone() = 0;
};


class XDocumentation : public XInfoBase
{
    QString _language;
protected:
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) ;

public:
    virtual ESchemaType getType()
    {
        return SchemaTypeDocumentation ;
    }
    TAG("documentation")

    XDocumentation(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XDocumentation();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    void setLanguage(const QString &newLanguage)
    {
        _language = newLanguage;
    }
    QString language()
    {
        return _language ;
    }

    virtual bool isAnnotationElement();
    virtual XDocumentation *clone();
};

class XAppInfo : public XInfoBase
{
protected:
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) ;

public:
    XAppInfo(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XAppInfo();
    virtual ESchemaType getType()
    {
        return SchemaTypeAppInfo ;
    }
    TAG("appinfo")
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual bool isAnnotationElement();
    virtual XAppInfo * clone();
};

class XSDSchema;

class XSchemaInfoPool
{
    /**
     * @brief the schemas that constitutefindElement the pool
     */
    QSet<XSDSchema*> _includesAndRedefines;
    /**
     * @brief all the redefined components live here for the whole schema.
     */
    QPair<QString, XSDSchema*> references;
    QList<XSchemaObject*> _redefinitions;
    XSDSchema *_mainSchema;
public:
    XSchemaInfoPool(XSDSchema *mainSchema);
    ~XSchemaInfoPool();

    void reset();
    void resetLite();
    QList<XSDSchema*> includes();
    void addInclude(XSDSchema *newInclude);
    void addRedefine(XSDSchema *newInclude);
    void addRedefinedTypes(XSchemaRedefine * redefine);
    XSchemaObject* findObject(const QString &name, const ESchemaType type);
    XSchemaElement* findElementOrType(const QString &name, const bool isType);

    // invoked from any schema of the pool
    XSchemaObject* getObject(const QString &name, const ESchemaType type);
    bool isBaseType(const QString &name, const ESchemaType type);

    QString targetNamespace() const;

    QList<XSchemaObject*> redefinitions();
    XSDSchema *mainSchema();
};

class XSDSchema : public XSchemaObject, public XSchemaRoot
{
    Q_OBJECT

    friend class XSchemaLoader;
    friend class XSchemaLoader1;
private:
    static QString _xsdURI;

    QString _location;

    EQualified _attributeFormDefault ;
    EQualified _elementFormDefault ;

    XSchemaInfoPool _ownInfoPool;
    XSchemaInfoPool *_infoPool;
    XSDSchema *_mainSchema;

    QString _version;
    QString _language ;

    QList <EFinalType> _blockDefault;
    QList <EFinalType> _finalDefault;

    // this is my own namespace;
    QString _namespacePrefix;
    QString _namespaceURI;
    QSet<QString> _namespaces;
    QMap<QString, QString> _namespacesByPrefix; // 1:1
    QMultiHash<QString, QString> _prefixesByNamespace; // 1:1
    QString _defaultNamespace ;
    QString _targetNamespace ;

    //----- region(include) ---
    QList<XSDSchema*>_includedSchemas;
    QMap<XSchemaInclude*, XSDSchema*>_includedSchemasByObject;
    //----- endregion(include) ---

    //----- region(redefine) ---
    QList<XSDSchema*>_redefinedSchemas;
    QMap<XSchemaRedefine*, XSDSchema*>_redefinedSchemasByObject;
    //----- endregion(redefine) ---

    //---- import: specific to this schema
    //----- region(import) ---
    QList<XSDSchema*>_importedSchemas;
    QMultiMap<QString, XSDSchema*>_importedSchemasByNamespace;
    QMap<XSchemaImport*, XSDSchema*>_importedSchemasByObject;
    //----- endregion(import) ---

    QMap<QString, XSchemaElement *> _baseTypes;

    //------
    void buildBaseTypes();
    void addBaseType(const EXType typeIndex);
    QString baseTypeName(const EXType typeIndex);

public:

    bool scanSchema(XSDLoadContext *loadContext, const QDomElement &schema);
    bool scanSchemaNS(const QDomElement &schema);

    QString elementsQualifiedString();
    QString attributesQualifiedString();

    QString findTargetNamespace();
    XSchemaElement *baseType(const QString &nameWOPrefix);
    QList<XSDSchema*> schemasByNamespace(const QString &theNamespace);

    QString targetNamespace() ;
    QString defaultNamespace() ;

    void setLocation(const QString &newLocation);
    QString location();

protected:

    void reset();
    virtual bool generateDom(QDomDocument &document) ;
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

    QString getAsString();
    QList<XSchemaObject*> getChildrenByType(const ESchemaType expected);
    QList<XSchemaInclude*> getIncludes();
    QList<XSchemaRedefine*> getRedefines();
    QList<XSchemaImport*> getImports();
    template< class T, ESchemaType atype>
    inline QList<T*> getFirstLevelChildren()
    {
        QList<XSchemaObject*> data = getChildrenByType(atype) ;
        QList<T*> result ;
        foreach(XSchemaObject * o, data) {
            result << (T*)o;
        }
        return result;
    }

    void addNamespace(QDomAttr &attr);
    void addDefaultNamespace(QDomAttr &attr);
    void addDefaultNamespace(const QString &ns);

    // RootItem() interface implementation -----------------------------
    QDomElement createElementWithNamespace(QDomDocument &document, const QString &tagName);
    QString xsdNS();
    QString namespacePrefix();
    QString namespaceURI();
    bool existsNamespace(const QString &namespaceToSearch);
    XSDSchema* schema();

    bool applyScan(XSDLoadContext *loadContext, QDomDocument &document);
    bool scanForNS(QDomDocument &documentNS);
    // -----------------------------------------------------------
    bool readFromInputString(XSDLoadContext *loadContext, const QString &inputText, const bool isRecursive, QNetworkAccessManager *newNetworkAccessManager, const QString &filePath);
    bool readFromIoDevice(XSDLoadContext *loadContext, QIODevice *file);
    void regenerateInternalLists();
    void registerData();
    void scanForInnerElements(XSchemaObject *parent, QList<XSchemaObject *> &lst);
    void scanForInnerElementReference(XSchemaObject *parent, QSet<QString> &references);

public:
    XSDSchema(XSDSchema *_mainSchema);
    ~XSDSchema();

    void postLoad();
    virtual ESchemaType getType()
    {
        return SchemaTypeSchema ;
    }
    TAG("schema")
    virtual XSchemaElement* addElement();
    virtual XSchemaAttribute *addAttribute();
    bool isAttributeFormQualified();
    bool isElementFormQualified();
    QStringList allNamespaces();

    ADD_E(true)
    ADD_A(true)

    bool read(XSDLoadContext *loadContext, const QString &filePath);
    bool saveToClipboard() ;
    bool readFromClipboard();
    bool readFromString(XSDLoadContext *loadContext, const QString &inputText, const bool isRecursive = false, QNetworkAccessManager *newNetworkAccessManager = NULL, const QString &filePath = NULL);

    virtual bool canAddChild(const ESchemaType newType);

    bool isValidSchema(const QDomElement &schema);
    static bool IsXsdValid(const QString &nsUri, const QString &localName);

    // general utility functions
    static const QString xsdURI()
    {
        return _xsdURI ;
    }
    QString qName(const QString &name);

    //-------------- region(single name)-----------------------------

    XSchemaGroup *topLevelGroup(const QString &referenceName);
    XSchemaElement *topLevelElement(const QString &referenceName);
    XSchemaAttribute *topLevelAttribute(const QString &referenceName);
    XSchemaAttributeGroup *topLevelAttributeGroup(const QString &referenceName);
    XSchemaElement *topLevelType(const QString &referenceName);
    bool isBaseType(const QString &referenceName);

    XSchemaElement *topLevelType(XSchemaFindReferenceContext &context, const QString &referenceName);
    XSchemaElement *topLevelElement(XSchemaFindReferenceContext &context, const QString &referenceName);

    XSchemaObject* findTopObject(const QString &name, const ESchemaType type);
    XSchemaObject* findReferencedObjectWithNamespace(const QString &findNamespace, const QString &name, const ESchemaType type);
    bool isBaseType(const QString &name, const ESchemaType type);
    //-------------- endregion(single name)-----------------------------

    //-------------- region(lists)-----------------------------
    void childListByName(const ESchemaType expected, QMap<QString, XSchemaObject *>&set, QList<XSchemaObject *> list);
    QList<XSchemaObject*> topLevelListByName(const ESchemaType expected, const bool onlyThisSchemaOrAll);
    QList<XSchemaObject*> getChildrenByType(const ESchemaType expected, const bool onlyThisSchemaOrAll);
    QList<XSchemaObject*> scanForAllInnerElements(const bool onlyThisSchemaOrAll);

    QList<XSchemaObject*> topLevelElements(const bool onlyThisSchemaOrAll);
    QList<XSchemaObject*> topLevelTypes(const bool onlyThisSchemaOrAll);
    QList<XSchemaObject*> topLevelAttributes(const bool onlyThisSchemaOrAll);
    QList<XSchemaObject*> topLevelAttributeGroups(const bool onlyThisSchemaOrAll);
    QList<XSchemaObject*> topLevelGroups(const bool onlyThisSchemaOrAll);
    QList<XSchemaObject*> topLevelReferences();
    QList<XSchemaObject*> allInnerElements(const bool onlyThisSchemaOrAll);
    //-------------- endregion(lists)-----------------------------

    virtual XSchemaObject *findComponentInValidationPath(const QString &component);
    virtual XSchemaObject *findComponentInValidationPath(SchemaSearchContext *context);
    virtual bool findSchemaChildComponents(XValidationContext *context, XElementContent *content);

    XSDCompareResult *compare(XSDCompareResult *result, XSchemaObject *target, XSDCompareOptions &options);

    XSchemaElement *findSimpleType(const QString &name);

    void processInclude(XSchemaInclude *include, XSDSchema * includedSchema);
    void processRedefine(XSchemaRedefine * redefine, XSDSchema * includedSchema);
    void processImport(XSchemaImport * importObject, XSDSchema * importedSchema);
    XSDSchema *includedSchemaByObject(XSchemaInclude *includeObject);
    XSDSchema *importedSchemaByObject(XSchemaImport *includeObject);
    XSDSchema *redefinedSchemaByObject(XSchemaRedefine *redefineObject);

    XSchemaInfoPool *infoPool();
    void setInfoPool(XSchemaInfoPool *value);

    XSDSchema *mainSchema() const;
    void setMainSchema(XSDSchema *mainSchema);
    QList<XSchemaElement *> collectCandidateRootElement();

signals:
    //void loadComplete(XSDSchema *schema, const bool isError);
    //void elementAdded(XSchemaElement* element);
};


// a simple container of existing objects and relationships
class XSchemaContainer : public XSchemaObject
{
    Q_OBJECT
    QList<XSchemaObject*> _existingChildren;

    QString _label;
protected:
    virtual void reset();

public:
    XSchemaContainer(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~ XSchemaContainer();

    virtual ESchemaType getType()
    {
        return SchemaContainer ;
    }
    void collectChildrenOfObject(XSchemaInquiryContext &context, XSchemaObject* target);
    void addExistingChild(XSchemaObject *newChild);

    QList<XSchemaObject*> &existingChildren()
    {
        return _existingChildren;
    }
    TAG("XXX")
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) ;

    QString label() const;
    void setLabel(const QString &label);
    void setFromElement(XSchemaElement *element);
    void setFromGroup(XSchemaGroup *object);
    void setFromAttributeGroup(XSchemaAttributeGroup *object);
    void setFromAttribute(XSchemaAttribute *attribute);
};

class Element ;
class XSchemaOther : public XSchemaObject
{
    Q_OBJECT
protected:

    QVariant extData;
    Element *extElement;

    virtual void reset();

public:
    XSchemaOther(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaOther();

    void setElement(Element *newElement);
    Element *element();
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject * /*target*/, XSDCompareOptions & /*options*/)
    {
        return XSDCompareObject::XSDOBJECT_DIFFERENT;
    }
    virtual bool generateDom(QDomDocument & /*document*/, QDomNode & /*parent*/)
    {
        return false;
    }

    virtual ESchemaType getType()
    {
        return SchemaOtherElement;
    }
    TAG("other")
    XSchemaOther* clone();

    QVariant getExtData() const;
    void setExtData(const QVariant & value);

signals:
};


#include "xsdeditor/xsdparseutils.h"

#endif // XSCHEMA_H
