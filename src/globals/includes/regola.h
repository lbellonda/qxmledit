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
#ifndef QXMLEDITWIDGET_REGOLA_H
#define QXMLEDITWIDGET_REGOLA_H

#include <QAbstractItemModel>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNode>
#include <QVariant>
#include <QtGui>
#include <QAbstractItemModel>

#include "libQXmlEdit_global.h"
#include "UIDelegate.h"

#include "element.h"
#include "bookmark.h"
#include "paintinfo.h"
#include "xmlprolog.h"

class Element;
class DocumentType;
class CopyAttributesSession;
class MetadataInfo;
class PseudoAttribute;
class QUndoGroup;
class QXmlEditData;

class LIBQXMLEDITSHARED_EXPORT DocumentDeviceProvider
{
public:
    virtual ~DocumentDeviceProvider();
    virtual QIODevice *newDeviceForWrite(const QString &path) = 0;
};

class XMLSaveContext;
class RegolaSettings;
class NamespaceManager;

class LIBQXMLEDITSHARED_EXPORT Regola : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool modified READ isModified WRITE setModified)

    friend class TestXsd;

    static const int ModelName;
    static const int ModelValue;
    static const int ModelColumns;

    static const QString DefaultEncoding;

    // constants
    enum EConsts {
        // undo limit
        UndoLimitCount = 10
    };

    bool _useIndent;
    int _indent;
    bool _useMixedContent;
    bool _isCrapCacheNSActivated;
    QHash<QString, QString> _crapCachePrefixNs;
    QUndoStack _undoStack;
    XmlProlog _prolog;
public:
    static const QString XsltNameSpace;
    static const QString XSDNameSpace;
    static const QString XSDSchemaInstance;

    typedef bool (*EditHook)(QWidget *const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate, Element* element);
    typedef bool (*EditTextHook)(QWidget *const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate, Element* element);
private:
    // This hook will throw an error, when used.
    EditHook _editHook ;
    EditTextHook _editTextHook ;

    QString _originalEncoding;

public:
    Regola(QDomDocument &document, const QString &name, const bool useMixedContent = false);
    Regola();
    ~Regola();

    Regola *lightClone();

    //----
    static Regola *loadFromDevice(QIODevice *ioDevice, const QString &fileName, QString *errorMessage);
    static Regola *loadFromFile(const QString &fileName, QString *errorMessage);
    //----
    //---------------------------------- Model Interface
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    //QVariant itemData(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    //bool hasChildren(const QModelIndex &index = QModelIndex()) const;

    //---------------------------------- end Model Interface

    //---region(encoding)
    QString encoding();
    QString originalEncoding();
    void checkEncoding(const bool isFirstTime = false);
    void checkEncodingByEl(Element *element);
    bool setEncoding(QTreeWidget *tree, UIDelegate *uiDelegate, const QString &newEncoding);
    //---endregion(encoding)

    void setDeviceProvider(DocumentDeviceProvider * value);

    bool isUseMixedContent();
    void setUseMixedContent(const bool value);

    QString docType();
    void setDocType(const QString& newDocType, const QString &systemId, const QString &publicId);

    void setPaintInfo(PaintInfo *newValue);
    PaintInfo *getPaintInfo();

    void setEditHook(EditHook theEditHook);
    void setEditTextHook(EditTextHook theEditTextHook);

    void caricaValori(QTreeWidget *pTree);
    bool isEmpty(const bool isRealElement);
    Element *root() const;
    Element *firstChild() const;
    Element *lastChild() const;
    Element *firstChildRecursive() const;
    Element *lastChildRecursive() const;
    Element *topElement(const int pos) const;
    void setRootElement(Element *newRoot);
    bool write(const QString &filePath);
    bool write(const QString &filePath, const bool isMarkSaved);
    bool write(QIODevice *device, const bool isMarkSaved);
    bool writeAlt(QIODevice *device, const bool isMarkSaved);
    bool writeAsJavaString(QIODevice *device);
    bool writeAsCString(QIODevice *device);
    bool writeAsJavaStringInFile(const QString &filePath);
    bool writeAsCStringInFile(const QString &filePath);
    QByteArray writeMemory();
    QString getAsText();
    QString getAsText(ElementLoadInfoMap *map);
    void removeAllElements(QTreeWidget *tree);
    void insertElementForce(Element *element);
    Element * syncRoot();

    Element *newElement();
    Element *newElement(const QString &tagName);
    Element *newElement(const Element::ElType newType);

    void setModified(const bool state);
    bool isModified() const;
    void setEmptyRoot();
    void expand(QTreeWidget *tree);

    QString &fileName();
    void setFileName(const QString &newFileName);

    void addChild(QWidget *window, QTreeWidget *tree);
    void addChild(QWidget *window, QTreeWidget *tree, Element *preElement);
    Element* addChildToElement(QWidget *window, QTreeWidget *tree, Element *parentElement, const QString &elementTag, const bool useUndo = false);
    void addBrother(QWidget* window, QTreeWidget *tree);
    void addBrother(QWidget *window, QTreeWidget *tree, Element* theNewElement);
    void paste(QTreeWidget *tree, Element *pasteElement);
    void paste(QTreeWidget *tree, QList<Element*> &pasteElements);
    void pasteAsSibling(QWidget *window, QTreeWidget *tree, Element *pasteElement);
    void pasteAsSibling(QWidget *window, QTreeWidget *tree, QList<Element*> &pasteElements);
    void pasteInternals(QTreeWidget *tree, Element *parentElement, Element *pasteElement, const int position);
    void pasteNoUI(Element *pasteElement, Element *pasteTo);
    Element *insertInternal(QTreeWidget *tree, Element *parentElement, Element *pasteElement, const int position);
    void pasteAttributesInternals(QTreeWidget *tree, Element *element, CopyAttributesSession *attributesCopied);
    void pasteClearAttributesInternals(QTreeWidget *tree, Element *element, CopyAttributesSession *attributesCopied);
    void pasteAttributes(QWidget *window, QTreeWidget *tree, CopyAttributesSession *attributesCopied);
    void pasteAttributes(QTreeWidget *tree, Element *element, CopyAttributesSession *attributesCopied);

    void redisplay();
    void redisplayElement(Element *element);
    void markSaved();
    void markEdited();
    bool editElement(QWidget * const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate) ;
    bool editInnerXMLBase64Element(QTreeWidgetItem *item, UIDelegate *uiDelegate);
    bool editInnerXMLElement(QTreeWidgetItem *item, UIDelegate *uiDelegate);
    bool editTextNodeElementBase64(QWidget *const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate);
    bool editAndSubstituteTextInNodeElement(QWidget *const parentWindow, Element *pElement, UIDelegate *uiDelegate);
    bool editEntry(QWidget * const parentWindow, const QString &title, const QString &label, const QString &actualText, QString &result);

    bool addTopElement(Element *theNewElement, const int position = -1);
    void notifyDeletionTopElement(Element *pEL);
    void appendComment(QWidget *window, QTreeWidget *tree);
    void appendComment(QWidget *window, QTreeWidget *tree, Element *newComment);
    void addComment(QWidget *window, QTreeWidget *tree);
    void addComment(QWidget *window, QTreeWidget *tree, Element *newComment);
    void appendProcessingInstruction(QWidget *window, QTreeWidget *tree);
    void appendProcessingInstruction(QWidget *window, QTreeWidget *tree, Element *newPI);
    void addProcessingInstruction(QWidget *window, QTreeWidget *tree);
    void addProcessingInstruction(QWidget *window, QTreeWidget *tree, Element *newPI);

    //---region(prolog)
    bool insertProlog(QTreeWidget *tree, const QString &encoding, const bool undoableOperation = true);
    Element *prolog();
    bool hasProlog();
    //---endregion(prolog)

    //---region(metadata)
    void insertMetadata(QTreeWidget *tree);
    void insertCompleteMetadata(QTreeWidget *tree);
    void updateMetadata(QTreeWidget *tree);
    bool parseMetadata(MetadataInfo *info);
    bool applyMetadata(QTreeWidget *tree, MetadataInfo *info);
    //--endregion(metadata)

    QVector<Element*> &getItems() {
        return childItems;
    }
    QVector<Element*> *getChildItems() {
        return &childItems;
    }

    static QBrush hiliteBkBrush;

    Element * findText(FindTextParams &findArgs, Element *selectedItem);
    Element * findNextTextMatch(FindTextParams &findArgs, Element *selectedItem);
    Element * replaceText(QTreeWidget *treeWidget, ReplaceTextParams &findArgs, Element *selectedItem);
    Element * replaceTextAndMove(QTreeWidget *treeWidget, ReplaceTextParams &replaceArgs, Element *selectedItem);
    void unhilite();

    void takeOutElement(Element*element);
    void addHilite(Element *element);
    void unhiliteAll();

    // bookmarks
    void clearBookmarks();
    bool addBookmark(Element* element);
    void removeBookmark(Element* element);
    void toggleBookmark(Element *element);
    bool isBookmarked(Element* element);

    Element *gotoPreviousBookmark();
    Element *gotoNextBookmark();

    int bookmarkSize();
    int nextBookmark();
    int previousBookmark();
    void handleSelection(Element *element);
    void notifyUIDeleted(Element *element);
    void takeoutFormBookmarks(Element *element);

    void deleteBookmarked();
    void cleanBookmarkedContents() ;

    bool loadXplore(const QString &fileName);

    QString documentXsd() {
        return _documentXsd ;
    }
    QString userDefinedXsd() {
        return _userDefinedXsd;
    }
    void setUserAndDocumentXsd(const QString &newXsd);

    void setUserDefinedXsd(const QString &path);
    void pasteXML(QTreeWidget *tree, Regola *newRegola) ;
    void pasteElements(QTreeWidget *tree, QList<Element*> elements);

    bool collectSizeData();
    void setCollectSizeData(const bool isCollect);
    void assignCollectSizeDataFlag(const bool isCollect);

    QString addNameToPool(const QString &inputString);
    QString getAttributeString(const QString &attributeName);
    QSet<QString> *namesPool();
    bool isValidXsd();
    void transformInComment(QWidget *window, QTreeWidget *tree, Element *elementToTransform);
    bool generateFromComment(QTreeWidget *tree, UIDelegate *uiDelegate, Element *elementToTransform);
    int indexOfTopLevelItem(Element *element);
    bool hasMoreThanOneXsdFile();

    Element *findChildElementByArray(Element *element, QList<int> &selection, const int listPos);
    Element *findElementByArray(QList<int> &selection);

    enum EInsSchemaRefInfo {
        INSERT_SCHEMA_ATTR_NOERROR = 0,
        INSERT_SCHEMA_ATTR_ERROR_NOROOT,
        INSERT_SCHEMA_ATTR_INFO_SCHEMAPRESENT,
        INSERT_SCHEMA_ATTR_INFO_REFPRESENT
    };
    EInsSchemaRefInfo insertNoNamespaceSchemaReferenceAttributes();
    EInsSchemaRefInfo insertSchemaReferenceAttributes();

    QString textOfCantEditMixedContentElementText();
    void hideLeafNodes();
    void showLeafNodes();

    bool moveUp(Element *element);
    bool moveDown(Element *element);
    bool internalMoveUp(Element *element, const bool registerInUndo);
    bool internalMoveDown(Element *element, const bool registerInUndo);

    void redo();
    void undo();
    bool canUndo();
    bool canRedo();
    int undoCount();

    bool checkIfUIItemsCorrespondToElements(Element *target);

    bool deleteElement(Element* element);
    bool deleteElement(Element* element, const bool isAutoDelete);
    bool takeElement(Element* element, const bool isUseUndo = false);
    void removeElementReference(Element *element);

    void emptyUndoStack();
    void dumpDocument(QDomDocument &document);

    void insertElementComplete(Element *theNewElement, Element *parentElement, QTreeWidget *tree);
    void appendElementComplete(Element *theNewElement, Element *brotherElement, QTreeWidget *tree, const bool useUndo);
    void refreshChildrenOfElement(Element *element, QTreeWidget *tree)    ;

    void namespacesPrefixFor(const QString &ns, Element *element, QSet<QString> &prefixes, QSet<QString> &allPrefixes);
    QSet<QString> namespacePrefixesXSD(Element *element);
    QString namespacePrefixFor(const QString &ns);
    QString namespacePrefixXSD();
    QString namespacePrefixXslt();
    QMap<QString, QString> namespaces();
    QString getCachedPrefixNS(const QString &nsURI);
    bool hasXSLTNamespace();
    QSet<QString> namespacesURI();
    QString namespaceOfPrefix(const QString &prefix);
    bool removeNillableAttribute(QTreeWidget *tree, Element *currentElement, NamespaceManager &namespaceManager);
    bool removeXSITypeAttribute(QTreeWidget *tree, Element *currentElement, NamespaceManager &namespaceManager);
    bool insertNillableAttribute(QTreeWidget *tree, Element *currentElement, NamespaceManager &namespaceManager);
    bool insertXSITypeAttribute(QTreeWidget *tree, Element *currentElement, const QString &typeValue, NamespaceManager &namespaceManager);

    static QList<Element*> decodeXMLFromString(const QString &input, const bool onlyRootElement, const bool onlyElements = true);

    int indentation() const;
    void setIndentation(const int indent);
    void setIndentationForce(const int indent);
    bool useIndentation() const ;
    void setUseIndentation(const bool value);
    void emitIndentationChange();

    //------------region(containers)
    void insertChildContainer(QTreeWidget *tree, Element *parentElement);
    void insertParent(QTreeWidget *tree, Element *parentElement);
    void removeParent(QTreeWidget *tree, Element *parentElement);

    void doInsertChildContainer(QTreeWidget *tree, const QString &tag, Element *parentElement);
    void doInsertParent(QTreeWidget *tree, const QString &tag, Element *parentElement);


    bool insertChildContainerAction(Element *parentElement, const QString &tag, QTreeWidget *tree);
    void removeChildContainerAction(Element *parentElement, QTreeWidget *tree);
    bool insertParentAction(Element *element, const QString &tag, QTreeWidget *tree, const int insPos, const int insLen);
    bool removeParentAction(Element *element, QTreeWidget *tree, int &insPos, int &insLen);
    //------------endregion(containers)

    void addUndo(QUndoCommand *undoGroup);
    void anonymize(AnonAlg *alg);
    void anonymize(AnonContext *context, QTreeWidget *treeWidget, const bool doDirect = false);

    QHash<QString, QString> mapOfSchemaReferences();

    static QString roleForKnownUri(const QString &uri);
    QString noNameSpaceXsd();
    QHash<QString, QSet<QString> > allNamespaces();

    RegolaSettings *getSettings();
    void restoreSettings(RegolaSettings *settings);


    NamespaceManager *namespaceManager() const;
    void setNamespaceManager(NamespaceManager *namespaceManager);

signals:
    void wasModified();
    void undoStateChanged();
    void uiDeleted(Element *element);
    void docTypeChanged(const QString &docType);
    void encodingChanged(const QString &encoding);
    void indentationChanged(const bool enabled, const int newIndent);

private:
    Element     *rootItem;
    // processing instructions and so on
    QVector<Element*> childItems;
    QString     xmlFileName;
    bool        modified;
    QHash<int, Element*> selection;
    Bookmarks   bookmarks;
    PaintInfo   *paintInfo;
    DocumentDeviceProvider *_deviceProvider;
    NamespaceManager *_namespaceManager ;


    /**
      XSD file referenced in the document (if any)
      */
    QString     _noNameSpaceXsd;
    QString     _documentXsd ; // see the note about xsd handling
    // list of namespaces - namespace name e.g.: <xsi, http://www.w3.org/2001/XMLSchema-instance>
    QHash<QString, QString> _namespacesByPrefixAndName;
    // the inverse of the previous one, <http://www.w3.org/2001/XMLSchema-instance, xsi>
    QHash<QString, QString> _namespacesByNameAndPrefix;
    // list of schemas - location e.g.: <http://www.w3.org/2001/XMLSchema-instance, xxx.xsd>
    QHash<QString, QString> _schemaLocationsByNamespace;
    QString     _userDefinedXsd;
    /**
      collect or not size informations about elements while editing
      */
    bool        _collectSizeData;
    QMap<QString, int>   _namesMap;
    QSet<QString>   _namesPool;
    QSet<QString>   _attributeValuesPool;
    DocumentType *_docType;

    void clear();
    void clearUndo();
    void addUndoInsert(QTreeWidget * tree, Element * element);
    QTreeWidgetItem *getSelItem(QTreeWidget *tree);
    bool editNodeElementAsXML(const bool isBase64Coded, Element *pElement, UIDelegate *uiDelegate);
    bool editNodeElement(QWidget *const parentWindow, Element *pElement, Element *pParentElement, const bool enableAllControls = true);
    bool editNodeComment(QWidget * const parentWindow, Element *pElement);
    bool editTextNodeElement(QWidget *const parentWindow, const bool isBase64Coded, Element *pElement);
    void recalcSize();

    void getBookmarkedElements(QVector<Element*> &elementsBookmarked);

    void checkValidationReference();
    void decodeASchema(const QString &value);

    int setTopLevelItemLike(Element *newElement, Element* oldElement);
    int swapAndDeleteElement(Element *newElement, Element* oldElement);

    void insertElementInternal(Element *theNewElement, Element *parentElement, QTreeWidget *tree);
    void insertElementInternal(Element *theNewElement, Element *parentElement, QTreeWidget *tree, const bool useUndo);
    void appendElementInternal(Element *theNewElement, Element *brotherElement, QTreeWidget *tree, const bool useUndo = true);
    void afterInsertHousekeeping(Element *theNewElement, QTreeWidget *tree);
    void afterInsertHousekeeping(Element *theNewElement, QTreeWidget *tree, const bool useUndo);
    void assignMixedContentText(Element *parent, const QString &text, const bool isCData, QVector<Element*> *collection);
    Element *assegnaValori(QDomNode &node, Element *parent, QVector<Element*> *collection);
    bool addUndoDeleteElement(Element* element);
    bool editAndSubstituteTextInNodeElementInternal(QWidget *const parentWindow, Element *pElement, UIDelegate *uiDelegate);
    void housework();
    void processDocument(QDomDocument &document);
    QDomDocument createNewDocument();
    void searchWithXQuery(FindTextParams &findArgs, Element *selectedItem);
    //metadata
    void updateMetadataRecord(QTreeWidget *tree, Element *metaElement, MetadataInfo *info, const bool metaExists = false);
    void appendAMetadatum(QTreeWidget *tree, PseudoAttribute *attribute, const QString &type);
    //---region(prolog)
    Element *insertPrologData(const QString &encoding);
    QString makePrologData(const QString &encoding);
    //---endregion(prolog)

    //------------region(containers)
    void insertParentForElement(QTreeWidget *tree, Element *element, Element *newParent, const int insPos, const int insLen, const bool replaceUI = true);
    bool removeParentForChildAndMoveOtherNodes(QTreeWidget *tree, Element *element, int &insPos, int &insLen, const bool replaceUI = true);
    //------------endregion(containers)

    Element *findTheNextTextMatch(FindTextParams &findArgs, Element *selectedItem);
    bool removeXSIAttribute(QTreeWidget *tree, Element *currentElement, NamespaceManager &namespaceManager, const QString &attributeToRemove);
    bool insertXSIAttribute(QTreeWidget *tree, Element *currentElement, NamespaceManager &namespaceManager,
                            const QString &newAttributeName, const QString &newAttribueValue);

    friend class TestSearch;
};

// external edit
bool EditCommentNode(QWidget * parent, Element *pTarget);

#endif // QXMLEDITWIDGET_REGOLA_H
