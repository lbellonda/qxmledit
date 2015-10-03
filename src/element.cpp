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
#include "element.h"
#include "findtextparams.h"
#include "regola.h"
#include "config.h"
#include "utils.h"
#include "xmlutils.h"
#include "xmlsavecontext.h"

#include "undo/undopasteattributescommand.h"
#include "modules/anonymize/anonbase.h"

/***********************************************************************************************************
  Element class uses the following userData of the WidgetItem:
  UserData: a pointer to self
  ShowDataRole = Qt::UserRole + 1, a boolean  indicating show structure

***********************************************************************************************************/


QBrush Element::commentBrush(QColor(0xC0, 0xC0, 0xC0));
QBrush Element::defaultBrush(QColor(0xC0, 0x00, 0x00));
QBrush Element::procInstrBrush(QColor(0x00, 0xC0, 0xC0));
QBrush Element::attrBrush(QColor(0xC0, 0x00, 0x00));

bool Element::isLoadedIcons = false ;
QIcon Element::procInstrIcon;
QIcon Element::commentIcon;
QIcon Element::elementIcon;
QIcon Element::bookmarkIcon;
QIcon Element::procInstrBkmIcon;
QIcon Element::commentBkmIcon;
QIcon Element::elementBkmIcon;
QIcon Element::textIcon;
QIcon Element::textBkmIcon;
QIcon Element::childrenHiddenIcon;
QIcon Element::childrenHiddenBkmIcon;
QIcon Element::filteredAttributesIcon;

//const int Element::ShowDataRole = Qt::UserRole + 1;
//const int Element::HtmlAttribRole = Qt::UserRole + 2;

QRegExp Element::terminatorSearch("[\\n\\r]");

QFont Element::fixedWidthFont(DEFAULT_FIXEDWIDTHFONT);
QFont *Element::attrFont = NULL ;

int Element::defaultAttrFonts = 0 ;
int Element::fixedSizeAttrFonts = 0 ;

bool Element::firstTimeFixedWidthFont = true ;
QString Element::textCompactViewPrefix ;

//--------------------------------

ElementLoadInfoMap::ElementLoadInfoMap()
{
    //
}

ElementLoadInfoMap::~ElementLoadInfoMap()
{
    //
}


//----------------------------



ElementInfo::ElementInfo()
{
    reset();
}

ElementInfo::~ElementInfo()
{
}

void ElementInfo::reset()
{
    numElements = 0 ;
    totalSize = 0 ;
}

TextChunk::TextChunk(const bool isCDATASection, const QString &txt)
{
    isCDATA = isCDATASection ;
    text  = txt ;
}

TextChunk *TextChunk::clone()
{
    TextChunk *cloned = new TextChunk(isCDATA, text);
    return cloned;
}

long  Element::instances = 0;

Element::Element(const QString newTag, const QString &itext, Regola *regola, Element *parent)
{
    houseWork(regola, parent);
    _tag = newTag ;
    type = ET_ELEMENT;
    text = itext;
}

Element::Element(Regola *regola)
{
    houseWork(regola, NULL);
    type = ET_ELEMENT;
}

Element::Element(Regola *regola, const ElType newType, Element *parent)
{
    houseWork(regola, parent);
    type = newType ;
}

Element::~Element()
{
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        delete it.next();
    }
    clearTextNodes();
    clearAttributes();
    parentRule = NULL;
    parentElement = NULL ;
}

void Element::generateId()
{
    instanceId = instances++;
}

void Element::houseWork(Regola *regola, Element *parent)
{
    generateId();
    visibilityState = EVN_NORMAL ;
    isShowTextBase64 = false ;
    ui = NULL;
    parentRule = regola ;
    parentElement = parent;
    wasOpen = false;
    _isCData = false ;
    //_isEntityReference = false;
    _viewMode = E_VM_NONE;
    _edited = false;
    _saved = false;
}


Element *Element::parent()
{
    return parentElement ;
}

Element *Element::setParent(Element *newParent)
{
    Element *currentParent = parent();
    parentElement = newParent ;
    return currentParent ;
}


Element *Element::findFirstChildNamed(const QString &childTagName)
{
    foreach(Element * child, childItems) {
        if(child->tag() == childTagName) {
            return child;
        }
    }
    return NULL ;
}


Element *Element::getFirstChildByLocalName(const QString &seekName)
{
    foreach(Element * child, childItems) {
        if(child->localName() == seekName) {
            return child;
        }
    }
    return NULL;
}


void Element::loadIcons()
{
    if(isLoadedIcons)
        return ;
    isLoadedIcons = true ;
    procInstrIcon.addPixmap(QPixmap(QString::fromUtf8(":/tree/pi_icon.png")), QIcon::Normal, QIcon::Off);
    commentIcon.addPixmap(QPixmap(QString::fromUtf8(":/tree/comm_icon.png")), QIcon::Normal, QIcon::Off);
    elementIcon.addPixmap(QPixmap(QString::fromUtf8(":/tree/el_icon.png")), QIcon::Normal, QIcon::Off);
    procInstrBkmIcon.addPixmap(QPixmap(QString::fromUtf8(":/tree/pi_bkm_icon.png")), QIcon::Normal, QIcon::Off);
    commentBkmIcon.addPixmap(QPixmap(QString::fromUtf8(":/tree/comm_bk_icon.png")), QIcon::Normal, QIcon::Off);
    elementBkmIcon.addPixmap(QPixmap(QString::fromUtf8(":/tree/el_bkm.png")), QIcon::Normal, QIcon::Off);
    bookmarkIcon.addPixmap(QPixmap(QString::fromUtf8(":/special/bookmark.png")), QIcon::Normal, QIcon::Off);
    textIcon.addPixmap(QPixmap(QString::fromUtf8(":/tree/text.png")), QIcon::Normal, QIcon::Off);
    textBkmIcon.addPixmap(QPixmap(QString::fromUtf8(":/tree/el_bkm.png")), QIcon::Normal, QIcon::Off);
    childrenHiddenIcon.addPixmap(QPixmap(QString::fromUtf8(":/tree/hidden_children")), QIcon::Normal, QIcon::Off);
    childrenHiddenBkmIcon.addPixmap(QPixmap(QString::fromUtf8(":/tree/hidden_children_bm")), QIcon::Normal, QIcon::Off);
    filteredAttributesIcon.addPixmap(QPixmap(QString::fromUtf8(":/editText/filter")), QIcon::Normal, QIcon::Off);
    textCompactViewPrefix =  "     ";
}

bool Element::isShownBase64() const
{
    return isShowTextBase64 ;
}

void Element::setShownBase64(const bool shownAs)
{
    isShowTextBase64 = shownAs ;
}

Element::EViewModes Element::viewMode() const
{
    return _viewMode ;
}
void Element::setViewMode(const EViewModes newMode)
{
    _viewMode = newMode ;
}


QString Element::limitLargeTextWithEllipsis(const QString &inputText)
{
    bool appendEllipsis = false;
    QString result ;
    QString inputData = inputText.trimmed();
    if(inputText.length() > MAX_LIMIT_LARGE_TEXTLEN) {
        result = inputData.left(MAX_LIMIT_LARGE_TEXTLEN);
        appendEllipsis = true ;
    } else {
        result = inputData ;
    }
    if(appendEllipsis) {
        result.append(tr("..."));
    }
    return result;
}


QString Element::limitTextWithEllipsis(const QString &inputText)
{
    bool appendEllipsis = false;
    QString result ;
    QString inputData = inputText.simplified();
    if(inputText.length() > MAX_LIMIT_TEXTLEN) {
        result = inputData.left(MAX_LIMIT_TEXTLEN);
        appendEllipsis = true ;
    } else {
        result = inputData ;
    }
    int indexOfLF = result.indexOf(terminatorSearch, 0);
    if(indexOfLF >= 0) {
        result = result.left(indexOfLF);
        appendEllipsis = true ;
    }
    if(appendEllipsis) {
        result.append(tr("..."));
    }
    return result;
}

void Element::registerState()
{
    if(NULL != ui) {
        wasOpen = ui->isExpanded();
    } else {
        wasOpen = false ;
    }
    // passa ai figli
    foreach(Element * value, childItems) {
        value->registerState();
    }
}

void Element::displayRecursive(PaintInfo *paintInfo)
{
    if(NULL == ui) {
        return ;
    }
    display(ui, paintInfo);
    // passa ai figli
    foreach(Element * value, childItems) {
        value->displayRecursive(paintInfo);
    }
}

VStyle *Element::calcStyle(PaintInfo *paintInfo)
{
    VStyle* style = paintInfo->currentStyle();
    // check namespace
    if(NULL != style) {
        QString ns = style->getNamespace();
        if(!ns.isEmpty()) {
            QString prefix = parentRule->getCachedPrefixNS(ns);
            if(! _tag.startsWith(prefix)) {
                style = NULL ;
            }
        }
    }
    return style ;
}

StyleEntry *Element::chooseStyle(VStyle* style)
{
    StyleEntry *se = style->getStyleOfKeyword(_tag) ;
    if(NULL == se) {
        se = style->getCalculatedStyle(this);
    }
    if(NULL == se) {
        se = style->defaultStyleEntry();
    }
    return se;
}

const QString Element::styleElementTag(QTreeWidgetItem *me, PaintInfo *paintInfo)
{
    if(!VStyle::globalInitFlag) {
        VStyle::globalActivate(createElementFonts(me->font(0)), QApplication::palette().text());
    }
    QString qualifiedInfo;
    bool isFont = false ;
    bool isBrush = false ;
    VStyle* style = calcStyle(paintInfo);
    // check namespace
    if(NULL != style) {
        StyleEntry *se = chooseStyle(style) ;

        foreach(Attribute * attribute, attributes) {
            IdEntry *id = style->getIdEntry(attribute->name);
            if(NULL != id) {
                if(id->isAlpha()) {
                    qualifiedInfo.append(" '");
                    qualifiedInfo.append(limitLargeTextWithEllipsis(attribute->value));
                    qualifiedInfo.append("'");
                } else {
                    qualifiedInfo.append(" ");
                    qualifiedInfo.append(limitLargeTextWithEllipsis(attribute->value));
                }
            }
        }

        if(NULL != se) {
            if(se->isColor()) {
                me->setForeground(0, se->brush());
                isBrush = true ;
            }
            if(se->useBackColor()) {
                me->setBackground(0, se->backColor());
            }
            QFont *styleFont = se->font();
            if(NULL != styleFont) {
                me->setFont(0, *styleFont);
                isFont = true ;
            }
            if(!se->icon().isNull()) {
                me->setIcon(0, se->icon());
            }
        }
    }
    //usa il default se non assegnato
    if(!isBrush) {
        me->setForeground(0, VStyle::defaultBrush());
    }
    if(!isFont) {
        me->setFont(0, VStyle::defaultFont());
    }
    return qualifiedInfo;
}

const QString Element::styleElementTagString(PaintInfo *paintInfo)
{
    QString qualifiedInfo;
    VStyle* style = calcStyle(paintInfo);
    if(NULL != style) {
        foreach(Attribute * attribute, attributes) {
            IdEntry *id = style->getIdEntry(attribute->name);
            if(NULL != id) {
                if(id->isAlpha()) {
                    qualifiedInfo.append(" '");
                    qualifiedInfo.append(limitLargeTextWithEllipsis(attribute->value));
                    qualifiedInfo.append("'");
                } else {
                    qualifiedInfo.append(" ");
                    qualifiedInfo.append(limitLargeTextWithEllipsis(attribute->value));
                }
            }
        }
    }
    return qualifiedInfo;
}

QFont Element::styleElementTagFont(PaintInfo *paintInfo)
{
    VStyle* style = calcStyle(paintInfo);
    if(NULL != style) {
        StyleEntry *se = style->getStyleOfKeyword(tag()) ;
        if(NULL != se) {
            QFont *styleFont = se->font();
            if(NULL != styleFont) {
                return *styleFont;
            }
        }
    }
    return VStyle::defaultFont();
}

QBrush Element::styleElementTagColor(PaintInfo *paintInfo)
{
    VStyle* style = calcStyle(paintInfo);
    if(NULL != style) {
        StyleEntry *se = style->getStyleOfKeyword(tag()) ;
        if(NULL != se) {
            if(se->isColor()) {
                return se->brush();
            }
        }
    }
    //usa il default se non assegnato
    return VStyle::defaultBrush();
}

QIcon Element::styleElementTagIcon()
{
    switch(type) {
    case ET_ELEMENT:
        return elementIcon;
    case ET_PROCESSING_INSTRUCTION:
        return procInstrIcon;
    case ET_COMMENT:
        return commentIcon;
    case ET_TEXT:
        return textIcon;
    default:
        return QIcon();
    }
}

QVariant Element::columnViewTooltipData(QHash<void *, QString> *mapDataAnon)
{
    switch(getType()) {
    case Element::ET_ELEMENT: {
        bool isAnon = mapDataAnon != NULL ;
        QString theTag = tag();
        QString thePath = pathString();
        QString attributesLabel = "";
        if(getAttributesList().count() > 0) {
            attributesLabel += tr(" Attributes:\n");
        }

        QString tooltip = QString(tr("%1\npath: %2\n%3")).arg(theTag).arg(thePath).arg(attributesLabel);

        QMap<QString, Attribute*> sortedCollection;
        QVector<Attribute*>::iterator it;
        for(it = attributes.begin(); it != attributes.end(); ++it) {
            Attribute* attr = *it;
            if(NULL != attr) {
                sortedCollection.insert(attr->name, attr);
            }
        }
        foreach(QString key, sortedCollection.keys()) {
            Attribute *attr = sortedCollection[key];
            if(isAnon && (mapDataAnon->contains(attr))) {
                QString origValue = mapDataAnon->value(attr);
                QString attrValue = QString("     %1 (anon.)=\"%2\" org:\"%3\"\n").arg(key).arg(limitTextWithEllipsis(attr->value)).arg(limitTextWithEllipsis(origValue));
                tooltip += attrValue ;
            } else {
                QString attrValue = QString("     %1=\"%2\"\n").arg(key).arg(limitTextWithEllipsis(attr->value));
                tooltip += attrValue ;
            }
        }

        return QVariant(tooltip);
    }
    case Element::ET_TEXT: {
        QString textContained = text;
        if(textContained.length() > 50) {
            textContained = textContained.left(50);
            textContained += "...";
        }
        if((NULL != mapDataAnon) && (mapDataAnon->contains(this))) {
            QString str = mapDataAnon->value(this);
            if(str.length() > 50) {
                str = str.left(50);
                str += "...";
            }
            QString tooltip = QString(tr("text (anon):\n\"%1\"\n orig:\n\"%2\"")).arg(textContained).arg(str);
            return QVariant(tooltip);
        } else {
            QString tooltip = QString(tr("text:\n\"%1\"")).arg(textContained);
            if(Config::getBool(Config::KEY_ELEMENT_TEXT_TOOLTIP_IMAGE, true)) {
                if( text.startsWith("data:image") ) {
                    tooltip = QString("<html>%1<br/>%2<br/><img src=\"%3\"/></html>")
                              .arg(Utils::escapeHTML(tooltip))
                              .arg(Utils::escapeHTML(tr("Image base 64 coded:")))
                              .arg(Utils::escapeHTML(text));
                } else {
                    tooltip = QString("<html>%1<br/>%2<br/><img src=\"data:image;base64,%3\"/></html>")
                              .arg(Utils::escapeHTML(tooltip))
                              .arg(Utils::escapeHTML(tr("Image base 64 coded:")))
                              .arg(Utils::escapeHTML(text));
                }
            }
            return QVariant(tooltip);
        }
    }
    default:
        break;
    } // switch type
    return QVariant();
}

QVariant Element::columnViewDisplayData(PaintInfo *paintInfo, const int rowOrdinal)
{
    switch(getType()) {
    case Element::ET_COMMENT:
        return QVariant(QString("%1 Comment: %2").arg(rowOrdinal).arg(limitTextWithEllipsis(getComment())));
        break;
    case Element::ET_PROCESSING_INSTRUCTION:
        return QVariant(QString("%1 P.I.: %2 %3").arg(rowOrdinal).arg(getPITarget()).arg(getPIData()));
        break;
    case Element::ET_TEXT: {
        QString txt = text;
        if(txt.length() > 20) {
            txt = txt.left(20);
            txt += "...";
        }
        return QVariant(QString("%1 Text: %2").arg(rowOrdinal).arg(txt));
        break;
    }
    case Element::ET_ELEMENT: {
        int totalChildrenNumber = selfInfo.numElements + childrenInfo.numElements;
        QString childInfo;
        if(totalChildrenNumber > 0) {
            childInfo = QString("(%1)").arg(totalChildrenNumber);
        }
        QString elmInfo = styleElementTagString(paintInfo);
        return QVariant(QString("%1 %2 %3 %4").arg(rowOrdinal).arg(tag()).arg(elmInfo).arg(childInfo));
    }
    default:
        return QVariant(QString("%1 ???").arg(rowOrdinal));
        break;
    } // switch type
    return QVariant("");
}

void Element::setAttributesText(QTreeWidgetItem *me, PaintInfo *paintInfo, const QString &newText)
{
    if(paintInfo->attributesHTML()) {
        me->setData(paintInfo->columnForAttributes, HtmlTextRole, newText);
        me->setText(paintInfo->columnForAttributes, "");
    } else {
        me->setText(paintInfo->columnForAttributes, newText);
    }
}

void Element::display(QTreeWidgetItem *me, PaintInfo *paintInfo, const bool setItem)
{
    if(setItem && (ui != me)) {
        ui = me ;
        setChildrenLeavesHiddenState(me, false);
    }
    me->setData(0, Qt::UserRole, qVariantFromValue((void*)this));
    me->setText(0, "");
    displayAlt(me, paintInfo);
    if(wasOpen) {
        ui->setExpanded(true);
    }
}

QString Element::convertStringToHtml(const QString &textToShow, const bool isAttributesHTML)
{
    if(!isAttributesHTML) {
        return textToShow;
    }
    QString result = "<html><body bgcolor=\"transparent\"><I>";
    QString txt = textToShow;
    txt = txt.replace("&", "&amp;");
    txt = txt.replace("<", "&lt;");
    txt = txt.replace(">", "&gt;");
    result += txt;
    result += "</I></body></html>";
    return result;
}

void Element::createFixedWidthFonts()
{
    firstTimeFixedWidthFont = false ;
    fixedWidthFont.setFamily(DEFAULT_FIXEDWIDTHFONT);
    //se abilitato
    if(Config::getBool(Config::KEY_ELEMENT_FONTFIXEXDWIDTHENABLED, false)) {
        QString family = Config::getString(QString(Config::KEY_ELEMENT_FONTFIXEXDWIDTHFAMILY), DEFAULT_FIXEDWIDTHFONT);
        if(family.isEmpty()) {
            family = DEFAULT_FIXEDWIDTHFONT ;
        }
        fixedWidthFont.setFamily(family);
        int fixedWidthFontPoints = Config::getInt(Config::KEY_ELEMENT_FONTFIXEXDWIDTHSIZE, 0);
        if(fixedWidthFontPoints > 0) {
            fixedWidthFont.setPointSize(fixedWidthFontPoints);
        }
    }
    fixedSizeAttrFonts = fixedWidthFont.pointSize();
}

QFont Element::createElementFonts(QFont defaultFont)
{
    if(Config::getBool(Config::KEY_ELEMENT_ELEMENTFONTENABLED, false)) {
        QString elementFontName = Config::getString(Config::KEY_ELEMENT_ELEMENTFONTFAMILY, "");
        int elementFontSize = Config::getInt(Config::KEY_ELEMENT_ELEMENTFONTSIZE, 0);
        bool elementFontItalic = Config::getBool(Config::KEY_ELEMENT_ELEMENTFONTITALIC, false);
        bool elementFontBold = Config::getBool(Config::KEY_ELEMENT_ELEMENTFONTBOLD, false);
        if(elementFontName.isEmpty()) {
            elementFontName = defaultFont.family();
        }
        if(elementFontSize <= 0) {
            elementFontSize = defaultFont.pointSize();
        }
        QFont newDefaultFont(elementFontName, elementFontSize, elementFontBold ? QFont::Bold : -1, elementFontItalic);
        return newDefaultFont;
    } else {
        return defaultFont ;
    }
}


QString Element::getAsText()
{
    QString textToShow;
    QVectorIterator<TextChunk*> tt(textNodes);
    while(tt.hasNext()) {
        TextChunk   *tx = tt.next();
        if(tx->isCDATA) {
            textToShow.append(" [[CDATA:");
            textToShow.append(tx->text);
            textToShow.append(" ]]");
        } else {
            textToShow.append(tx->text);
        }
    }
    return textToShow ;
}

QString Element::getAsSimpleText(const bool isBase64)
{
    QString textToShow;
    if(type == ET_TEXT) {
        textToShow = text ;
    } else {
        QVectorIterator<TextChunk*> tt(textNodes);
        while(tt.hasNext()) {
            TextChunk   *tx = tt.next();
            textToShow.append(tx->text);
        }
    }
    if(isBase64) {
        textToShow = Utils::fromBase64(textToShow);
    }
    return textToShow ;
}

QString Element::getAsSimpleTextXml(const bool isBase64)
{
    QString textToShow;
    if(type == ET_TEXT) {
        textToShow = text ;
    } else {
        QVectorIterator<TextChunk*> tt(textNodes);
        while(tt.hasNext()) {
            TextChunk   *tx = tt.next();
            textToShow.append(tx->text);
        }
    }
    if(isBase64) {
        textToShow = Utils::fromBase64Xml(textToShow);
    }
    return textToShow ;
}

bool Element::isCDATA()
{
    if(type == ET_TEXT) {
        return _isCData ;
    }
    QVectorIterator<TextChunk*> tt(textNodes);
    while(tt.hasNext()) {
        TextChunk   *tx = tt.next();
        return tx->isCDATA ;
    }
    return false ;
}

void Element::setAsSingleTextNode(const QString &newText, const bool isBase64, const bool isCData)
{
    clearTextNodes();
    QString text = newText ;
    if(isBase64) {
        text = Utils::toBase64(newText);
    }
    if(type == ET_TEXT) {
        setTextOfTextNode(text, isCData);
    }  else {
        TextChunk *textChunk = new TextChunk(isCData, text);
        addTextNode(textChunk);
    }
    markEdited();
    //TODO fa il display?
}

void Element::markEdited()
{
    _edited = true;
    _saved  = false;
}

void Element::markSavedRecursive()
{
    if(_edited) {
        _saved  = true;
    }
    foreach(Element * value, childItems) {
        value->markSavedRecursive();
    }
}

void Element::markEditedRecursive()
{
    _edited = true;
    _saved  = false;
    foreach(Element * value, childItems) {
        value->markEditedRecursive();
    }
}

bool Element::edited()
{
    return _edited ;
}

bool Element::saved()
{
    return _saved ;
}


void Element::caricaFigli(QTreeWidget *pTree, QTreeWidgetItem *parent, PaintInfo *paintInfo, const bool isGUI, const int pos)
{
    D(printf("..statement %s, figli %d\n", text.toAscii().data(), childItems.size()));
    // Carica se stesso
    QTreeWidgetItem *me = NULL ;
    bool isTop = false;
    if(isGUI) {
        if(NULL == parent) {
            me = new QTreeWidgetItem(0);
            isTop = true ;
        } else {
            if(pos >= 0) {
                me = new QTreeWidgetItem();
                parent->insertChild(pos, me);
            } else {
                me = new QTreeWidgetItem(parent);
            }
        }
        display(me, paintInfo);
    }
    // passa ai figli
    foreach(Element * value, childItems) {
        value->caricaFigli(NULL, me, paintInfo, isGUI);
    }
    if(isTop) {
        if(pos >= 0) {
            pTree->insertTopLevelItem(pos, me);
        } else {
            pTree->addTopLevelItem(me);
        }
    }
}

void Element::createUI(QTreeWidgetItem *parent, PaintInfo *paintInfo, const bool isGUI, const int pos)
{
    QTreeWidgetItem *me = NULL ;
    if(isGUI) {
        if(NULL == parent) {
            me = new QTreeWidgetItem(0);
        } else {
            if(pos >= 0) {
                me = new QTreeWidgetItem();
                parent->insertChild(pos, me);
            } else {
                me = new QTreeWidgetItem(parent);
            }
        }
        display(me, paintInfo);
    }
}

void Element::refreshChildren(PaintInfo *paintInfo)
{
    // Carica se stesso
    QTreeWidgetItem *me = ui ;
    QList<QTreeWidgetItem *> childrenList = ui->takeChildren();
    foreach(QTreeWidgetItem * item, childrenList) {
        delete item;
    }

    // passa ai figli
    foreach(Element * value, childItems) {
        value->caricaFigli(NULL, me, paintInfo, true);
    }
}

void Element::caricaFigli_to_refactor(QTreeWidget *pTree, QTreeWidgetItem *parent, PaintInfo *paintInfo, const bool isGUI, const int pos)
{
    Utils::TODO_NEXT_RELEASE("refactor");
    D(printf("..statement %s, figli %d\n", text.toAscii().data(), childItems.size()));
    // Carica se stesso
    QTreeWidgetItem *me = NULL ;
    bool isTop = false;
    if(isGUI) {
        if(NULL == parent) {
            me = new QTreeWidgetItem(0);
            isTop = true ;
        } else {
            if(pos >= 0) {
                me = new QTreeWidgetItem();
                parent->insertChild(pos, me);
            } else {
                me = new QTreeWidgetItem(parent);
            }
        }
        if(isTop) {
            if(pos >= 0) {
                pTree->insertTopLevelItem(pos, me);
            } else {
                pTree->addTopLevelItem(me);
            }
        }
        display(me, paintInfo, false);
    }
    // passa ai figli
    foreach(Element * value, childItems) {
        value->caricaFigli_to_refactor(NULL, me, paintInfo, isGUI, -1);
    }
}

void Element::handleMapEncodingPreInsert(QDomNode &parent, ElementLoadInfoMap *dataMap)
{
    // make the key
    QString key = dataMap->currentKey + "*" ;
    int thisPos = parent.childNodes().size()  ;
    key = QString("%1*%2").arg(dataMap->currentKey).arg(thisPos) ;
    dataMap->dataMap.insert(key, this);
    dataMap->currentKey = key ;
}

void Element::handleMapEncodingPreInsert(ElementLoadInfoMap *dataMap)
{
    // make the key
    QString key = dataMap->currentKey + "*" ;
    // single test noded does not show up, but this is not a problem. Only registering top level objects.
    int thisPos = indexOfSelfAsChild() ;
    key = QString("%1*%2").arg(dataMap->currentKey).arg(thisPos) ;
    dataMap->dataMap.insert(key, this);
    dataMap->currentKey = key ;
}

bool Element::generateDom(QDomDocument &document, QDomNode &parent)
{
    return generateDom(document, parent, NULL);
}

bool Element::generateDom(QDomDocument &document, QDomNode &parent, ElementLoadInfoMap *dataMap)
{
    bool result = true;
    QString prevDMKey ;
    if(NULL != dataMap) {
        prevDMKey = dataMap->currentKey ;
        handleMapEncodingPreInsert(parent, dataMap);
    }
    switch(type) {
    default:
    case ET_ELEMENT: {
        // appends itself
        QDomElement node = document.createElement(tag());

        QVectorIterator<TextChunk*> tt(textNodes);
        while(tt.hasNext()) {
            TextChunk   *tx = tt.next();
            if(tx->isCDATA) {
                QDomCDATASection nodeCData = document.createCDATASection(tx->text);
                node.appendChild(nodeCData);
            } else {
                QDomText nodeText = document.createTextNode(tx->text);
                node.appendChild(nodeText);
            }
        }

        //itera sulla lista e prendi i valori dalla chiabe
        QVectorIterator<Attribute*>  attrs(attributes);
        while(attrs.hasNext()) {
            Attribute* attribute = attrs.next();
            node.setAttribute(attribute->name, attribute->value);
        }

        parent.appendChild(node);
        //append children
        foreach(Element * value, childItems) {
            if(!value->generateDom(document, node, dataMap)) {
                result = false;
                break;
            }
        }
    }
    break;

    case ET_PROCESSING_INSTRUCTION: {
        QDomProcessingInstruction node = document.createProcessingInstruction(getPITarget(), getPIData());
        parent.appendChild(node);
    }
    break;

    case ET_COMMENT: {
        QDomComment node = document.createComment(getComment());
        parent.appendChild(node);
    }
    break;

    case ET_TEXT: {
        if(_isCData) {
            QDomCDATASection nodeCData = document.createCDATASection(text);
            parent.appendChild(nodeCData);
        } else {
            QDomText nodeText = document.createTextNode(text);
            parent.appendChild(nodeText);
        }
    }
    break;

    }
    if(NULL != dataMap) {
        dataMap->currentKey = prevDMKey ;
    }
    return result;
}

bool Element::writeStream(XMLSaveContext *context, QXmlStreamWriter &writer, ElementLoadInfoMap *dataMap)
{
    bool result = true;
    QString prevDMKey ;
    if(NULL != dataMap) {
        prevDMKey = dataMap->currentKey ;
        handleMapEncodingPreInsert(dataMap);
    }
    switch(type) {
    default:
    case ET_ELEMENT: {
        // appends itself
        writer.writeStartElement(tag());
        checkSaveAndSetIndent(context, writer);

        //itera sulla lista e prendi i valori dalla chiave
        if(context->isSortAttributesAlpha()) {
            QMap<QString, QString> sortedCollection;
            QVector<Attribute*>::iterator it;
            for(it = attributes.begin(); it != attributes.end(); ++it) {
                Attribute* attr = *it;
                if(NULL != attr) {
                    sortedCollection.insert(attr->name, attr->value);
                }
            }
            foreach(QString key, sortedCollection.keys()) {
                QString value = sortedCollection[key];
                writer.writeAttribute(key, value);
            }
        } else {
            QVectorIterator<Attribute*>  attrs(attributes);
            while(attrs.hasNext()) {
                Attribute* attribute = attrs.next();
                writer.writeAttribute(attribute->name, attribute->value);
            }
        }

        QVectorIterator<TextChunk*> tt(textNodes);
        while(tt.hasNext()) {
            TextChunk   *tx = tt.next();
            if(tx->isCDATA) {
                writer.writeCDATA(tx->text);
            } else {
                writer.writeCharacters(tx->text);
            }
        }
        foreach(Element * value, childItems) {
            if(!value->writeStream(context, writer, dataMap)) {
                result = false;
                break;
            }
        }
        writer.writeEndElement();
    }
    break;

    case ET_PROCESSING_INSTRUCTION: {
        writer.writeProcessingInstruction(getPITarget(), getPIData());
    }
    break;

    case ET_COMMENT: {
        writer.writeComment(getComment());
    }
    break;

    case ET_TEXT: {
        if(_isCData) {
            writer.writeCDATA(text);
        } else {
            writer.writeCharacters(text);
        }
    }
    break;

    }
    checkSaveAndSetIndent(context, writer);

    if(NULL != dataMap) {
        dataMap->currentKey = prevDMKey ;
    }
    return result;
}

bool Element::checkSaveAndSetIndent(XMLSaveContext *context, QXmlStreamWriter &writer) const
{
    if(!context->isDoIndent()) {
        context->setDoIndent(true);
        if(context->indentation() >= 0) {
            writer.setAutoFormatting(true);
            writer.setAutoFormattingIndent(context->indentation());
            return true ;
        }
    }
    return false;
}

bool Element::isEmpty() const
{
    return childItems.isEmpty();
}

void Element::clearAttributes()
{
    QVectorIterator<Attribute*> it(attributes);
    while(it.hasNext()) {
        delete it.next();
    }
    attributes.clear();
}

bool Element::addAttribute(const QString &name, const QString &value)
{
    if(!name.isEmpty()) {
        bool isExisting = false;
        foreach(Attribute * attr, attributes) {
            if(attr->name == name) {
                isExisting = true ;
                attr->value = value ;
                break;
            }
        } // foreach()
        if(!isExisting) {
            Attribute *attribute = new Attribute(name, value);
            //if( NULL == attribute ) { TODO
            //    throw new QXmlException(ERROR_1, tr("cannot add an attribute") );
            //}
            attributes.append(attribute);
        }
        return true;
    }
    return false;
}

Element *Element::fromItemData(QTreeWidgetItem *item)
{
    if(NULL == item) {
        return NULL ;
    }
    QVariant data = item->data(0, Qt::UserRole);
    Element *pElement = (Element *)data.value<void*>();
    return pElement ;
}

Element *Element::fromModelIndex(const QModelIndex & index)
{
    if(!index.isValid()) {
        return NULL ;
    }
    QVariant data = index.data(Qt::UserRole);
    Element *pElement = (Element *)data.value<void*>();
    return pElement ;
}

void Element::addChild(Element *newElement)
{
    newElement->parentElement = this ;
    childItems.append(newElement);
    addChildInfo(newElement);
}

void Element::addChildAt(Element *newElement, const int position)
{
    newElement->parentElement = this ;
    childItems.insert(position, newElement);
    addChildInfo(newElement);
}


int Element::addChildAfter(Element *newElement, Element *brotherElement)
{
    newElement->parentElement = this ;
    if(NULL == brotherElement) {
        childItems.append(newElement);
        return childItems.size() - 1;
    }
    QVectorIterator<Element*> it(childItems);
    int index = 0;
    while(it.hasNext()) {
        if(it.next() == brotherElement) {
            childItems.insert(index + 1, newElement);
            return index + 1 ;
        }
        index ++;
    }
    // not found, print a warning
    // TODO: D_W("addChildAfter: not found");
    childItems.append(newElement);
    addChildInfo(newElement);
    parentRule->setModified(true) ;
    return childItems.size() - 1;
}

bool Element::moveUp(QVector<Element*> &items, Element *element)
{
    int indexOf = items.indexOf(element);
    if(indexOf <= 0)
        return false;
    Element *pToSwap = items.at(indexOf - 1);
    items.replace(indexOf - 1, element);
    items.replace(indexOf, pToSwap);
    // user interface
    QTreeWidget *tree = element->ui->treeWidget();
    QTreeWidgetItem *item = NULL;
    Element *parent = element->parentElement;
    if(NULL == parent) {
        item = tree->takeTopLevelItem(indexOf);
        QTreeWidgetItem *item1p = tree->takeTopLevelItem(indexOf - 1);
        tree->insertTopLevelItem(indexOf - 1, item);
        tree->insertTopLevelItem(indexOf, item1p);
    } else {
        QTreeWidgetItem *parentUI = parent->ui;
        item = parentUI->takeChild(indexOf);
        QTreeWidgetItem *item1p = parentUI->takeChild(indexOf - 1);
        parentUI->insertChild(indexOf - 1, item);
        parentUI->insertChild(indexOf, item1p);
    }
    tree->setCurrentItem(item);
    element->markEditedRecursive();
    element->parentRule->setModified(true) ;
    return true;
}

bool Element::moveUp(Element *element)
{
    return moveUp(childItems, element);
}

bool Element::moveDown(QVector<Element*> &items, Element *element)
{
    int indexOf = items.indexOf(element);
    if((indexOf < 0) || (indexOf >= (items.size() - 1)))
        return false;
    Element *pToSwap = items.at(indexOf + 1);
    items.replace(indexOf + 1, element);
    items.replace(indexOf, pToSwap);
    // user interface
    /*
        QTreeWidgetItem *item1p = ui->takeChild(indexOf+1);
        QTreeWidgetItem *item1 = ui->takeChild(indexOf);
        ui->insertChild(indexOf, item1p);
        ui->insertChild(indexOf+1, item1);
        return true;
    */
    QTreeWidget *tree = element->ui->treeWidget();
    QTreeWidgetItem *item = NULL;
    QTreeWidgetItem *item1p ;
    Element *parent = element->parentElement;
    if(NULL == parent) {
        item1p = tree->takeTopLevelItem(indexOf + 1);
        item = tree->takeTopLevelItem(indexOf);
        tree->insertTopLevelItem(indexOf, item1p);
        tree->insertTopLevelItem(indexOf + 1, item);
    } else {
        QTreeWidgetItem *parentUI = parent->ui;

        item1p = parentUI->takeChild(indexOf + 1);
        item = parentUI->takeChild(indexOf);
        parentUI->insertChild(indexOf, item1p);
        parentUI->insertChild(indexOf + 1, item);
    }
    tree->setCurrentItem(item);
    element->markEditedRecursive();
    element->parentRule->setModified(true) ;
    return true;
}

bool Element::moveDown(Element *element)
{
    return moveDown(childItems, element);
}

void Element::zeroUI()
{
    zeroUISelf(true);
    foreach(Element * item, childItems) {
        item->zeroUI();
    }
}

void Element::zeroUISelf(const bool emitMe)
{
    parentRule->takeOutElement(this);
    ui = NULL ;
    if(emitMe) {
        parentRule->notifyUIDeleted(this);
    } else {
        parentRule->takeoutFormBookmarks(this);
    }
}

Regola *Element::getParentRule()
{
    return parentRule ;
}

void Element::deleteUI()
{
    if(NULL != ui) {
        QTreeWidgetItem *parentWItem = ui->parent();
        int index;
        if(NULL != parentWItem) {
            index = parentWItem->indexOfChild(ui);
            delete parentWItem->takeChild(index);
        } else {
            index = ui->treeWidget()->indexOfTopLevelItem(ui);
            delete ui->treeWidget()->takeTopLevelItem(index);
        }
        // elimina tutti i figli
        zeroUI();
    }
}

void Element::deleteUnbindUI()
{
    if(NULL != ui) {
        delete ui;
        // elimina tutti i figli
        zeroUI();
    }
}

void Element::autoDelete(const bool deleteMe)
{
    // sgancia l'elemento dal parent
    if(NULL != parentElement) {
        int indexOf = parentElement->childItems.indexOf(this);
        parentElement->childItems.remove(indexOf);
    } else {
        // iif it is a true element
        parentRule->notifyDeletionTopElement(this);
    }
    parentRule->takeOutElement(this) ;
    // this operation causes a notify message and must be done after processing.
    deleteUI();
    parentRule->setModified(true) ;
    parentRule = NULL;
    if(deleteMe) {
        delete this;
    }
}

void Element::autoDeleteRecursiveInner()
{
    foreach(Element * child, childItems) {
        child->autoDeleteRecursiveInner();
    }
    // let the parent deal with this
    zeroUISelf(false);
    ui = NULL ;
    parentRule = NULL;
}

void Element::autoDeleteRecursive()
{
    foreach(Element * child, childItems) {
        child->autoDeleteRecursiveInner();
        delete child;
    }
    childItems.clear();
    // sgancia l'elemento dal parent
    if(NULL != parentElement) {
        int indexOf = parentElement->childItems.indexOf(this);
        parentElement->childItems.remove(indexOf);
    } else {
        // iif it is a true element
        parentRule->notifyDeletionTopElement(this);
    }
    parentRule->takeOutElement(this) ;
    // this operation causes a notify message and must be done after processing.
    if(NULL != ui) {
        deleteUI();
    } else {
        zeroUISelf(false);
    }
    parentRule->setModified(true) ;
    parentRule = NULL;
    delete this;
}

Element *Element::copyTo(Element &newElement)
{
    return copyTo(newElement, true);
}

Element *Element::copyTo(Element &newElement, const bool isRecursive)
{
    copyHeaderAndDirectNodes(newElement);
    // TODO newElement.nameSpace = nameSpace;
    if(isRecursive) {
        QVectorIterator<Element*> it(childItems);
        while(it.hasNext()) {
            Element *newEl = new Element(newElement.parentRule);
            newElement.addChild(newEl);
            it.next()->copyTo(*newEl, isRecursive);
        }
    }
    return &newElement;
}


void Element::copyHeader(Element &newElement)
{
    // TODO newElement.nameSpace = nameSpace;
    if(NULL != newElement.parentRule) {
        newElement._tag = newElement.parentRule->addNameToPool(_tag);
    } else {
        newElement._tag = _tag ;
    }
    newElement.text = text;
    newElement.type = type ;
    newElement._viewMode = _viewMode ;
    newElement._isCData = _isCData ;
    //newElement._isEntityReference = _isEntityReference ;
}

void Element::copyHeaderAndDirectNodes(Element &newElement)
{
    copyHeader(newElement);
    newElement.clearTextNodes();
    newElement.clearAttributes();

    QVectorIterator<Attribute*>attr(attributes);
    while(attr.hasNext()) {
        Attribute *src = attr.next();
        Attribute *dst = new Attribute(src->name, src->value);
        newElement.attributes.append(dst);
    }

    QVectorIterator<TextChunk*> tt(textNodes);
    while(tt.hasNext()) {
        TextChunk  *tx = tt.next();
        TextChunk  *newText = tx->clone();
        newElement.textNodes.append(newText);
    }
}


Element* Element::copyToClipboard()
{
    Element *newElement = new Element(NULL);
    if(NULL != newElement) {
        copyTo(*newElement);
    }
    return newElement;
}

Element *Element::cutToClipboard()
{
    // detach from parent
    if(NULL != parentElement) {
        parentElement->removeChildInfo(this);
    }
    parentRule->deleteElement(this, false);
    return this;
}

void Element::setRegola(Regola *newRegola, const bool isRecursive)
{
    parentRule = newRegola ;
    if(isRecursive) {
        foreach(Element * child, childItems) {
            child->setRegola(newRegola, isRecursive);
        }
    }
}

void Element::removeReferencesFromRegola()
{
    if(NULL != parentRule) {
        parentRule->removeElementReference(this);
    }
    foreach(Element * child, childItems) {
        child->removeReferencesFromRegola();
    }
}

Element *Element::detachFromParent()
{
    // detach from parent
    if(NULL != parentElement) {
        parentElement->removeChildInfo(this);
        int indexAsChild = parentElement->indexOfSelfAsChild();
        if(indexAsChild >= 0) {
            parentElement->childItems.remove(indexAsChild);
        }
    }
    removeReferencesFromRegola();
    setRegola(NULL, true);
    // recurse to detach rule from children
    return this;
}

bool Element::isFirstChild()
{
    if(NULL == parentElement) {
        return (parentRule->getItems().indexOf(this) <= 0) ? true : false;
    }
    return (parentElement->childItems.indexOf(this) <= 0) ? true : false;
}

bool Element::isLastChild()
{
    int indexOf = -1 ;
    int size = 0;
    if(NULL == parentElement) {
        QVector<Element*> &items = parentRule->getItems();
        size = items.size();
        indexOf = items.indexOf(this);
    } else {
        indexOf = parentElement->childItems.indexOf(this);
        size = parentElement->childItems.size();
    }
    if((indexOf < 0) || (indexOf >= (size - 1)))
        return true;
    return false;
}

void Element::expand(QTreeWidget *tree)
{
    if(NULL != ui)
        tree->expandItem(ui);
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        it.next()->expand(tree);
    }
}

Element* Element::getChildAt(const int childIndex)
{
    if((childIndex >= 0) && (childIndex < childItems.size())) {
        return childItems.at(childIndex);
    }
    return NULL ;
}

Element* Element::firstChild()
{
    if(childItems.isEmpty()) {
        return NULL ;
    } else {
        return childItems.first();
    }
}

Element* Element::lastChild()
{
    if(childItems.isEmpty()) {
        return NULL ;
    } else {
        return childItems.last();
    }
}

int Element::childIndex(Element *child)
{
    int index = 0 ;
    foreach(Element * aChild, childItems) {
        if(aChild == child) {
            return index ;
        }
        index ++ ;
    }
    return -1;
}


void Element::setText(const QString &data)
{
    text = data ;
}

void Element::setTextOfTextNode(const QString &data, const bool isCData)
{
    text = data ;
    _isCData = isCData ;
}

const QString &Element::getComment()
{
    return text ;
}

void Element::setComment(const QString & comment)
{
    text = comment ;
}

void Element::addTextNode(TextChunk *text)
{
    textNodes.append(text);
}

void Element::clearTextNodes()
{
    QVectorIterator<TextChunk*> tt(textNodes);
    while(tt.hasNext()) {
        delete tt.next();
    }
    textNodes.clear();
}

const QString &Element::getPITarget()
{
    return text;
}

const QString Element::getPIData()
{
    return tag();
}

void Element::setPITarget(const QString & target)
{
    text = target ;
}

void Element::setPIData(const QString & data)
{
    if(NULL != parentRule) {
        _tag = parentRule->addNameToPool(data);
    } else {
        _tag = data ;
    }
}

void Element::unhilite()
{
    QTreeWidgetItem *theUi = getUI();
    if(NULL != theUi) {
        theUi->setBackgroundColor(0, QColor(0xFF, 0xFF, 0xFF, 0));
    }
}

void Element::hilite()
{
    QTreeWidgetItem *theUi = getUI();
    if(NULL != theUi) {
        theUi->setBackgroundColor(0, QColor(0xFF, 0xC0, 0x40));
    }
    parentRule->addHilite(this);
}

bool Element::isMixedContent()
{
    foreach(Element * value, childItems) {
        if(value->type == ET_TEXT) {
            return true ;
        }
    }
    return false;
}

QVector<Element*> & Element::getParentChildren()
{
    if(NULL == parentElement) {
        return parentRule->getItems();
    } else {
        return parentElement->childItems;
    }
}

QTreeWidgetItem *Element::findNextBrother(QVector<Element*> &items)
{
    int indexOfSelf = items.indexOf(this);
    int size = items.size();
    if((indexOfSelf >= 0) && (indexOfSelf < (size - 1))) {
        return items.at(indexOfSelf + 1)->ui ;
    }
    return NULL;
}

QTreeWidgetItem *Element::goToNextBrother()
{
    return findNextBrother(getParentChildren());
}

QTreeWidgetItem *Element::findPreviousBrother(QVector<Element*> &items)
{
    int indexOfSelf = items.indexOf(this);
    if(indexOfSelf > 0) {
        return items.at(indexOfSelf - 1)->ui ;
    }
    return NULL;
}

QTreeWidgetItem *Element::goToPreviousBrother()
{
    return findPreviousBrother(getParentChildren());
}

void Element::isFirstOrLastChild(bool &isFirst, bool &isLast)
{
    findFirstAndLast(getParentChildren(), isFirst, isLast);
}

void Element::findFirstAndLast(QVector<Element*> &items, bool &isFirst, bool &isLast)
{
    int indexOfSelf = items.indexOf(this);
    if(indexOfSelf > 0) {
        isFirst = false;
    } else {
        isFirst = true ;
    }
    int size = items.size();
    if(indexOfSelf < (size - 1)) {
        isLast = false;
    } else {
        isLast = true ;
    }
}

void Element::refreshUI()
{
    parentRule->redisplayElement(this);
}

void Element::displayWithPaintInfo(PaintInfo *paintInfo)
{
    if(NULL != ui) {
        display(ui, paintInfo);
    }
}

QString Element::formatRowNumber(const int index)
{
    QString str = QString::number(index, 16);
    return str.rightJustified(6, '0', true);
}

QString Element::topLevelItemIndex(QTreeWidgetItem *topLevelItem)
{
    int position = ui->treeWidget()->indexOfTopLevelItem(topLevelItem);
    return formatRowNumber(position);
}

/**
  * swaps a child with a new element ans return its position
  */
int Element::setItemLike(Element *newElement, Element* oldElement)
{
    int pos = childItems.indexOf(oldElement);
    if(pos >= 0) {
        childItems.insert(pos, newElement);
    }
    return pos;
}

QString Element::getRow()
{
    QString result = "";
    if(NULL != ui) {
        QTreeWidgetItem *parent = ui->parent();
        if(NULL == parent) {
            return topLevelItemIndex(ui);
        }
        QTreeWidgetItem *child = ui;
        while(NULL != parent) {
            result = formatRowNumber(parent->indexOfChild(child)) + result;
            child = parent;
            parent = parent->parent();
        }
        result = topLevelItemIndex(child) + result;
    }
    return result;
}

void Element::hideBrothers()
{
    int indexOfThis = -1 ;
    int size = 0;
    QVector<Element*> *elems = NULL ;
    if(NULL == parentElement) {
        QVector<Element*> *items = parentRule->getChildItems();
        size = items->size();
        indexOfThis = items->indexOf(this);
        elems = items;
    } else {
        indexOfThis = parentElement->childItems.indexOf(this);
        size = parentElement->childItems.size();
        elems = this->parentElement->getChildItems();
    }
    if((indexOfThis < 0) || (indexOfThis >= size))
        return ;
    int previousIndex = indexOfThis - 1 ;
    int nextIndex = indexOfThis + 1 ;

    QVectorIterator<Element*> it(*elems);
    PaintInfo *paintInfo = parentRule->getPaintInfo();
    int index = 0 ;
    while(it.hasNext()) {
        Element *elm = it.next();
        if(indexOfThis == index) {
            elm->setVisibilityStateShow();
        } else if((index == previousIndex) || (index == nextIndex)) {
            elm->setVisibilityStateEllipsis();
            if(elm->ui->isExpanded()) {
                elm->ui->setExpanded(false);
            }
            QVectorIterator<Element*> it(elm->childItems);
            while(it.hasNext()) {
                Element *el = it.next();
                if(NULL != el->ui) {
                    el->ui->setHidden(true);
                }
            }
        } else {
            elm->setVisibilityStateHidden();
            elm->ui->setHidden(true);
        }
        elm->displayWithPaintInfo(paintInfo);
        index ++ ;
    }
}

void Element::showBrothers()
{
    int indexOfThis = -1 ;
    int size = 0;
    QVector<Element*> *elems = NULL ;
    if(NULL == parentElement) {
        QVector<Element*> *items = parentRule->getChildItems();
        size = items->size();
        indexOfThis = items->indexOf(this);
        elems = items;
    } else {
        indexOfThis = parentElement->childItems.indexOf(this);
        size = parentElement->childItems.size();
        elems = this->parentElement->getChildItems();
    }
    if((indexOfThis < 0) || (indexOfThis >= size))
        return ;

    QVectorIterator<Element*> it(*elems);
    PaintInfo *paintInfo = parentRule->getPaintInfo();
    int index = 0 ;
    while(it.hasNext()) {
        Element *elm = it.next();
        if(elm->isVisibilityStateEllipsis()) {
            QVectorIterator<Element*> it(elm->childItems);
            while(it.hasNext()) {
                Element *el = it.next();
                if(NULL != el->ui) {
                    el->ui->setHidden(false);
                }
            }
        }
        elm->resetVisibilityState();
        if(elm->ui->isHidden()) {
            elm->ui->setHidden(false);
        }
        elm->displayWithPaintInfo(paintInfo);
        index ++ ;
    }
}

bool Element::isNormalViewState()
{
    return visibilityState == EVN_NORMAL ;
}

void Element::resetVisibilityState()
{
    visibilityState = EVN_NORMAL ;
}

void Element::setVisibilityStateEllipsis()
{
    visibilityState = EVN_ELLIPSIS ;
}

void Element::setVisibilityStateHidden()
{
    visibilityState = EVN_HIDDEN;
}

bool Element::isHidden()
{
    return (visibilityState == EVN_HIDDEN);
}

void Element::setVisibilityStateShow()
{
    visibilityState = EVN_SHOW ;
}

bool Element::isVisibilityStateEllipsis()
{
    return visibilityState == EVN_ELLIPSIS ;
}

QList<Attribute*>Element::getAttributesList()
{
    QList<Attribute*>attrList;
    attrList = attributes.toList();
    return attrList ;
}

Attribute* Element::getAttribute(const QString &attributeName)
{
    foreach(Attribute * attribute, attributes) {
        if(attribute->name == attributeName) {
            return attribute;
        }
    }
    return NULL ;
}

QString Element::getAttributeValue(const QString &attributeName)
{
    foreach(Attribute * attribute, attributes) {
        if(attribute->name == attributeName) {
            return attribute->value;
        }
    }
    return "" ;
}


void Element::setZoomFactor(const int zoom)
{
    if(NULL != attrFont) {
        attrFont->setPointSize(VStyle::getZoomFontSize(defaultAttrFonts, zoom));
    }
    int newSize = VStyle::getZoomFontSize(fixedSizeAttrFonts, zoom) ;
    if(newSize > 0) {
        fixedWidthFont.setPointSize(VStyle::getZoomFontSize(fixedSizeAttrFonts, zoom));
    }
}


int Element::textSize()
{
    int size = 0;
    QVectorIterator<TextChunk*> tt(textNodes);
    while(tt.hasNext()) {
        TextChunk   *tx = tt.next();
        size += tx->text.length();
    }
    return size;
}

void Element::recalcSize(const bool isRecursive)
{
    int sizeOfData ;
    selfInfo.reset();
    if(isRecursive) {
        childrenInfo.reset();
    }

    selfInfo.numElements = childItems.size();

    switch(type) {
    default:
    case ET_ELEMENT: {
        // appends itself
        sizeOfData = textSize();
        selfInfo.totalSize += tag().length() * 2 + 5;
        selfInfo.totalSize += sizeOfData ;

        //itera sulla lista e prendi i valori dalla chiabe
        QVectorIterator<Attribute*>  attrs(attributes);
        while(attrs.hasNext()) {
            Attribute* attribute = attrs.next();
            selfInfo.totalSize += attribute->name.length() * 2 + 5;
            selfInfo.totalSize += attribute->value.length() ;
        }

        //append children
        if(isRecursive) {
            foreach(Element * value, childItems) {
                value->recalcSize(true);
                collectChildInfo(value, true);
            }
        }
    }
    break;

    case ET_PROCESSING_INSTRUCTION: {
        sizeOfData = getPITarget().length() + getPIData().length();
        selfInfo.totalSize = 5 + sizeOfData ;
    }
    break;

    case ET_COMMENT: {
        sizeOfData = getComment().length();
        selfInfo.totalSize = 7 + sizeOfData ;
    }
    break;
    case ET_TEXT:
        sizeOfData = text.length();
        selfInfo.totalSize = sizeOfData ;
        break;
    }
}

void Element::recalcChildSize()
{
    childrenInfo.reset();
    selfInfo.numElements = childItems.size();

    switch(type) {
    default:
        break;
    case ET_ELEMENT: {
        foreach(Element * child, childItems) {
            collectChildInfo(child, true);
        }
    }
    break;
    }
}


void Element::propagateChildInfoChange()
{
    if(parentRule->collectSizeData()) {
        if(NULL != parentElement) {
            parentElement->recalcChildSize();
            parentElement->displayWithPaintInfo(parentRule->getPaintInfo());
            parentElement->propagateChildInfoChange();
        }
    }
}

void Element::collectChildInfo(Element *child, const bool isAdd)
{
    if(isAdd) {
        childrenInfo.numElements += child->selfInfo.numElements;
        childrenInfo.totalSize += child->selfInfo.totalSize;
        childrenInfo.numElements += child->childrenInfo.numElements;
        childrenInfo.totalSize += child->childrenInfo.totalSize;
    } else {
        childrenInfo.numElements -= child->selfInfo.numElements;
        childrenInfo.totalSize -= child->selfInfo.totalSize;
        childrenInfo.numElements -= child->childrenInfo.numElements;
        childrenInfo.totalSize -= child->childrenInfo.totalSize;
    }
}


void Element::removeChildInfo(Element *childToRemove)
{
    if((parentRule != NULL) && parentRule->collectSizeData()) {
        collectChildInfo(childToRemove, false);
        propagateChildInfoChange();
    }
}

void Element::addChildInfo(Element *child)
{
    if((parentRule != NULL) && parentRule->collectSizeData()) {
        collectChildInfo(child, true);
        propagateChildInfoChange();
    }
}


void Element::updateSizeInfo()
{
    if((parentRule != NULL) && parentRule->collectSizeData()) {
        recalcSize(false);
        propagateChildInfoChange();
    }
}

void Element::incrementSizeInfo(const int size)
{
    selfInfo.totalSize += size ;
}

QString Element::tag()
{
    return _tag;
}

void Element::setTag(const QString &newTag)
{
    _tag = newTag ;
}

void Element::namespaceOfElement(QString &elPrefix, QString &elLocalName)
{
    QStringList ns = _tag.split(':');
    if(ns.length() < 2) {
        elPrefix = "" ;
        elLocalName = _tag;
    } else {
        elPrefix = ns.at(0);
        elLocalName = ns.at(1);
    }
}

QString Element::transformToText()
{
    QDomDocument    document;
    QString text ;
    if(generateDom(document, document)) {
        text = document.toString(4);
    }
    return text ;
}

int Element::indexOfSelfAsChild()
{
    if(NULL != parentElement) {
        return parentElement->childItems.indexOf(this);
    } else {
        if(NULL != parentRule) {
            return parentRule->indexOfTopLevelItem(this);
        } else {
            return -1 ;
        }
    }
}

Element *Element::nextSibling()
{
    int nextChildIndex = indexOfSelfAsChild() + 1;
    if(NULL != parentElement) {
        return parentElement->getChildAt(nextChildIndex);
    } else {
        if(NULL != parentRule) {
            return parentRule->topElement(nextChildIndex);
        } else {
            return NULL ;
        }
    }
}

Element *Element::previousSibling()
{
    int nextChildIndex = indexOfSelfAsChild() - 1;
    if(NULL != parentElement) {
        return parentElement->getChildAt(nextChildIndex);
    } else {
        if(NULL != parentRule) {
            return parentRule->topElement(nextChildIndex);
        } else {
            return NULL ;
        }
    }
}

Element *Element::previousSiblingRecursive()
{
    int nextChildIndex = indexOfSelfAsChild() - 1;
    if(NULL != parentElement) {
        Element *chl = parentElement->getChildAt(nextChildIndex);
        if(NULL != chl) {
            chl = chl->lastChildRecursiveOrThis();
        }
        return chl ;
    } else {
        if(NULL != parentRule) {
            Element *prevTop = parentRule->topElement(nextChildIndex);
            if(NULL != prevTop) {
                return prevTop->lastChildRecursiveOrThis();
            }
            return NULL ;
        } else {
            return NULL ;
        }
    }
}

bool Element::areChildrenLeavesHidden(QTreeWidgetItem *twi)
{
    if(NULL == twi) {
        return false;
    }
    QVariant res = twi->data(0, ShowDataRole);
    return res.toBool();
    //return twi->data(0, ShowDataRole).toBool();
}

void Element::setChildrenLeavesHiddenState(QTreeWidgetItem *twi, const bool newState)
{
    if(NULL != twi) {
        QVariant vState(newState);
        twi->setData(0, ShowDataRole, vState);
        //twi->setData(0, ShowDataRole, QVariant(newState));
    }
}

bool Element::hasElementInChildren()
{
    foreach(Element * child, getItems()) {
        if(child->getType() == Element::ET_ELEMENT) {
            return true ;
        }
    }
    return false;
}

bool Element::isALeaf()
{
    return !hasElementInChildren();
}

void Element::hideChildrenLeaves()
{
    setChildrenLeavesHiddenState(ui, true);
    if(!hasChildren()) {
        if(NULL != ui) {
            ui->setHidden(true);
        }
    } else {
        displayWithPaintInfo(parentRule->getPaintInfo());
    }
    foreach(Element * theChild, childItems) {
        theChild->hideChildrenLeaves();
    }
}

void Element::showChildrenLeaves()
{
    setChildrenLeavesHiddenState(ui, false);
    if(!hasChildren()) {
        if(NULL != ui) {
            ui->setHidden(false);
        }
    } else {
        displayWithPaintInfo(parentRule->getPaintInfo());
    }
    foreach(Element * theChild, childItems) {
        theChild->showChildrenLeaves();
    }
}

//returns the position of the element
QList<int> Element::indexPath()
{
    QList<int> list ;

    Element *parentE = parentElement;
    Element *target = this ;
    while(parentE != NULL) {
        list.prepend(parentE->childIndex(target));
        target = parentE ;
        parentE = parentE->parentElement ;
    }
    list.prepend(parentRule->getChildItems()->indexOf(target));

    return list ;
}

QStringList Element::path()
{
    QStringList list ;

    Element *parentE = this;
    while(parentE != NULL) {
        list.prepend(parentE->tag());
        parentE = parentE->parentElement ;
    }
    return list ;
}

QStringList Element::parentPath()
{
    QStringList list ;

    Element *parentE = parentElement;
    while(parentE != NULL) {
        list.prepend(parentE->tag());
        parentE = parentE->parentElement ;
    }
    return list ;
}

bool Element::copyTextNodesToTarget(Element *target)
{
    if(NULL != target) {
        copyHeaderAndDirectNodes(*target);
        QList<Element*> elementsToRemove;
        QList<Element*> elementsToMaintain;

        foreach(Element * e, target->childItems) {
            if(e->getType() == Element::ET_TEXT) {
                elementsToRemove.append(e);
            } else {
                elementsToMaintain.append(e);
            }
        }
        foreach(Element * e, elementsToRemove) {
            e->autoDelete(true);
            // delete it from the user interface too
        }
        // now the second list has elements only, no text nodes.
        int index = 0 ; // This is the index of the second list.
        int sourceIndex = 0 ; // This is the index of the second list.

        foreach(Element * e, childItems) {
            if(e->getType() == Element::ET_TEXT) {

                Element *textElement = new Element(target->getParentRule(), Element::ET_TEXT, target);
                textElement->setTextOfTextNode(e->text, e->isCDATA()) ;
                textElement->markEdited();
                target->getItems().insert(sourceIndex, textElement);
                textElement->caricaFigli(target->getUI()->treeWidget(), target->getUI(), target->getParentRule()->getPaintInfo(), true, sourceIndex);
            }
            index ++;
        } // foreach
        target->display(target->getUI(), target->getParentRule()->getPaintInfo());
        // Now, check if the elements correspond between the two implementations
        int numChildrenSource = getChildItemsCount();
        int numChildrenTarget = target->getChildItemsCount();
        if(numChildrenSource  != numChildrenTarget) {
            return false;
        }

        // Now, make sure that the new list is the current one for the target element.
        for(int i = 0 ; i < numChildrenSource ; i ++) {
            Element * sourceElement = childItems.at(i) ;
            Element * targetElement = target->childItems.at(i) ;
            // check for synchronization
            if((sourceElement->getType() != targetElement->getType())
                    || (sourceElement->tag() != targetElement->tag())
                    || (sourceElement->text != targetElement->text)) {
                return false;
            }
        } // for children
        return true;
    }
    return false;
}

// filters all non element children
QVector<Element*> Element::getOnlyElementChildren()
{
    QVector<Element*> result;
    foreach(Element * child, childItems) {
        if(child->getType() == ET_ELEMENT) {
            result.append(child);
        }
    }
    return result;
}


void Element::copyPathToClipboard()
{
    if(getType() != ET_ELEMENT) {
        return ;
    }
    QString path ;
    Element *aParent = this;
    while(NULL != aParent) {
        path =  "/" + aParent->tag() + path;
        aParent = aParent->parent();
    }
    QClipboard *clipboard = QApplication::clipboard();
    if(NULL != clipboard) {
        clipboard->setText(path);
    }
}


QString Element::makeCSVFromSiblings()
{
    Element *theParent = parent() ;

    if(NULL == theParent) {
        QVector<Element*> list ;
        list.append(this);
        return insertAttributesListInCSV(list);
    } else {
        return insertAttributesListInCSV(theParent->childItems);
    }
}

void Element::insertAttributesInMap(QMap<QString, QString> &attributesNameMap)
{
    foreach(Attribute * attribute, attributes) {
        if(!attributesNameMap.contains(attribute->name)) {
            attributesNameMap.insert(attribute->name, _tag);
        }
    }
}


void Element::insertAttributesValuesInMap(QHash<QString, QString> &valuesMap)
{
    foreach(Attribute * attribute, attributes) {
        valuesMap.insert(attribute->name, attribute->value);
    }
}

void Element::insertAttributesInMap(QHash<QString, Attribute*> &valuesMap)
{
    foreach(Attribute * attribute, attributes) {
        valuesMap.insert(attribute->name, attribute);
    }
}

QString Element::insertAttributesInCSV(QMap<QString, QString> &attributesNameMap)
{
    QHash<QString, QString> valuesMap;
    insertAttributesValuesInMap(valuesMap);

    QString result = "" ;
    foreach(QString key, attributesNameMap.keys()) {
        QString value = valuesMap.value(key);
        if(!valuesMap.contains(key)) {
            value = "";
        } else {
            value = valuesMap.value(key);
        }
        result += Utils::valueStringCSV(value, false);
    }
    result += "\n";
    return result ;
}

QString Element::insertAttributesListInCSV(QVector<Element*> &elements)
{
    QMap<QString, QString> attributesNameMap;
    QString result = "";
    // 1- get the titles
    foreach(Element * element, elements) {
        if(element->getType() == ET_ELEMENT) {
            element->insertAttributesInMap(attributesNameMap);
        }
    }
    // 2 - build the titles row
    result += tr("element");
    foreach(QString title, attributesNameMap.keys()) {
        result += Utils::separatorStringCSV();
        result += title ;
    }
    result += "\n";

    // 3- get the data
    foreach(Element * element, elements) {
        if(element->getType() == ET_ELEMENT) {
            result += Utils::valueStringCSV(element->tag(), true);
            result += element->insertAttributesInCSV(attributesNameMap);
        }
    }
    return result;
}

void Element::addElementChildrenInList(QList<Element*> &childrenList)
{
    foreach(Element * child, childItems) {
        childrenList.append(child);
    }
}

bool Element::pasteAttributes(QList<Attribute*> &newAttributes, UndoPasteAttributesCommand *undoCommand, bool &isModified)
{
    bool isOk = true ;
    isModified = false;
    QHash<QString, Attribute*> attributesMap;
    insertAttributesInMap(attributesMap);
    if((NULL != undoCommand) && !undoCommand->setOriginalElement(this)) {
        isOk = false;
    }

    foreach(Attribute * newAttribute, newAttributes) {
        if(!attributesMap.contains(newAttribute->name)) {
            attributes.append(newAttribute->clone());
            isModified = true ;
        } else {
            Attribute *oldAttribute = attributesMap[newAttribute->name];
            if(oldAttribute->value != newAttribute->value) {
                oldAttribute->value = newAttribute->value;
                isModified = true ;
            }
        }
    }
    if(isModified) {
        if((NULL != undoCommand) && !undoCommand->setModifiedElement(this)) {
            isOk = false;
        }
    }
    return isOk ;
}

bool Element::pasteClearAttributes(QList<Attribute*> &newAttributes, UndoPasteAttributesCommand *undoCommand, bool &isModified)
{
    bool isOk = true ;
    isModified = false;
    QHash<QString, Attribute*> attributesMap;
    insertAttributesInMap(attributesMap);
    if((NULL != undoCommand) && !undoCommand->setOriginalElement(this)) {
        isOk = false;
    }
    QList<Attribute*> deleteList;

    foreach(Attribute * newAttribute, newAttributes) {
        bool isEmpty = newAttribute->value.isEmpty();

        if(!attributesMap.contains(newAttribute->name)) {
            if(!isEmpty) {
                attributes.append(newAttribute->clone());
                isModified = true ;
            }
        } else {
            Attribute *oldAttribute = attributesMap[newAttribute->name];
            if(isEmpty) {
                deleteList.append(oldAttribute);
            } else {
                if(oldAttribute->value != newAttribute->value) {
                    oldAttribute->value = newAttribute->value;
                    isModified = true ;
                }
            }
        }
    }

    foreach(Attribute * attributeToDelete, deleteList) {
        if(NULL != attributeToDelete) {
            int size = attributes.size();
            for(int i = 0 ; i < size ; i ++) {
                Attribute * attribute = attributes.at(i);
                if(attribute == attributeToDelete) {
                    attributes.remove(i);
                    delete attributeToDelete;
                    isModified = true ;
                    break;
                }
            }
        }
    }

    if(isModified) {
        if((NULL != undoCommand) && !undoCommand->setModifiedElement(this)) {
            isOk = false;
        }
    }
    return isOk ;
}


bool Element::setAttributes(QList<Attribute*> newAttributes)
{
    bool isOk = true ;
    clearAttributes();
    foreach(Attribute * newAttribute, newAttributes) {
        Attribute * attribute = newAttribute->clone();
        if(NULL != attribute) {
            attributes.append(newAttribute->clone());
        } else {
            isOk = false ;
        }
    }
    return isOk ;
}

bool Element::setAttribute(const QString &name, const QString &value)
{
    bool found = false;
    foreach(Attribute * attribute, attributes) {
        if(attribute->name == name) {
            found = true ;
            attribute->value = value ;
            break;
        }
    }
    if(!found) {
        addAttribute(name, value);
    }
    return found ;
}

bool Element::removeAttribute(const QString &name)
{
    foreach(Attribute * attribute, attributes) {
        if(attribute->name == name) {
            int index = attributes.indexOf(attribute);
            if(index >= 0) {
                attributes.remove(index);
                return true;
            }
        }
    }
    return false ;
}

QString Element::localName()
{
    QString aTag = tag();
    if(isElement()) {
        int indexOfColon = aTag.indexOf(":");
        if(indexOfColon >= 0) {
            aTag = aTag.mid(indexOfColon + 1);
        }
    }
    return aTag ;
}

QString Element::getStringRepresentationForClipboard()
{
    QString clipboardText ;
    QDomDocument    document;
    if(generateDom(document, document)) {
        clipboardText = document.toString(4);
    }
    return clipboardText;
}

void Element::moveChildrenTo(Element *theNewElement)
{
    foreach(Element * child, childItems) {
        theNewElement->addChild(child);
        child->parentElement = theNewElement ;
    }
    foreach(TextChunk * text, textNodes) {
        theNewElement->addTextNode(text);
    }
    childItems.clear();
    textNodes.clear();
}

void Element::moveUIChildrenTo(Element *theNewElement)
{
    if(NULL != ui) {
        QList<QTreeWidgetItem*> items = ui->takeChildren();
        ui->addChild(theNewElement->ui);
        theNewElement->ui->addChildren(items);
    }
}

bool Element::parentIsRoot()
{
    if(NULL != parentElement) {
        if(NULL == parentElement->parent()) {
            return true ;
        }
    }
    return false;
}


bool Element::siblingsElementsMultGTValue(const int value, const bool isTextFail)
{
    int elements = 0;
    QVector<Element*> &parentVector = getParentChildren();
    foreach(Element * el, parentVector) {
        if(el->isElement()) {
            elements++;
            if(elements > value) {
                return true ;
            }
        } else if((el->getType() == ET_TEXT) && (isTextFail)) {
            return true;
        }
    }
    return false ;
}

bool Element::canRemoveParent()
{
    if(NULL == parent()) {
        return false;
    }
    if(parentIsRoot()) {
        // avoid double root
        Element *root = parentRule->root();
        if((NULL != root) && (root != parent())) {
            if(siblingsElementsMultGTValue(0)) {
                return false;
            }
        } else if((NULL == root) || ((NULL != root) && (root == parent()))) {
            if(siblingsElementsMultGTValue(1, true)) {
                return false;
            }
        }
        return true;
    } else {
        return true ;
    }
}

bool Element::canInsertParent()
{
    // always
    if(NULL != parent()) {
        return true;
    }
    // parent null
    Element *root = parentRule->root();
    if(root == this) {
        return true;
    }
    // E root, but not this
    if(NULL != root) {
        return false;
    }
    // no root, but not p.i
    if((this->getType() == ET_PROCESSING_INSTRUCTION) && (this->getPITarget() == "xml")) {
        return false;
    }
    return true ;
}

void Element::unexpandRecursive()
{
    if((NULL != ui) && ui->isExpanded()) {
        ui->setExpanded(false);
    }
    foreach(Element * child, childItems) {
        child->unexpandRecursive();
    }
}

void Element::expandRecursive()
{
    foreach(Element * child, childItems) {
        child->expandRecursive();
    }
    if((NULL != ui) && !ui->isExpanded()) {
        ui->setExpanded(true);
    }
}

bool Element::removeChild(Element *toDelete)
{
    int index = 0;
    foreach(Element * child, childItems) {
        if(child == toDelete) {
            childItems.remove(index);
            delete child;
            return true;
        }
        index ++ ;
    }
    return false;
}


void Element::anonymize(AnonAlg *alg)
{
    switch(type) {
    case ET_ELEMENT:
        anonymizeElement(alg);
        break;
    case ET_PROCESSING_INSTRUCTION:
        break;
    case ET_COMMENT:
        break;
    case ET_TEXT:
        anonymizeText(alg);
    default:
        break;
    }
    foreach(Element * child, childItems) {
        child->anonymize(alg);
    }
}

void Element::anonymizeElement(AnonAlg *alg)
{
    QVectorIterator<TextChunk*> tt(textNodes);
    while(tt.hasNext()) {
        TextChunk *tx = tt.next();
        tx->text = alg->processText(tx->text);
    }
    foreach(Attribute * attribute, getAttributesList()) {
        attribute->value = alg->processText(attribute->value);
    }
}

void Element::anonymizeText(AnonAlg *alg)
{
    text = alg->processText(text);
}


Element *Element::firstChildRecursiveOrThis()
{
    if(childItems.isEmpty()) {
        return this ;
    }
    return childItems.first()->firstChildRecursiveOrThis();
}

Element *Element::lastChildRecursiveOrThis()
{
    if(childItems.isEmpty()) {
        return this ;
    }
    return childItems.last()->lastChildRecursiveOrThis();
}

Element *Element::lastChildRecursive()
{
    if(childItems.isEmpty()) {
        return NULL ;
    }
    return childItems.last()->lastChildRecursiveOrThis();
}


void Element::allNamespaces(QHash<QString, QSet<QString> > &nameSpacesMap)
{
    foreach(Element * child, childItems) {
        child->allNamespaces(nameSpacesMap);
    }

    foreach(Attribute * attribute, attributes) {
        if(XmlUtils::isDeclaringNS(attribute->name)) {
            QString prefix ;
            XmlUtils::getNsPrefix(attribute->name, prefix);
            if(!nameSpacesMap.contains(attribute->value)) {
                QSet<QString> prefixes ;
                nameSpacesMap.insert(attribute->value, prefixes);
            }
            nameSpacesMap[attribute->value] += prefix;
        }
    }
} // allNamespaces()

/*!
 * \brief setOrClearAttribute: can delete the attribute (clear) or set it to a specific value
 * \param isRemoveAttribute
 * \param attrName
 * \param value
 * \return true if the element has been modified
 */
bool Element::setOrClearAttribute(const bool isRemoveAttribute, const QString &attrName, const QString &value)
{
    if(isRemoveAttribute) {
        return removeAttribute(attrName);
    }
    Attribute *attrib = getAttribute(attrName);
    if(NULL != attrib) {
        if(attrib->value == value) {
            return false ;
        }
    }
    setAttribute(attrName, value);
    return true ;
}

//----------------------------------------------------------------

Attribute::Attribute()
{
}

Attribute::Attribute(const QString & newName, const QString & newValue)
{
    name = newName;
    value = newValue ;
}


Attribute::~Attribute()
{
}


bool Attribute::copyAttributeListTo(QList<Attribute*> &sourceList, QList<Attribute*> &destList)
{
    foreach(Attribute * attribute, sourceList) {
        Attribute *newAttribute = new Attribute();
        if(NULL == newAttribute) {
            return false;
        }
        newAttribute->name = attribute->name;
        newAttribute->value = attribute->value;
        destList.append(newAttribute);
    }
    return true ;
}

void Attribute::emptyList(QList<Attribute*> &sourceList)
{
    foreach(Attribute * attribute, sourceList) {
        delete attribute ;
    }
    sourceList.clear();
}


Attribute *Attribute::clone()
{
    Attribute *newAttribute = new Attribute();
    newAttribute->name = name;
    newAttribute->value = value;
    return newAttribute ;
}

//-------------

QString Element::toString()
{
    QString s = "element " ;
    s += QString("type %1, tag:'%2'\n").arg(getType()).arg(_tag);
    s += QString("  attributes: %1").arg(attributes.size()) ;
    foreach(Attribute * a, attributes) {
        s += QString("   %1='%2'\n").arg(a->name).arg(a->value);
    }
    foreach(TextChunk * t, textNodes) {
        s += QString(" text='%1'\n").arg(t->text);
    }
    s += "\n\n";
    return s;
}

bool Element::compareToElement(Element *other, QString &msg)
{
    if(getType() != other->getType()) {
        msg = QString("types this=%1, other=%2").arg(getType()).arg(other->getType());
        return false;
    }
    if(tag() != other->tag()) {
        msg = QString("tag this='%1', other='%2'").arg(tag()).arg(other->tag());
        return false;
    }
    if(attributes.size() != other->attributes.size()) {
        msg = QString("Attributes# this='%1', other='%2'").arg(attributes.size()).arg(other->attributes.size());
        return false;
    }

    QHash<QString, QString> a1;
    foreach(Attribute * a, attributes) {
        a1.insert(a->name, a->value);
    }

    foreach(Attribute * a, other->attributes) {
        if(!a1.contains(a->name)) {
            msg = QString("Attribute missing in 1 # name='%1', value='%2'").arg(a->name).arg(a->value);
            return false;
        }
        QString val1 = a1[a->name];
        if(val1 != a->value) {
            msg = QString("Attribute differs name=%1 val1='%2', val2='%3'").arg(a->name).arg(a->value).arg(val1);
            return false;
        }
    }

    QSet<QString> t1;
    foreach(TextChunk * t, textNodes) {
        t1.insert(t->text);
    }

    foreach(TextChunk * t, textNodes) {
        if(!t1.contains(t->text)) {
            msg = QString("Text missing in 1 ='%1'").arg(t->text);
            return false;
        }
    }
    return true;
}

// update view
void Element::forceUpdateGui(const bool forceLayout)
{
    QTreeWidgetItem *item = getUI();
    if(NULL != item) {
        QRect bounds = item->treeWidget()->visualItemRect(item);
        QModelIndex index = item->treeWidget()->indexAt(QPoint(bounds.left() + bounds.width() / 2, bounds.top() + bounds.height() / 2));
        item->treeWidget()->update(index);
        if(forceLayout) {
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
            item->treeWidget()->doItemsLayout();
#else
            QAbstractItemModel *model = item->treeWidget()->model();
            emit model->layoutAboutToBeChanged();
            emit model->layoutChanged();
#endif
        }
    }
}
