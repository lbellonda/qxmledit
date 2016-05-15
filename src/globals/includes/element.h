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
#ifndef QXMLEDITWIDGET_DOMITEM_H
#define QXMLEDITWIDGET_DOMITEM_H

#include <QtXml/QDomNode>
#include <QHash>
#include <QtGui>
#include <QRegExp>
#include "libQXmlEdit_global.h"

#include "paintinfo.h"

class UndoPasteAttributesCommand;
class Regola;
class FindTextParams;
class ReplaceTextParams;
class ElementViewInfo;
class AnonAlg;

class ElementOp
{
public:
    enum Op {
        EOInsChild = 0,
        EOInsSibling,
        EOModify
    };
};

class TextChunk
{
public:
    QString text;
    bool  isCDATA;

    TextChunk(const bool isCDATA, const QString &text);

    TextChunk *clone();
};

class QXmlException
{
public:
    QString cause;
    int code;
    QXmlException(const int newCode, const QString &theCause) {
        code = newCode ;
        cause = theCause ;
    }
};

class AnonContext ;
class AnonElem;
class AnonException;

class LIBQXMLEDITSHARED_EXPORT Attribute
{

public:
    QString name;
    QString value;

    Attribute();
    Attribute(const QString &newName, const QString &newValue);
    ~Attribute();

    Attribute *clone();

    /*!
     * \brief isDataAttribute whether not a xml system attribute
     * \return true if real data attribute
     */
    bool isDataAttribute();

    /*!
     * \brief anonWithContext anonymize the attribute, if possible
     * \param context
     * \param anonElem
     */
    void anonWithContext(AnonContext *context, AnonElem *anonElem, const QString &path);

    static bool copyAttributeListTo(QList<Attribute*> &sourceList, QList<Attribute*> &destList);
    static void emptyList(QList<Attribute*> &sourceList);

    void anonymize(AnonContext *context);

};

class ElementInfo
{
public:
    int numElements;
    int totalSize;

    ElementInfo();
    ~ElementInfo();

    void reset();
};

class LIBQXMLEDITSHARED_EXPORT ElementLoadInfoMap
{
public:

    QString currentKey;
    QHash<QString, Element*> dataMap;
    //-----
    ElementLoadInfoMap();
    ~ElementLoadInfoMap();
};

class XMLSaveContext;

class LIBQXMLEDITSHARED_EXPORT Element
{

    Q_DECLARE_TR_FUNCTIONS(Element)

public:

    static const int MAX_LIMIT_TEXTLEN = 100;
    static const int MAX_LIMIT_LARGE_TEXTLEN = 1000;
    static const int ATTR_PER_COLUMN = 5;
    static const int WIDTH_LEN_CHARS = 6;
#define DEFAULT_FIXEDWIDTHFONT "Courier"

    enum EViewModes {
        E_VM_NONE,
        /**
          a base 64 text
        */
        E_VM_BASE64TEXT,
        /**
          Base 64 coded XML
         */
        E_VM_BASE64XML,
        /**
          an XML as payload (CData or not )
          */
        E_VM_XML

    };
    // TODO uso di fields :1
//TODO private
    QString text;
    //QString nameSpace;
    QString _tag;
    QString tag();
    void setTag(const QString &newTag);
    bool isShowTextBase64;
    bool wasOpen ;
    ElementInfo selfInfo;
    ElementInfo childrenInfo;
    bool _isCData;
    //bool _isEntityReference;

    static QFont fixedWidthFont;
    static QFont *attrFont ;
    static int defaultAttrFonts ;
    static int fixedSizeAttrFonts ;

    enum ElType { ET_ATTRIBUTE = -1, ET_ELEMENT = 0, ET_PROCESSING_INSTRUCTION, ET_COMMENT, ET_TEXT /*, ET_ATTRIBUTE this is reserved*/};

    Element(const QString &newTag, const QString &text, Regola *regola, Element *parent = 0);
    Element(Regola *regola, const ElType newType, Element *parent);
    Element(Regola *regola);
    ~Element();
    Element *child(int i);
    Element *parent();
    Element *setParent(Element *newParent);

    Element *findFirstChildNamed(const QString &childTagName);
    Element *getFirstChildByLocalName(const QString &seekName);

    void namespaceOfElement(QString &elPrefix, QString &elLocalName);
    QString namespaceForPrefix(const QString &prefix);
    Attribute *nsDeclarationForPrefixOwned(const QString &prefix);
    bool areChildrenUsingPrefix(const QString &prefix);
    bool isUsingPrefixRecursive(const QString &prefix);
    bool findPrefixForNamespace(const QString nsToSearch, QString &prefixToSet);
    QHash<QString, QString> findVisibleNamespaces();


    static QRegExp terminatorSearch;
    static bool firstTimeFixedWidthFont;
    static QString textCompactViewPrefix;

    void caricaFigli(QTreeWidget *pTree, QTreeWidgetItem *parent, PaintInfo *paintInfo, const bool isGUI = true, const int pos = -1);
    void caricaFigli_to_refactor(QTreeWidget *pTree, QTreeWidgetItem *parent, PaintInfo *paintInfo, const bool isGUI, const int pos);
    void createUI(QTreeWidgetItem *parent, PaintInfo *paintInfo, const bool isGUI, const int pos = -1);
    void display(QTreeWidgetItem *me, PaintInfo *paintInfo, const bool setItem = true);
    void displayAlt(QTreeWidgetItem *me, PaintInfo *paintInfo);
    void setAttributesText(QTreeWidgetItem *me, PaintInfo *paintInfo, const QString &newText);
    void displayRecursive(PaintInfo *paintInfo);
    void displayWithPaintInfo(PaintInfo *paintInfo);
    void refreshUI();
    void refreshChildren(PaintInfo *paintInfo);

    QString getStringRepresentationForClipboard();

    void moveChildrenTo(Element *theNewElement);
    void moveUIChildrenTo(Element *theNewElement);

    bool isEmpty() const;
    bool generateDom(QDomDocument &document, QDomNode &parent, ElementLoadInfoMap *dataMap);
    bool generateDom(QDomDocument &document, QDomNode &parent);

    bool writeStream(XMLSaveContext *context, QXmlStreamWriter &writer, ElementLoadInfoMap *dataMap = NULL);

    void clearAttributes();
    bool addAttribute(const QString &name, const QString &value);
    void addChild(Element *newChild);
    void addChildAt(Element *newElement, const int position);
    int addChildAfter(Element *newElement, Element *brotherElement);

    bool moveDown(Element *element);
    bool moveUp(Element *element);

    static bool moveDown(QVector<Element*> &items, Element *element);
    static bool moveUp(QVector<Element*> &items, Element *element);

    void autoDelete(const bool deleteMe);
    void autoDeleteRecursive();

    static Element *fromItemData(QTreeWidgetItem *item);
    static Element *fromModelIndex(const QModelIndex & index);

    Element* copyToClipboard();
    Element *cutToClipboard();
    Element* copyTo(Element &newElement);
    Element *copyTo(Element &newElement, const bool isRecursive);
    void copyHeader(Element &newElement);
    void copyHeaderAndDirectNodes(Element &newElement);

    Element *detachFromParent();
    void removeReferencesFromRegola();
    void setRegola(Regola *newRegola, const bool isRecursive);

    QTreeWidgetItem *getUI() const {
        return ui;
    }

    void forceUpdateGui(const bool forceLayout);
    void repaint();
    bool isFirstChild() ;
    bool isLastChild() ;

    void expand(QTreeWidget *tree);
    void deleteUI();
    void deleteUnbindUI();

    void setText(const QString &data);
    void setTextOfTextNode(const QString &data, const bool isCData);

    ElType getType() const {
        return type ;
    }
    QVector<Element*> &getChildItemsRef() {
        return childItems;
    }
    QVector<Element*> *getChildItems() {
        return &childItems;
    }
    int getChildItemsCount() {
        return childItems.size();
    }

    QVector<Element*> &getItems() {
        return childItems;
    }

    Element* getChildAt(const int childIndex);
    int childIndex(Element *child);
    Element* firstChild();
    Element* lastChild();

    QVector<TextChunk*> &getTextChunks() {
        return textNodes;
    }

    int getTextChunksNumber() {
        return textNodes.size();
    }

    bool hasChildren() {
        return !childItems.isEmpty();
    }

    const QString &getComment();
    void setComment(const QString & Comment);
    static void loadIcons();

    void unexpandRecursive();
    void expandRecursive();

    //Attributi
    // TODO consider using an array
    QVector<Attribute*> attributes;

    void addTextNode(TextChunk *text);
    void clearTextNodes();
    bool isElement() const {
        return ET_ELEMENT == type ? true : false ;
    }
    bool isText() const {
        return ET_TEXT == type ? true : false ;
    }
    bool isTopLevel();

    const QString &getPITarget();
    const QString getPIData();
    void setPITarget(const QString & target);
    void setPIData(const QString & data);

    bool findText(FindTextParams &findArgs);
    bool matchText(FindTextParams &findArgs);
    bool searchInScope(FindTextParams &findArgs);
    bool replaceText(ReplaceTextParams &findArgs);

    void unhilite();
    void hilite();

    long getInstanceId() const {
        return instanceId;
    }

    QTreeWidgetItem *goToPreviousBrother();
    QTreeWidgetItem *goToNextBrother();
    void isFirstOrLastChild(bool &isFirst, bool &isLast);
    QString getRow();
    Regola *getParentRule();

    //--- hide / show
    void hideBrothers();
    void showBrothers();
    bool isNormalViewState();
    void registerState();

    bool isShownBase64() const ;
    void setShownBase64(const bool shownAs);

    QList<Attribute*>getAttributesList();
    Attribute* getAttribute(const QString &attributeName);
    QString getAttributeValue(const QString &attributeName);
    bool hasAttribute(const QString &attributeName);

    QString getAsText();
    QString getAsSimpleText(const bool isBase64);
    QString getAsSimpleTextXml(const bool isBase64);

    bool isCDATA();
    void setAsSingleTextNode(const QString &newText, const bool isBase64, const bool isCData);

    EViewModes viewMode() const ;
    void setViewMode(const EViewModes newMode) ;

    void markSavedRecursive();
    void markEditedRecursive();
    void markEdited();

    bool edited();
    bool saved();

    void recalcSize();
    int textSize();

    void updateSizeInfo(const bool isRecursive = false);
    void modifiedChildInfo();
    void addChildInfo(Element *child);
    void removeChildInfo(Element *childToRemove);
    void recalcSize(const bool isRecursive);
    void incrementSizeInfo(const int size);
    int setItemLike(Element *newElement, Element* oldElement);
    QString transformToText();
    int indexOfSelfAsChild();
    Element *previousSibling();
    Element *previousSiblingRecursive();
    Element *nextSibling();
    bool isMixedContent() ;

    //returns the position of the element
    QList<int> indexPath();
    QString indexPathString();
    QStringList path();
    QStringList parentPath();
    // copy the first level attributes, text nodes and attributes to the target
    bool copyTextNodesToTarget(Element *target);

    QString makeCSVFromSiblings();

    // filters all non element children
    QVector<Element*> getOnlyElementChildren();
    void copyPathToClipboard();
    QString localName();
    QString pathString();
    QString pathStringAsText();
    bool siblingsElementsMultGTValue(const int value, const bool isTextFail = false);
    bool parentIsRoot();

    bool removeChild(Element *toDelete);

private:
    static const int ShowDataRole = Qt::UserRole + 1;
public:
    /**
     * @brief HtmlAttribRole text as HTML for item delegate
     */
    static const int HtmlTextRole = Qt::UserRole + 2;
    static const int StyleIdRole = Qt::UserRole + 3;
private:
    static long   instances;

    long    instanceId;
    Element *parentElement;
    Regola *parentRule;
    QTreeWidgetItem *ui;
    EViewModes _viewMode;
    bool _edited;
    bool _saved;

    void houseWork(Regola *regola, Element *parent);

    void zeroUI();
    void zeroUISelf(const bool emitMe);

    QVector<TextChunk*> textNodes;
    ElType  type;
    QVector<Element*> childItems;

    //------------------------------------------------------------
    enum EVisibilityState {
        EVN_NORMAL,
        EVN_SHOW,
        EVN_HIDDEN,
        EVN_ELLIPSIS
    };
    EVisibilityState visibilityState ;
    //------------------------------------------------------------

    static QBrush commentBrush;
    static QBrush defaultBrush;
    static QBrush procInstrBrush;
    static QBrush attrBrush;

    static bool isLoadedIcons;
    static QIcon procInstrIcon;
    static QIcon commentIcon;
    static QIcon elementIcon;
    static QIcon procInstrBkmIcon;
    static QIcon commentBkmIcon;
    static QIcon elementBkmIcon;
    static QIcon bookmarkIcon;
    static QIcon textIcon;
    static QIcon textBkmIcon;
    static QIcon childrenHiddenIcon;
    static QIcon childrenHiddenBkmIcon;
    static QIcon filteredAttributesIcon;

    void generateId();


    QTreeWidgetItem *findPreviousBrother(QVector<Element*> &items);
    QTreeWidgetItem *findNextBrother(QVector<Element*> &items);
    void findFirstAndLast(QVector<Element*> &items, bool &isFirst, bool &isLast);
    static QString formatRowNumber(const int index);
    QString topLevelItemIndex(QTreeWidgetItem *topLevelItem);
    void resetVisibilityState();
    void setVisibilityStateEllipsis();
    void setVisibilityStateHidden();
    void setVisibilityStateShow();
    bool isVisibilityStateEllipsis();
    void createFixedWidthFonts();
    const QString styleElementTag(QTreeWidgetItem *me, PaintInfo *paintInfo);
    const QString findStyleElementTag(PaintInfo *paintInfo);
    StyleEntry *chooseStyle(VStyle* style);
    VStyle *calcStyle(PaintInfo *paintInfo);

    void collectChildInfo(Element *child, const bool isAdd);
    void propagateChildInfoChange();
    void recalcChildSize();

    static QString limitTextWithEllipsis(const QString &inputText);
    static QString limitLargeTextWithEllipsis(const QString &inputText);
    void autoDeleteRecursiveInner();

    void setChildrenLeavesHiddenState(QTreeWidgetItem *twi, const bool newState);

    void insertAttributesInMap(QMap<QString, QString> &attributesNameMap);
    void insertAttributesValuesInMap(QHash<QString, QString> &valuesMap);
    void insertAttributesInMap(QHash<QString, Attribute*> &valuesMap);
    QString insertAttributesInCSV(QMap<QString, QString> &attributesNameMap);
    static QString insertAttributesListInCSV(QVector<Element*> &elements);
    QString convertStringToHtml(const QString &textToShow, const bool isAttributesHTML);

    void displayComment(ElementViewInfo *dataInfo, const bool isCompactView, const bool isBookmarked, const bool isSelected);
    void displayProcessingInstruction(ElementViewInfo *dataInfo, const bool isBookmarked, const bool isSelected);
    const QString getIdUsingStyle(PaintInfo *paintInfo);
    void handleMapEncodingPreInsert(QDomNode &parent, ElementLoadInfoMap *dataMap);
    void handleMapEncodingPreInsert(ElementLoadInfoMap *dataMap);
    void anonymizeElement(AnonAlg *alg);
    void anonymizeText(AnonAlg *alg);
    QString anonymizeTextOfElement(AnonContext * context, const QString &inputText, void * key);
    void handleNamespace(AnonContext *context);

    bool checkSaveAndSetIndent(XMLSaveContext *context, QXmlStreamWriter &writer) const;

public:
    bool areChildrenLeavesHidden(QTreeWidgetItem *twi);

    static void setZoomFactor(const int zoom);
    void hideChildrenLeaves();
    void showChildrenLeaves();
    bool isALeaf();
    bool hasElementInChildren();

    const QString styleElementTagString(PaintInfo *paintInfo);
    QFont styleElementTagFont(PaintInfo *paintInfo);
    QBrush styleElementTagColor(PaintInfo *paintInfo);
    QIcon styleElementTagIcon();
    QVariant columnViewTooltipData(QHash<void *, QString> *mapDataAnon = NULL);
    QVariant columnViewDisplayData(PaintInfo *paintInfo, const int rowOrdinal);

    void addElementChildrenInList(QList<Element*> &childrenList);
    bool pasteAttributes(QList<Attribute*> &newAttributes, UndoPasteAttributesCommand *undoCommand, bool &isModified);
    bool pasteClearAttributes(QList<Attribute*> &newAttributes, UndoPasteAttributesCommand *undoCommand, bool &isModified);
    bool setAttributes(QList<Attribute*> newAttributes);
    bool setAttribute(const QString &name, const QString &value);
    bool removeAttribute(const QString &name);

    void getVisInfo(PaintInfo *paintInfo, ElementViewInfo *dataInfo, const bool isSelected, const QColor &selectedTextColor);
    QFont createElementFonts(QFont defaultFont);

    bool isHidden();
    QVector<Element*> & getParentChildren();

    bool canRemoveParent();
    bool canInsertParent();

    void anonymize(AnonAlg *alg);
    void anonymize(AnonContext *context);
    void anonymizeElement(AnonContext *context);
    void anonymizeText(AnonContext *context);

    //---
    Element *firstChildRecursiveOrThis();
    Element *lastChildRecursiveOrThis();
    Element *lastChildRecursive();

    void allNamespaces(QHash<QString, QSet<QString> > &nameSpacesMap);
    void declaredNamespaces(QHash<QString, QString> &prefixToNameSpacesMap);

    QString toString();
    bool compareToElement(Element *other, QString &msg);
    bool setOrClearAttribute(const bool isRemoveAttribute, const QString &attrName, const QString &value);
    bool handleMixedContentToInnerText();
    bool sortAttributesByMap(QList<int> oldPositions);
    void sortAttributes(QList<int> *undoPositionList, const bool isRecursive);

    static QList<Attribute*> sortAttributesList(const QVector<Attribute *> &attributes);

#ifdef  QXMLEDIT_TEST
    friend class TestStyle;
#endif
};

#endif //QXMLEDITWIDGET_DOMITEM_H
