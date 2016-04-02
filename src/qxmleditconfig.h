/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2016 by Luca Bellonda and individual contributors  *
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

#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QColor>
#include "libQXmlEdit_global.h"

class LIBQXMLEDITSHARED_EXPORT Config
{

public:

    //general
    static const QString KEY_GENERAL_AUTOSCROLL_ENABLED;
    static const QString KEY_GENERAL_NOTIFICATIONS_ENABLED;
    static const QString KEY_GENERAL_LICENSE_AGREED;
    static const QString KEY_GENERAL_OPEN_NEWWINDOW;
    static const QString KEY_GENERAL_SINGLE_INSTANCE;
    static const QString KEY_GENERAL_VIEW_EDITOR_ADJUST;
    // element
    static const QString KEY_ELEMENT_FONTFIXEXDWIDTHFAMILY;
    static const QString KEY_ELEMENT_FONTFIXEXDWIDTHSIZE;
    static const QString KEY_ELEMENT_FONTFIXEXDWIDTHENABLED;
    //--
    static const QString KEY_ELEMENT_ELEMENTFONTENABLED;
    static const QString KEY_ELEMENT_ELEMENTFONTFAMILY;
    static const QString KEY_ELEMENT_ELEMENTFONTSIZE;
    static const QString KEY_ELEMENT_ELEMENTFONTITALIC;
    static const QString KEY_ELEMENT_ELEMENTFONTBOLD;
    static const QString KEY_ELEMENT_TEXT_TOOLTIP_IMAGE;
    // mainview
    static const QString  KEY_MAIN_COMPACTVIEW;
    static const QString  KEY_MAIN_ONEATTRLINE;
    static const QString  KEY_MAIN_SHOWATTRLEN;
    static const QString  KEY_MAIN_ATTRFIXEDS;
    static const QString  KEY_MAIN_INDEXPERCHILD;
    static const QString  KEY_MAIN_SHOWBASE64;
    static const QString  KEY_MAIN_SHOWZOOM;
    static const QString  KEY_MAIN_RECENTFILES;
    static const QString  KEY_MAIN_PREFDIRS;
    static const QString  KEY_MAIN_SHOWELTEXTLEN;
    static const QString  KEY_MAIN_SHOWELSIZE;
    static const QString  KEY_MAIN_ENABLEEXPERIMENTS;
    static const QString  KEY_MAIN_SHOWFULLCOMMENTS;
    static const QString  KEY_MAIN_SHOWBUTTONS;
    static const QString  KEY_MAIN_SHOWSTATUSBAR;
    static const QString  KEY_MAIN_SORTATTRIBUTESALPHA;
    static const QString  KEY_MAIN_ATTRCOLLLIMIT;
    static const QString  KEY_MAIN_SHOWELEMNTSICON;

    // style
    static const QString KEY_VIEW_STYLE;
    //search
    static const QString KEY_SEARCH_CLOSEUNRELATED;
    static const QString KEY_SEARCH_MATCHEXACT;
    static const QString KEY_SEARCH_CASEUNSITIVE;
    static const QString KEY_SEARCH_ONLYCHILDREN;
    static const QString KEY_SEARCH_SEL2BOOKMARK;
    static const QString KEY_SEARCH_FINDTARGET;
    static const QString KEY_SEARCH_SHOWSIZE;
    static const QString KEY_SEARCH_USEXQUERY;
    static const QString KEY_MAIN_HIDEVIEW;
    static const QString KEY_MAIN_EXPANDONLOAD;
    static const QString KEY_SEARCH_ITEMS;
    static const QString KEY_SEARCH_SCOPES;
    static const QString KEY_SEARCH_WRAPAROUND;
    // styles
    static const QString KEY_STYLE_DATADIR;
    static const QString KEY_STYLE_DIRLOCATION;
    //static const QString KEY_VIEW_ITEMRENDERERATTR; deprecated but still alive
    static const QString KEY_VIEW_STYLE_DEFAULT_BOLD;

    // test (private use)
    static const QString KEY_TEST_BALSAMIQ_INPUT;
    static const QString KEY_TEST_BALSAMIQ_OUTPUT;
    static const QString KEY_TEST_SHOW_XSD_EDITOR;
    static const QString KEY_TEST_SAMPLE_FILE_XSD;
    static const QString SAMPLE_SCHEMA_FILE_READ;
    // balsamiq translator
    static const QString KEY_BALSAMIQ_OVERWRITEFILES;
    static const QString KEY_BALSAMIQ_OUTPUTDIR;
    static const QString KEY_BALSAMIQ_INPUTDIR;
    // search in files
    static const QString KEY_SEARCHINFILES_INPUTFILE;
    static const QString KEY_SEARCHINFILES_PATTERN;
    static const QString KEY_SEARCHINFILES_GROUP;
    // schemas
    static const QString KEY_SCHEMA_LAST;
    static const QString KEY_SCHEMA_PREFERITES;
    static const QString KEY_AUTOLOAD_VALIDATION;
    // schema cache
    static const QString KEY_XSDCACHE_ENABLED;
    static const QString KEY_XSDCACHE_LIMIT;
    // extractFragments
    static const QString  KEY_FRAGMENTS_INPUTFILE;
    static const QString  KEY_FRAGMENTS_SPLITPATH;
    static const QString  KEY_FRAGMENTS_EXTRACTIONTYPE;
    static const QString  KEY_FRAGMENTS_MINDOC;
    static const QString  KEY_FRAGMENTS_MAXDOC;
    static const QString  KEY_FRAGMENTS_EXTRACTFOLDER;
    static const QString  KEY_FRAGMENTS_MAKESUBFOLDERS;
    static const QString  KEY_FRAGMENTS_SUBFOLDERSEACH;
    static const QString  KEY_FRAGMENTS_FILESNAMEPATTERN;
    static const QString  KEY_FRAGMENTS_SUBFOLDERSNAMEPATTERN;
    static const QString  KEY_FRAGMENTS_REVERSERANGE;
    static const QString  KEY_FRAGMENTS_ISAFILTER;
    static const QString  KEY_FRAGMENTS_ATTRIBUTENAME;
    static const QString  KEY_FRAGMENTS_COMPARISONTERM;
    static const QString  KEY_FRAGMENTS_COMPARISONTYPE;
    static const QString  KEY_FRAGMENTS_DEPTH;
    static const QString  KEY_FRAGMENTS_SPLITTYPE;
    static const QString  KEY_FRAGMENTS_FILTERALLTEXT;
    static const QString  KEY_FRAGMENTS_FILTERTEXTFORPATH;
    static const QString  KEY_FRAGMENTS_FILTERTEXT_PATH;

    // welcome dialog
    static const QString  KEY_WELCOMEDIALOG_ENABLED;
    static const QString  KEY_WELCOMEDIALOG_VERSION;

    // sessions
    static const QString KEY_SESSIONS_ENABLED;
    static const QString KEY_SESSIONS_ACTIVECODE;
    static const QString KEY_SESSIONS_ACTIVESTATE;

    // xsd
    static const QString KEY_XSD_FONTFAMILY;
    static const QString KEY_XSD_FONTSIZE;
    static const QString KEY_XSD_MAINFONTFAMILY;
    static const QString KEY_XSD_MAINFONTSIZE;
    static const QString KEY_XSD_MAINFONTBOLD;
    static const QString KEY_XSD_TYPEFONTFAMILY;
    static const QString KEY_XSD_TYPEFONTSIZE;
    static const QString KEY_XSD_TYPEFONTITALIC;
    static const QString KEY_XSD_COMPARE_ANNOTATIONS;
    static const QString KEY_XSD_DISPLAY_LAYOUT;

    // xsd gradients
    static const QString KEY_XSD_DIAGRAM_BK_GRADIENTMAINCOLOR;
    static const QString KEY_XSD_DIAGRAM_BK_GRADIENTALTCOLOR;
    static const QString KEY_XSD_DIAGRAM_BK_GRADIENTTYPE;
    static const QString KEY_XSD_DIAGRAM_BK_GRADIENTDIRECTION;
    static const QString KEY_XSD_DIAGRAM_BK_GRADIENTHEIGHT;
    static const QString KEY_XSD_DIAGRAM_BK_GRADIENTUNITS;

    //logs
    static const QString KEY_LOGS_ENABLED;
    static const QString KEY_LOGS_LEVEL;

    //compare
    static const QString KEY_COMPARE_OPTION_TEXT;
    static const QString KEY_COMPARE_OPTION_COMMENT;
    static const QString KEY_COMPARE_OPTION_DENORMALIZEEOL;

    // XSL
    static const QString KEY_XSL_SHOW_PANEL;
    static const QString KEY_XSL_AUTOMODE;

    // binary viewer
    static const QString KEY_BINARY_ENCODING;

    // metadata
    static const QString KEY_METADATA_UPDATE;

    // xml management
    static const QString KEY_XML_AUTOINSERT_PROLOG;
    static const QString KEY_XML_AUTOINSERT_ENCODING;
    static const QString KEY_XML_INSERT_ENCODING;
    static const QString KEY_XML_INDENT;
    static const QString KEY_XML_INDENT_ATTRIBUTES_COLUMNS;
    static const QString KEY_XML_INDENT_ATTRIBUTES_TYPE;

    // code page
    static const QString KEY_CODEPAGE_VISUTF8;
    static const QString KEY_CODEPAGE_VISVALUE;
    static const QString KEY_CODEPAGE_VISNAMES;

    // colors
    static const QString KEY_COLOR_TAGS;
    static const QString KEY_COLOR_DECL_PROCINSTR;
    static const QString KEY_COLOR_ATTRNAMES;
    static const QString KEY_COLOR_ATTRVALUES;
    static const QString KEY_COLOR_COMMENTS;
    static const QString KEY_COLOR_INFO;
    static const QString KEY_COLOR_TEXTDECL;
    static const QString KEY_COLOR_ANONELPREVIEW;
    static const QString KEY_COLOR_ANONTEXTPREVIEW;
    static const QString KEY_COLOR_ANONATTRPREVIEW;

    // xml
    static const QString KEY_XML_SAVE_STREAM;
    static const QString KEY_XML_SAVE_SORTATTRIBUTES;
    static const QString KEY_XML_LOAD_STREAM;

    // base64
    static const QString KEY_BASE64_TYPE;

    // test
    static const QString KEY_TEST;

    //-----------------------------------------------------

    static const bool KEY_GENERAL_NOTIFICATIONS_ENABLED_DEFAULT_VALUE = true ;
    //-----------------------------------------------------

    static bool getBool(const QString &key, const bool defaultValue);
    static const QString getString(const QString &key, const QString &defaultValue);
    static int getInt(const QString &key, const int defaultValue);
    static qreal getReal(const QString &key, const qreal defaultValue);
    static QColor getColor(const QString &key, const QColor &defaultValue);

    static bool saveBool(const QString &key, const bool value);
    static bool saveString(const QString &key, const QString &value);
    static bool saveInt(const QString &key, const int value);
    static bool saveReal(const QString &key, const qreal value);
    static bool saveColor(const QString &key, const QColor &value);

    static bool loadStringArray(const QString &keyBase, QStringList &result, const int maxEntries = -1);
    static bool saveStringArray(const QString &keyBase, QStringList &values, const int maxEntries = -1);

    static bool loadIntArray(const QString &keyBase, QList<int> &result);
    static bool saveIntArray(const QString &keyBase, QList<int> &values);

    static bool init();
    static bool end();

    static void setBackend(QMap<QString, QVariant> *newBackend);

private:
    Config() {}
    ~Config() {}
};


#endif // CONFIG_H
