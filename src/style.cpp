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

#include "style.h"
#include <QDir>
#include "stylepersistence.h"
#include "utils.h"
#include "element.h"

//----------------------------------------------------------
StyleCalc::StyleCalc(const QString &newTp)
{
    _tp = newTp ;
}

StyleCalc::~StyleCalc()
{
}

QString StyleCalc::tp()
{
    return _tp;
}

//------------------------

StyleRule::StyleRule() : StyleCalc("r")
{
    _isElement = false ;
    _type = CT_STRING ;
    _op = EQ ;
    _numReady = false ;
    _caseSensitive = true ;
    _cfrValNum = 0 ;
}

StyleRule::~StyleRule()
{
}

void StyleRule::setAxis(const QString &newAxis)
{
    _axis = newAxis;
}

void StyleRule::setEntity(const bool newIsElement)
{
    _isElement = newIsElement ;
}

bool StyleRule::isEntityElement()
{
    return _isElement ;
}

void StyleRule::setCaseSensitive(const bool newCaseSensitive)
{
    _caseSensitive = newCaseSensitive;
}

bool StyleRule::isCaseSensitive()
{
    return _caseSensitive ;
}

void StyleRule::setName(const QString& newName)
{
    _name = newName ;
}

QString StyleRule::name()
{
    return _name ;
}

#define XOP(x) if( newOp == #x) { _op = x ; return;}

void StyleRule::setOp(const QString& newOp)
{
    /*if( op == "EQ") {
        op = EQ ;
    }*/
    XOP(EQ);
    XOP(NE);
    XOP(EX);
    XOP(NX);
    XOP(GT);
    XOP(LT);
    XOP(GE);
    XOP(LE);
    XOP(SS);
    XOP(ST);
    Utils::warning(QString(QObject::tr("Unknown operator in style: %1")).arg(newOp));
}


bool StyleRule::evaluate(Element *element)
{
    if(_isElement || (CT_POSITION == _type)) {
        return evaluateElement(element);
    } else {
        return evaluateAttribute(element);
    }
}

bool StyleRule::evaluateAttribute(Element *inputElement)
{
    Element *element = findElement(inputElement);
    if(NULL == element) {
        return false;
    }
    bool exists = false;
    QString value ;
    Attribute *attr = element->getAttribute(_name);
    if(NULL != attr) {
        exists = true;
        value = attr->value;
    }
    switch(_type) {
    case CT_STRING:
        return evalString(value, exists);
    case CT_NUM:
        return evalNum(value, exists);
    default:
        return false;
    }
}

Element *StyleRule::findElement(Element *startElement)
{
    if(_axis.isEmpty()) {
        return startElement;
    }
    if(_axis == "parent") {
        return startElement->parent();
    } else {
        QStringList list = _axis.split("/");
        Element *el = startElement;
        while(list.size() > 0) {
            QString pos = list.at(0);
            list.removeAt(0);
            if(pos == "..") {
                el = el->parent();
            } else {
                el = el->findFirstChildNamed(pos);
            }
            if(NULL == el) {
                return NULL ;
            }
        }
        return el;
    }
}

bool StyleRule::evaluateElement(Element *inputElement)
{
    Element *element = NULL ;
    switch(_type) {
    case CT_STRING:
    case CT_NUM:
        element = findElement(inputElement);
        break;
    case CT_POSITION:
        element = inputElement;
        break;
    default:
        return false;
    }
    if(NULL == element) {
        return false;
    }
    QString value = element->tag();
    switch(_type) {
    case CT_STRING:
        return evalString(value, true);
    case CT_NUM:
        return evalNum(value, true);
    case CT_POSITION:
        return evalPos(element);
    default:
        return false;
    }
}

bool StyleRule::evalString(const QString &value, const bool exists)
{
    Qt::CaseSensitivity cs = _caseSensitive ?  Qt::CaseSensitive : Qt::CaseInsensitive;
    bool result = false;
    switch(_op) {
    case EQ:
        result = (0 == QString::compare(value, _cfr, cs));
        break;
    case NE:
        result = (0 != QString::compare(value, _cfr, cs));
        break;
    case EX:
        return exists;
        break;
    case NX:
        return !exists;
        break;
    case GT:
        result = (QString::compare(value, _cfr, cs) > 0);
        break;
    case LT:
        result = (QString::compare(value, _cfr, cs) < 0);
        break;
    case GE:
        result = (QString::compare(value, _cfr, cs) >= 0);
        break;
    case LE:
        result = (QString::compare(value, _cfr, cs) <= 0);
        break;
    case SS:
        result = (value.indexOf(_cfr, 0, cs) >= 0);
        break;
    case ST:
        result = value.startsWith(_cfr, cs);
        break;
    default:
        Utils::warning(QString(QObject::tr("Unknown operator in style: %1")).arg(_op));
        break;
    }
    return result;
}

bool StyleRule::evalNum(const QString &value, const bool exists)
{
    if(!_numReady) {
        bool isOk = false;
        _cfrValNum = _cfr.toDouble(&isOk);
        if(!isOk) {
            _cfrValNum = 0 ;
        }
        _numReady = true ;
    }
    bool isOk = false;
    double valNum = value.toDouble(&isOk);
    if(!isOk) {
        valNum = 0 ;
    }

    bool result = false;
    switch(_op) {
    case EQ:
        result = (valNum == _cfrValNum);
        break;
    case NE:
        result = (valNum != _cfrValNum);
        break;
    case EX:
        return exists;
        break;
    case NX:
        return !exists;
        break;
    case GT:
        result = (valNum > _cfrValNum);
        break;
    case LT:
        result = (valNum < _cfrValNum);
        break;
    case GE:
        result = (valNum >= _cfrValNum);
        break;
    case LE:
        result = (valNum <= _cfrValNum);
        break;
    case SS:
        result = false;
        break;
    case ST:
        result = false;
        break;
    default:
        Utils::warning(QString(QObject::tr("Unknown operator in style: %1")).arg(_op));
        break;
    }
    return result;
}

bool StyleRule::evalPos(Element *element)
{
    int numChildren = 0 ;
    Element *parent = element->parent();
    if(NULL != parent) {
        numChildren = parent->getChildItemsCount();
    } else {
        numChildren = 1 ;
    }

    bool isOk = false;
    if(0 == QString::compare(_cfr, "first", Qt::CaseInsensitive)) {
        _cfrValNum  = 1 ; // Start from one.
    } else if(0 == QString::compare(_cfr, "last", Qt::CaseInsensitive)) {
        _cfrValNum = numChildren;
    } else {
        _cfrValNum = _cfr.toDouble(&isOk);
        if(!isOk) {
            _cfrValNum = 0 ;
        }
    }
    double valNum = element->indexOfSelfAsChild() + 1;

    bool result = false;
    switch(_op) {
    case EQ:
        result = (valNum == _cfrValNum);
        break;
    case NE:
        result = (valNum != _cfrValNum);
        break;
    case EX:
        return false;
        break;
    case NX:
        return false;
        break;
    case GT:
        result = (valNum > _cfrValNum);
        break;
    case LT:
        result = (valNum < _cfrValNum);
        break;
    case GE:
        result = (valNum >= _cfrValNum);
        break;
    case LE:
        result = (valNum <= _cfrValNum);
        break;
    case SS:
        result = false;
        break;
    case ST:
        result = false;
        break;
    default:
        Utils::warning(QString(QObject::tr("Unknown operator in style: %1")).arg(_op));
        break;
    }
    return result;
}

//-------------------------------

StyleRule::oper StyleRule::op()
{
    return _op ;
}


QString StyleRule::axis()
{
    return _axis ;
}

void StyleRule::setType(const QString &type)
{
    if(type == "n") {
        _type = CT_NUM;
    } else if(type == "p") {
        _type = CT_POSITION;
    } else {
        _type = CT_STRING;
    }
}

StyleRule::ECfrType StyleRule::type()
{
    return _type;
}

void StyleRule::setValue(const QString &value)
{
    _cfr = value ;
    _numReady = false;
}

QString StyleRule::value()
{
    return _cfr ;
}

//------------------------

StyleRuleSet::StyleRuleSet() : StyleCalc("s")
{
    _isAndConnector = true ;
}

StyleRuleSet::~StyleRuleSet()
{
    foreach(StyleCalc * sc, _calc) {
        delete sc;
    }
}

bool StyleRuleSet::evaluateOr(Element *element)
{
    bool result = false;
    foreach(StyleCalc * sc, _calc) {
        bool eval = sc->evaluate(element);
        if(eval) {
            result = true ;
            break;
        }
    }
    return result;
}

bool StyleRuleSet::evaluateAnd(Element *element)
{
    bool result = true ;
    bool oneRule = false;
    foreach(StyleCalc * sc, _calc) {
        oneRule = true ;
        bool eval = sc->evaluate(element);
        if(!eval) {
            result = false ;
            break;
        }
    }
    if(!oneRule) {
        return false;
    }
    return result ;
}

bool StyleRuleSet::evaluate(Element *element)
{
    if(_isAndConnector) {
        return evaluateAnd(element);
    } else {
        return evaluateOr(element);
    }
}


QString StyleRuleSet::idStyle()
{
    return _idStyle ;
}

void StyleRuleSet::setIdStyle(const QString &newIdStyle)
{
    _idStyle = newIdStyle ;
}

void StyleRuleSet::setConnectorAnd(const bool isAnd)
{
    _isAndConnector = isAnd ;
}

void StyleRuleSet::addRule(StyleCalc *newChild)
{
    _calc.append(newChild);
}

bool StyleRuleSet::isConnectorAnd()
{
    return _isAndConnector ;
}

QList<StyleCalc*> StyleRuleSet::children()
{
    return _calc ;
}
//----------------------------------------------------------


StyleEntry::StyleEntry(const QString &newId)
{
    inited = false;
    _font = NULL ;
    _fontMetrics = NULL ;
    _brush = NULL;
    _id = newId ;
    _useColor = false ;
    _useBackColor = false ;
    _bold = false;
    _italic = false;
    _fontSize = 0;
    _fontFamily = "";
    _originalFontSize = 0;
}

StyleEntry::~StyleEntry()
{
    deactivate();
}

const QString &StyleEntry::id()
{
    return _id ;
}

QColor StyleEntry::color()
{
    return _color;
}

QFont *StyleEntry::font()
{
    return _font ;
}

QFontMetrics * StyleEntry::fontMetrics()
{
    return _fontMetrics;
}

const QBrush &StyleEntry::brush()
{
    return *_brush;
}

QIcon StyleEntry::icon()
{
    return _icon ;
}

void StyleEntry::updateFontMetrics()
{
    if(NULL != _fontMetrics) {
        delete _fontMetrics;
        _fontMetrics = NULL ;
    }
    if(NULL != _font) {
        _fontMetrics = new QFontMetrics(*_font);
    }
}

void StyleEntry::setBold(const bool isBold)
{
    _bold = isBold ;
}

void StyleEntry::setItalic(const bool isItalic)
{
    _italic = isItalic ;
}

void StyleEntry::setFontSize(const int newSize)
{
    _fontSize = newSize ;
}

void StyleEntry::setFontFamily(const QString & newFamily)
{
    _fontFamily = newFamily;
}

void StyleEntry::setColor(const QString &newColor)
{
    if(!newColor.isEmpty()) {
        bool ok ;
        uint val = newColor.toUInt(&ok, 16) ;
        if(ok) {
            _color = QColor((val >> 16) & 0x00FF, (val >> 8) & 0x00FF, val & 0x00FF);
            _useColor = true;
        }
    }
}

void StyleEntry::setBackColor(const QString &newColor)
{
    if(!newColor.isEmpty()) {
        bool ok ;
        uint val = newColor.toUInt(&ok, 16) ;
        if(ok) {
            if(newColor.length() <= 6) {
                _backColor = QColor((val >> 16) & 0x00FF, (val >> 8) & 0x00FF, val & 0x00FF);
            } else {
                _backColor = QColor((val >> 16) & 0x00FF, (val >> 8) & 0x00FF, val & 0x00FF, (val >> 24) & 0x00FF);
            }
            _useBackColor = true;
        }
    }
}

void StyleEntry::setZoom(const int zoom)
{
    if(NULL != _font) {
        _font->setPointSize(VStyle::getZoomFontSize(_originalFontSize, zoom));
        updateFontMetrics();
    }
}

bool StyleEntry::isColor()
{
    return _useColor ;
}

QColor StyleEntry::backColor()
{
    return _backColor;
}

bool StyleEntry::useBackColor()
{
    return _useBackColor ;
}

bool StyleEntry::activate(const QFont &baseFont, const int zoom)
{
    bool isOk = true ;
    if(!inited) {
        if(!_fontFamily.isEmpty() && (_fontSize != 0)) {
            _font = new QFont(_fontFamily, _fontSize);
            if(NULL == _font) {
                isOk = false;
            }
        } else if(_bold || _italic) {
            _font = new QFont(baseFont);
            if(NULL == _font) {
                isOk = false;
            }
        }
        if(NULL != _font) {
            if(_italic) {
                _font->setItalic(true);
            }
            if(_bold) {
                _font->setBold(true);
            }
        }
        //---- brush
        if(_useColor) {
            _brush = new QBrush(_color);
            if(NULL == _brush)  {
                isOk = false;
            }
        }
        inited = true ;
        if(NULL != _font) {
            _originalFontSize = _font->pointSize();
        }
        updateFontMetrics();
    }
    setZoom(zoom);
    return isOk ;
}

void StyleEntry::deactivate()
{
    if(inited) {
        if(_font != NULL) {
            delete _font;
            _font = NULL;
        }
        updateFontMetrics();
        if(NULL != _brush) {
            delete _brush ;
            _brush = NULL ;
        }
        inited = false;
    }
}

#define ICON_ERROR  ":E:"
#define ICON_WARN  ":W:"
#define ICON_INFO  ":I:"

void StyleEntry::setIconName(const QString &newIconName)
{
    if(newIconName.isEmpty()) {
        return ;
    }
    _iconName = newIconName ;
    int internalIconName ;
    if(_iconName == ICON_ERROR) {
        internalIconName = QStyle::SP_MessageBoxCritical ;
    } else if(_iconName == ICON_WARN) {
        internalIconName = QStyle::SP_MessageBoxWarning ;
    } else if(_iconName == ICON_INFO) {
        internalIconName = QStyle::SP_MessageBoxInformation;
    } else {
        bool isOk = false;
        internalIconName = newIconName.toInt(&isOk);
        if(!isOk) {
            internalIconName = 0 ;
        }
    }
    QStyle *style = QApplication::style();
    _icon = style->standardIcon((QStyle::StandardPixmap)internalIconName, 0, 0);
}


//----------------------------------------------------------------

TokenEntry::TokenEntry(const QString &newKeyword, StyleEntry *newStyle)
{
    _keyword = newKeyword ;
    _style = newStyle ;
}

TokenEntry::~TokenEntry()
{
}

const QString & TokenEntry::keyword()
{
    return _keyword ;
}

StyleEntry *TokenEntry::style()
{
    return _style;
}

void TokenEntry::setStyle(StyleEntry * newStyle)
{
    _style = newStyle;
}

//----------------------------------------------------------------

IdEntry::IdEntry(const QString &value, const bool isAlpha)
{
    _value = value ;
    _isAlpha = isAlpha;
}

IdEntry::~IdEntry()
{
}

bool IdEntry::isAlpha() const
{
    return _isAlpha;
}

const QString &IdEntry::value() const
{
    return _value;
}

//----------------------------------------------------------------

QFont VStyle::_defaultFont("Sans Serif");
QFontMetrics VStyle::_defaultFontMetrics(VStyle::_defaultFont);
QBrush VStyle::_defaultBrush;
bool VStyle::globalInitFlag = false;
int  VStyle::_defaultFontSize = 0;

VStyle::VStyle(const QString &newName, const QString &newDescription)
{
    _read = false;
    _inited = false;
    _ready = false;
    _name = newName;
    _description = newDescription ;
    _defaultStyle = NULL ;
}


VStyle::~VStyle()
{
    foreach(StyleEntry * entry, _styles.values()) {
        delete entry;
    }
    foreach(TokenEntry * entry, _keywords.values()) {
        delete entry;
    }
    foreach(IdEntry * entry, _elementIds.values()) {
        delete entry;
    }
    foreach(StyleRuleSet * ruleSet, _ruleSets) {
        delete ruleSet;
    }
    if(NULL != _defaultStyle) {
        delete _defaultStyle;
    }
}


void VStyle::updateFontMetrics()
{
    _defaultFontMetrics = QFontMetrics(_defaultFont);
}


bool VStyle::addId(const QString &newId, const bool isAlpha)
{
    if(_elementIds.value(newId, NULL) != NULL) {
        return false;
    }
    IdEntry * entry = new IdEntry(newId, isAlpha) ;
    if(NULL != entry) {
        _elementIds.insert(newId, entry) ;
        return true;
    }
    return false ;
}

const QString &VStyle::name() const
{
    return _name;
}

QString VStyle::description() const
{
    return _description ;
}

bool VStyle::addToken(TokenEntry *tk)
{
    if(_keywords.value(tk->keyword(), NULL) != NULL) {
        return false;
    }
    _keywords.insert(tk->keyword(), tk);
    return true;
}

bool VStyle::addEntry(StyleEntry* entry)
{
    if(_styles.value(entry->id(), NULL) != NULL) {
        return false;
    }
    _styles.insert(entry->id(), entry);
    return true;
}

void VStyle::addRuleSet(StyleRuleSet *rs)
{
    if(NULL == rs) {
        return ;
    }
    if(rs->idStyle().isEmpty()) {
        Utils::error(QObject::tr("A style ruleset is missing style reference. Check styles"));
    } else {
        _ruleSets.append(rs);
    }
}

void VStyle::setDefaultStyleEntry(StyleEntry* newEntry)
{
    _defaultStyle = newEntry ;
}

StyleEntry* VStyle::getStyleOfKeyword(const QString &kw)
{
    QString keyword = kw;
    int indexOfColon = keyword.indexOf(":");
    if(indexOfColon >= 0) {
        keyword = keyword.mid(indexOfColon + 1); // if -1 => 0
    }
    TokenEntry *tk = _keywords.value(keyword, NULL);
    if(NULL != tk) {
        return tk->style();
    }
    return NULL ;
}

StyleEntry* VStyle::getCalculatedStyle(Element *element)
{
    foreach(StyleRuleSet * rs, _ruleSets) {
        if(rs->evaluate(element)) {
            QString idStyle = rs->idStyle();
            if(_styles.contains(idStyle)) {
                return _styles[rs->idStyle()];
            }
            Utils::warning("Style '%1' has a rule set without associated style.");
            return NULL ;
        }
    }
    return NULL ;
}

StyleEntry* VStyle::getStyleEntry(const QString &key)
{
    StyleEntry * entry = _styles.value(key, NULL);
    if(NULL == entry) {
        if(NULL != _defaultStyle) {
            if(key == _defaultStyle->id()) {
                return _defaultStyle ;
            }
        }
    }
    return entry ;
}

IdEntry* VStyle::getIdEntry(const QString &key)
{
    return _elementIds.value(key, NULL);
}

QList<StyleRuleSet*> VStyle::ruleSets()
{
    return _ruleSets;
}

StyleEntry* VStyle::defaultStyleEntry()
{
    return _defaultStyle ;
}


void VStyle::globalActivate(const QFont &baseFont, const QBrush &baseBrush)
{
    globalInitFlag = true;
    _defaultFont = baseFont ;
    _defaultFontSize = baseFont.pointSize();
    updateFontMetrics();
    _defaultBrush = baseBrush ;
}

void VStyle::changeDefaultBrush(const QBrush &baseBrush)
{
    _defaultBrush = baseBrush ;
}

bool VStyle::activate(const QFont &baseFont, const int zoom)
{
    bool isOk = true ;
    if(!_inited) {
        _inited = true;
    }
    initFromResources();
    foreach(StyleEntry * entry, _styles.values()) {
        if(!entry->activate(baseFont, zoom)) {
            isOk = false ;
        }
    }
    if(NULL != _defaultStyle) {
        if(!_defaultStyle->activate(baseFont, zoom)) {
            isOk = false ;
        }
    }

    return isOk ;
}

void VStyle::deactivate()
{
    foreach(StyleEntry * entry, _styles.values()) {
        entry->deactivate();
    }
    if(NULL != _defaultStyle) {
        _defaultStyle->deactivate();
    }
}

const QFont &VStyle::defaultFont()
{
    return _defaultFont;
}

const QFontMetrics &VStyle::defaultFontMetrics()
{
    return _defaultFontMetrics;
}

const QBrush &VStyle::defaultBrush()
{
    return _defaultBrush;
}

void VStyle::setZoom(const int zoom)
{
    int newSize = getZoomFontSize(_defaultFontSize, zoom);
    if(0 == newSize) {
        return ;
    }
    _defaultFont.setPointSize(newSize);
    updateFontMetrics();
    // gestisce lo stile nei figli
    foreach(StyleEntry * entry, _styles.values()) {
        entry->setZoom(zoom);
    }
}

int VStyle::getZoomFontSize(const int size, const int zoom)
{
    if(zoom <= 1) {
        return size ;
    }
    int newSize = ((zoom + 1) * size) / 2;
    return newSize ;
}

void VStyle::setResFileName(const QString &newResFileName)
{
    _resFileName = newResFileName ;
}

void VStyle::setNamespace(const QString &newNs)
{
    _ns = newNs ;
}

QString VStyle::getNamespace()
{
    return _ns ;
}

bool VStyle::initFromResources()
{
    if(_read) {
        return _ready ;
    }
    bool ok = false;
    if(!_resFileName.isEmpty()) {
        StylePersistence sPers;
        ok = sPers.readStyleFile(this, _resFileName);
    } else {
        ok = true ;
    }
    _read = true ;
    _ready = ok;
    return ok;
}
