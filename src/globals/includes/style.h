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
#ifndef STYLE_H
#define STYLE_H

#include "libQXmlEdit_global.h"
#include "xmlEdit.h"

class Element ;
class StyleCalc
{
    QString _tp;
public:
    virtual bool evaluate(Element *element) = 0;
    StyleCalc(const QString &newTp);
    virtual ~StyleCalc();
    QString tp();
};

class StyleRule : public StyleCalc
{
public:
    enum oper {
        EQ,
        NE,
        EX,
        NX,
        GT,
        LT,
        GE,
        LE,
        SS,
        ST
    };

    enum ECfrType {
        CT_STRING,
        CT_NUM,
        CT_POSITION
    };


private:

    bool _isElement ;
    QString _name;
    QString _cfr;
    ECfrType _type;
    oper _op;
    bool _caseSensitive;
    double _cfrValNum;
    bool _numReady;
    QString _axis;
    //operator

    bool evaluateAttribute(Element *element);
    bool evaluateElement(Element *element);
    bool evalString(const QString &value, const bool exists);
    bool evalNum(const QString &value, const bool exists);
    bool evalPos(Element *element);
    Element *findElement(Element *startElement);
public:
    bool evaluate(Element *element);
    StyleRule();
    virtual ~StyleRule();

    void setAxis(const QString &newAxis);
    void setEntity(const bool isElement);
    void setName(const QString& name);
    void setOp(const QString&op);
    void setType(const QString&type);
    void setValue(const QString&value);
    void setCaseSensitive(const bool newCaseSensitive);
    //-----
    bool isCaseSensitive();
    bool isEntityElement();
    QString name();
    QString value();
    ECfrType type();
    StyleRule::oper op();
    QString axis();

};

class StyleRuleSet : public StyleCalc
{
    QString _idStyle;
    QList<StyleCalc*> _calc;
    bool _isAndConnector;
    bool evaluateOr(Element *element);
    bool evaluateAnd(Element *element);
public:
    bool evaluate(Element *element);
    StyleRuleSet();
    virtual ~StyleRuleSet();
    QString idStyle();
    void setIdStyle(const QString &newIdStyle);
    void setConnectorAnd(const bool isAnd);
    void addRule(StyleCalc *newChild);
    bool isConnectorAnd();
    QList<StyleCalc*> children();

};

class StyleEntry
{

    bool inited;
    QFont *_font;
    QFontMetrics *_fontMetrics;
    QBrush *_brush;
    QString _id;
    QColor _color;
    bool _useColor;
    QString _fontFamily;
    int _fontSize;
    bool _bold;
    bool _italic;
    int _originalFontSize;
    QString _iconName;
    QIcon _icon;
    QColor _backColor;
    bool _useBackColor;

    void updateFontMetrics();
public:

    const QString &id();
    QColor color();
    const QBrush &brush();
    bool isColor();
    QFont *font();
    QFontMetrics *fontMetrics();
    QIcon icon();

    QColor backColor();
    bool useBackColor();

    StyleEntry(const QString &id);
    ~StyleEntry();

    void setBold(const bool isBold);
    void setItalic(const bool isItalic);
    void setFontSize(const int newSize);
    void setFontFamily(const QString &newFamily);
    void setColor(const QString &newColor);
    void setBackColor(const QString &newColor);
    void setZoom(const int zoom);
    void setIconName(const QString &newIconName);

    void deactivate();
    bool activate(const QFont &baseFont, const int zoom);
};

class TokenEntry
{

    QString _keyword;
    StyleEntry *_style;

public:
    const QString & keyword();
    StyleEntry * style();
    void setStyle(StyleEntry * style);

    TokenEntry(const QString &newKeyword, StyleEntry *entry);
    ~TokenEntry();
};

class IdEntry
{

    QString _value;
    bool _isAlpha;

public:
    IdEntry(const QString &newValue, const bool newIsAlpha);
    ~IdEntry();

    bool isAlpha() const;
    const QString &value() const ;
};

class LIBQXMLEDITSHARED_EXPORT VStyle
{

public:
    static bool globalInitFlag;
private:
    bool                        _inited;
    bool                        _read;
    bool                        _ready;
    QString                     _name;
    QString                     _description;
    QMap<QString, TokenEntry*>   _keywords;
    QMap<QString, StyleEntry*>   _styles;
    QMap<QString, IdEntry*>      _elementIds;
    QList<StyleRuleSet*>         _ruleSets;
    QString                     _resFileName;
    QString                     _ns;
    StyleEntry                 *_defaultStyle;

    static QFont _defaultFont;
    static QFontMetrics _defaultFontMetrics;
    static int  _defaultFontSize;
    static QBrush _defaultBrush;

    static void updateFontMetrics();
public:

    const QString &name() const ;
    QString description() const ;
    bool addId(const QString &newId, const bool isAlpha);
    bool addToken(TokenEntry *tk);
    bool addEntry(StyleEntry* entry);
    void addRuleSet(StyleRuleSet *rs);
    void setDefaultStyleEntry(StyleEntry* newEntry);
    StyleEntry* getStyleOfKeyword(const QString &kw);
    StyleEntry* getStyleEntry(const QString &key);
    StyleEntry* defaultStyleEntry();
    StyleEntry* getCalculatedStyle(Element *element);
    IdEntry* getIdEntry(const QString &key);
    QList<StyleRuleSet*> ruleSets();

    VStyle(const QString &newName, const QString &newDescription);
    ~VStyle();

    bool activate(const QFont &baseFont, const int zoom);
    void deactivate();

    static void globalActivate(const QFont &baseFont, const QBrush &baseBrush);
    static void changeDefaultBrush(const QBrush &baseBrush);
    static const QFont &defaultFont();
    static const QFontMetrics &defaultFontMetrics();
    static const QBrush &defaultBrush();
    static int getZoomFontSize(const int size, const int zoom);
    void setZoom(const int zoom);

    void setResFileName(const QString &newResFileName);
    void setNamespace(const QString &newNs);
    QString getNamespace();
    bool initFromResources();

};


#endif // STYLE_H
