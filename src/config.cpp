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

#include "xmlEdit.h"
#include <QtGui>
#include "utils.h"
#include "qxmleditconfig.h"
#include <QSettings>


//general
const QString Config::KEY_GENERAL_AUTOSCROLL_ENABLED("generalConfiguration/autoScrollEnabled");
const QString Config::KEY_GENERAL_NOTIFICATIONS_ENABLED("generalConfiguration/notificationsEnabled");
const QString Config::KEY_GENERAL_LICENSE_AGREED("generalConfiguration/licenseAgreed");
const QString Config::KEY_GENERAL_OPEN_NEWWINDOW("generalConfiguration/openNewWindow");
const QString Config::KEY_GENERAL_SINGLE_INSTANCE("generalConfiguration/singleInstance");
const QString Config::KEY_GENERAL_VIEW_EDITOR_ADJUST("generalConfiguration/adjustEditor");
const QString Config::KEY_MULTITHREADING_ENABLED("generalConfiguration/multiThreading");
// edit
const QString Config::KEY_ELEMENT_EDIT_MODEFORM("edit/modeForm");
const QString Config::KEY_ELEMENT_EDIT_MODEFORM_MODIFIED("edit/modeFormModified"); // config has been set
const QString Config::KEY_ELEMENT_EDIT_SHORTCUT_USED("edit/editShortcutUsed");
const QString Config::KEY_ELEMENT_EDIT_SHORTCUT_DIALOG_SHOWN("edit/shortcutDialogShown");
const QString Config::KEY_ELEMENT_EDIT_TYPE_DIALOG_SHOWN("edit/editTypeDialogShown");
// element
const QString Config::KEY_ELEMENT_FONTFIXEXDWIDTHFAMILY("element/fontFidexWidthFamily");
const QString Config::KEY_ELEMENT_FONTFIXEXDWIDTHSIZE("element/fontFidexWidthSize");
const QString Config::KEY_ELEMENT_FONTFIXEXDWIDTHENABLED("element/fontFixedWidthEnabled");

const QString Config::KEY_ELEMENT_ELEMENTFONTENABLED("element/elementFontEnabled");
const QString Config::KEY_ELEMENT_ELEMENTFONTFAMILY("element/elementFontFamily");
const QString Config::KEY_ELEMENT_ELEMENTFONTSIZE("element/elementFontSize");
const QString Config::KEY_ELEMENT_ELEMENTFONTITALIC("element/elementFontItalic");
const QString Config::KEY_ELEMENT_ELEMENTFONTBOLD("element/elementFontBold");
const QString Config::KEY_ELEMENT_TEXT_TOOLTIP_IMAGE("element/showImagesOnTextTooltip");
const QString Config::KEY_ELEMENT_TEXT_TOOLTIP_PATH("element/showPathOnTextTooltip");

// mainview
const QString Config::KEY_MAIN_COMPACTVIEW("main/compactView");
const QString Config::KEY_MAIN_ONEATTRLINE("main/oneAttrPerLine");
const QString Config::KEY_MAIN_SHOWATTRLEN("main/showAttrLen");
const QString Config::KEY_MAIN_ATTRFIXEDS("main/attrFixedFonts");
const QString Config::KEY_MAIN_INDEXPERCHILD("main/indexPerChild");
const QString Config::KEY_MAIN_SHOWBASE64("main/showBase64");
const QString Config::KEY_MAIN_SHOWZOOM("main/zoom");
const QString Config::KEY_MAIN_RECENTFILES("main/recentFiles");
const QString Config::KEY_MAIN_PREFDIRS("main/preferredDirs");
const QString Config::KEY_MAIN_SHOWELTEXTLEN("main/showElementTextLen");
const QString Config::KEY_MAIN_SHOWELSIZE("main/showElementSize");
const QString Config::KEY_MAIN_HIDEVIEW("main/hideView");
const QString Config::KEY_MAIN_EXPANDONLOAD("main/expandOnLoad");
const QString Config::KEY_MAIN_ENABLEEXPERIMENTS("main/enableExperiments");
const QString Config::KEY_MAIN_SHOWFULLCOMMENTS("main/showFullComments");
const QString Config::KEY_MAIN_SHOWBUTTONS("main/showButtons");
const QString Config::KEY_MAIN_SHOWSTATUSBAR("main/showStatusBar");
const QString Config::KEY_MAIN_SORTATTRIBUTESALPHA("main/showSortAttrAlpha");
const QString Config::KEY_MAIN_ATTRCOLLLIMIT("main/showAttrCols");
const QString Config::KEY_MAIN_SHOWELEMNTSICON("main/showElementsIcon");
const QString Config::KEY_MAIN_SHOWLTONTAGS("main/showLTOnTags");

// style
const QString Config::KEY_VIEW_STYLE("view/styleSelected");
//const QString Config::KEY_VIEW_ITEMRENDERERATTR("view/itemRendererAttr");  deprecated but still alive

// test (private use)
const QString Config::KEY_TEST_BALSAMIQ_INPUT("test/balsamiqInput");
const QString Config::KEY_TEST_BALSAMIQ_OUTPUT("test/balsamiqOutput");
const QString Config::KEY_TEST_SHOW_XSD_EDITOR("test/showXsdEditor");
const QString Config::KEY_TEST_SAMPLE_FILE_XSD("test/xsdFileSample");
const QString Config::SAMPLE_SCHEMA_FILE_READ("test/xsdFileMainTest");
// search
const QString Config::KEY_SEARCH_CLOSEUNRELATED("search/closeUnrelated");
const QString Config::KEY_SEARCH_MATCHEXACT("search/matchExact");
const QString Config::KEY_SEARCH_CASEUNSITIVE("search/caseSensitive");
const QString Config::KEY_SEARCH_ONLYCHILDREN("search/onlyChildren");
const QString Config::KEY_SEARCH_SEL2BOOKMARK("search/selToBoorkmark");
const QString Config::KEY_SEARCH_FINDTARGET("search/findTarget");
const QString Config::KEY_SEARCH_SHOWSIZE("search/showSize");
const QString Config::KEY_SEARCH_USEXQUERY("search/useXQuery");
const QString Config::KEY_SEARCH_ITEMS("search/items");
const QString Config::KEY_SEARCH_SCOPES("search/scopes");
const QString Config::KEY_SEARCH_WRAPAROUND("search/wrapAround");

// styles
const QString Config::KEY_STYLE_DATADIR("styles/dataDir");
const QString Config::KEY_STYLE_DIRLOCATION("styles/choicer");
const QString Config::KEY_VIEW_STYLE_DEFAULT_BOLD("styles/defaultBold");
const QString Config::KEY_INFO_SHORTCUT_LIGHT_THEME("styles/infoShortcut");

// balsamiq translator
const QString Config::KEY_BALSAMIQ_OVERWRITEFILES("balsamiq/overwriteFiles");
const QString Config::KEY_BALSAMIQ_OUTPUTDIR("balsamiq/outputDir");
const QString Config::KEY_BALSAMIQ_INPUTDIR("balsamiq/inputDir");
// search in files
const QString Config::KEY_SEARCHINFILES_INPUTFILE("searchinFiles/inputFile");
const QString Config::KEY_SEARCHINFILES_PATTERN("searchinFiles/pattern");
const QString Config::KEY_SEARCHINFILES_GROUP("searchinFiles/group");
// schemas
const QString Config::KEY_SCHEMA_LAST("schemas/last");
const QString Config::KEY_SCHEMA_PREFERITES("schemas/favorites");
const QString Config::KEY_AUTOLOAD_VALIDATION("schemas/autoloadValidation");
// schema cache
const QString Config::KEY_XSDCACHE_ENABLED("xsdcache/enabled");
const QString Config::KEY_XSDCACHE_LIMIT("xsdcache/limit");
// extractFragments
const QString Config::KEY_FRAGMENTS_INPUTFILE("extractFragments/inputFile");
const QString Config::KEY_FRAGMENTS_SPLITPATH("extractFragments/splitPath");
const QString Config::KEY_FRAGMENTS_EXTRACTIONTYPE("extractFragments/extractionType");
const QString Config::KEY_FRAGMENTS_MINDOC("extractFragments/minDoc");
const QString Config::KEY_FRAGMENTS_MAXDOC("extractFragments/maxDoc");
const QString Config::KEY_FRAGMENTS_EXTRACTFOLDER("extractFragments/extractFolder");
const QString Config::KEY_FRAGMENTS_MAKESUBFOLDERS("extractFragments/makeSubfolders");
const QString Config::KEY_FRAGMENTS_SUBFOLDERSEACH("extractFragments/subFoldersEach");
const QString Config::KEY_FRAGMENTS_FILESNAMEPATTERN("extractFragments/filenamesPattern");
const QString Config::KEY_FRAGMENTS_SUBFOLDERSNAMEPATTERN("extractFragments/subFoldersNamePattern");
const QString Config::KEY_FRAGMENTS_REVERSERANGE("extractFragments/reverseRange");
const QString Config::KEY_FRAGMENTS_ATTRIBUTENAME("extractFragments/attributeName");
const QString Config::KEY_FRAGMENTS_COMPARISONTERM("extractFragments/comparisonTerm");
const QString Config::KEY_FRAGMENTS_COMPARISONTYPE("extractFragments/comparisonType");
const QString Config::KEY_FRAGMENTS_DEPTH("extractFragments/depth");
const QString Config::KEY_FRAGMENTS_SPLITTYPE("extractFragments/splitType");
const QString Config::KEY_FRAGMENTS_FILTERALLTEXT("extractFragments/filterAllText");
const QString Config::KEY_FRAGMENTS_FILTERTEXTFORPATH("extractFragments/filterTextForPath");
const QString Config::KEY_FRAGMENTS_FILTERTEXT_PATH("extractFragments/pathForFilterText");
const QString Config::KEY_FRAGMENTS_OPERATION_TYPE("extractFragments/operationType");
const QString Config::KEY_FRAGMENTS_USENAMESPACES("extractFragments/useNameSpaces");
const QString Config::KEY_FRAGMENTS_FILTERSID("extractFragments/filtersId");

// welcome dialog and user profiling
const QString Config::KEY_WELCOMEDIALOG_ENABLED("welcomeDialog/enabled");
const QString Config::KEY_WELCOMEDIALOG_VERSION("welcomeDialog/version");
const QString Config::KEY_USERPROFILING_FIRSTUSE("userProfiling/firstUse");
const QString Config::KEY_USERPROFILING_GUIDED("userProfiling/guided");
const QString Config::KEY_USERPROFILING_USERTYPE("userProfiling/type");
const QString Config::KEY_USERPROFILING_SHOWKEYBOARD_SHORTCUTS("userProfiling/showKbrdShrcts");

// sessions
const QString Config::KEY_SESSIONS_ENABLED("sessions/sessionsEnabled");
const QString Config::KEY_SESSIONS_ACTIVECODE("sessions/activeSessionCode");
const QString Config::KEY_SESSIONS_ACTIVESTATE("sessions/activeState");
//const QString Config::KEY_SESSIONS_DBCONFIG("sessions/dbConfig");

// xsd
const QString Config::KEY_XSD_FONTFAMILY("xsd/fontFamily");
const QString Config::KEY_XSD_FONTSIZE("xsd/fontSize");
const QString Config::KEY_XSD_MAINFONTFAMILY("xsd/mainFontFamily");
const QString Config::KEY_XSD_MAINFONTSIZE("xsd/mainFontSize");
const QString Config::KEY_XSD_MAINFONTBOLD("xsd/mainFontBold");
const QString Config::KEY_XSD_TYPEFONTFAMILY("xsd/typeFontFamily");
const QString Config::KEY_XSD_TYPEFONTSIZE("xsd/typeFontSize");
const QString Config::KEY_XSD_TYPEFONTITALIC("xsd/typeFontItalic");
const QString Config::KEY_XSD_COMPARE_ANNOTATIONS("xsd/compareAnnotations");
const QString Config::KEY_XSD_DISPLAY_LAYOUT("xsd/display/layout");
const QString Config::KEY_XSD_REPORT_PDF_EMBEDFONTS("xsd/report/pdf/embedfonts"); // font embedding in pdf report
const QString Config::KEY_XSD_REPORT_EMBEDIMAGES("xsd/report/html/embedimages");
const QString Config::KEY_XSD_REPORT_USEGRAPHVIZ("xsd/report/html/usegraphviz");
const QString Config::KEY_XSD_REPORT_OVERRIDEGRAPHVIZ("xsd/report/html/overridegvpath");
const QString Config::KEY_XSD_REPORT_PATHGRAPHVIZ("xsd/report/html/pathgraphviz");

// xsd gradients
const QString Config::KEY_XSD_DIAGRAM_BK_GRADIENTMAINCOLOR("xsd/gradient/mainColor");
const QString Config::KEY_XSD_DIAGRAM_BK_GRADIENTALTCOLOR("xsd/gradient/altColor");
const QString Config::KEY_XSD_DIAGRAM_BK_GRADIENTTYPE("xsd/gradient/gradientType");
const QString Config::KEY_XSD_DIAGRAM_BK_GRADIENTDIRECTION("xsd/gradient/gradientDirection");
const QString Config::KEY_XSD_DIAGRAM_BK_GRADIENTHEIGHT("xsd/gradient/gradientHeight");
const QString Config::KEY_XSD_DIAGRAM_BK_GRADIENTUNITS("xsd/gradient/gradientUnits");

//logs
const QString Config::KEY_LOGS_ENABLED("logs/enabled");
const QString Config::KEY_LOGS_LEVEL("logs/level");

//compare
const QString Config::KEY_COMPARE_OPTION_TEXT("compare/optionText");
const QString Config::KEY_COMPARE_OPTION_COMMENT("compare/optionComment");
const QString Config::KEY_COMPARE_OPTION_DENORMALIZEEOL("compare/optionDenormEOL");

// XSL
const QString Config::KEY_XSL_SHOW_PANEL("xsl/showPanel");
const QString Config::KEY_XSL_AUTOMODE("xsl/autoMode");
const QString Config::KEY_XSL_SAXON_PATH("xsl/saxonPath");
const QString Config::KEY_XSL_USE_SAXON("xsl/useSaxon");
const QString Config::KEY_XSL_LAST_OUTPUT_FILE("xsl/lastOutputFile");

// SCXML
const QString Config::KEY_SCXML_SHOW_PANEL("scxml/showPanel");
const QString Config::KEY_SCXML_AUTOMODE("scxml/autoMode");

// binary viewer
const QString Config::KEY_BINARY_ENCODING("binViewer/encoding");

// metadata
const QString Config::KEY_METADATA_UPDATE("metadata/update");
const QString Config::KEY_FORMATTING_INFO_ENABLED("formattingInfo/enabled");
const QString Config::KEY_FORMATTING_INFO_INSERT_ON_NEW("formattingInfo/insertOnNew");

// xml management
const QString Config::KEY_XML_AUTOINSERT_PROLOG("xmlManagement/autoInsertProlog");
const QString Config::KEY_XML_AUTOINSERT_ENCODING("xmlManagement/autoInsertPrologEncoding");
const QString Config::KEY_XML_INSERT_ENCODING("xmlManagement/insertPrologEncoding");
const QString Config::KEY_XML_INDENT("xmlManagement/indent");
const QString Config::KEY_XML_INDENT_ATTRIBUTES_COLUMNS("xmlManagement/indentAttributesColumns");
const QString Config::KEY_XML_INDENT_ATTRIBUTES_TYPE("xmlManagement/indentAttributesType");
// code page
const QString Config::KEY_CODEPAGE_VISUTF8("codePage/visUTF8");
const QString Config::KEY_CODEPAGE_VISVALUE("codePage/visValue");
const QString Config::KEY_CODEPAGE_VISNAMES("codePage/visNames");

// colors
const QString Config::KEY_COLOR_TAGS("colors/tags");
const QString Config::KEY_COLOR_DECL_PROCINSTR("colors/declProcInstruction");
const QString Config::KEY_COLOR_ATTRNAMES("colors/attrnames");
const QString Config::KEY_COLOR_ATTRVALUES("colors/attrvalues");
const QString Config::KEY_COLOR_COMMENTS("colors/comments");
const QString Config::KEY_COLOR_INFO("colors/info");
const QString Config::KEY_COLOR_TEXTDECL("colors/textDecl");
const QString Config::KEY_COLOR_ANONELPREVIEW("colors/anonElPreview");
const QString Config::KEY_COLOR_ANONTEXTPREVIEW("colors/anonTextPreview");
const QString Config::KEY_COLOR_ANONATTRPREVIEW("colors/anonAttrPreview");

// xml
const QString Config::KEY_XML_SAVE_SORTATTRIBUTES("xml/sortAttributes");
//deprecated: do not use
const QString Config::deprecated_KEY_XML_LOAD_STREAM("xml/loadStream"); // deprecated
const QString Config::deprecated_KEY_XML_SAVE_STREAM("xml/saveStream"); // deprecated

// base64
const QString Config::KEY_BASE64_TYPE("base64/type");
const QString Config::KEY_BASE64_COLUMNS("base64/columns");
const QString Config::KEY_BASE64_ENABLECOLUMNS("base64/enableColumns");

// test
const QString Config::KEY_TEST("test");

//------------------------------------------------------
static QSettings *config = NULL ;

static QMap<QString, QVariant> *theBackend = NULL ;

bool Config::init()
{
    if(NULL == config) {
        config = new QSettings();
    }
    if(NULL != config) {
        return true ;
    }
    return false;
}

bool Config::end()
{
    bool isOK = true;
    if(NULL != config) {
        isOK = false ;
        config->sync();
        if(config->status() == QSettings::NoError) {
            isOK = true ;
        }
        delete config;
        config = NULL;
    }
    return isOK;
}

void Config::setBackend(QMap<QString, QVariant> *newBackend)
{
    theBackend = newBackend ;
    end();
}

bool Config::getBool(const QString &key, const bool defaultValue)
{
    if(NULL != theBackend) {
        if(!theBackend->contains(key)) {
            return defaultValue ;
        }
        return theBackend->value(key).toBool();
    }
    if(NULL != config) {
        return config->value(key, defaultValue).toBool();
    }
    return defaultValue ;
}

const QString Config::getString(const QString &key, const QString &defaultValue)
{
    if(NULL != theBackend) {
        if(!theBackend->contains(key)) {
            return defaultValue ;
        }
        return theBackend->value(key).toString();
    }
    if(NULL != config) {
        QVariant result = config->value(key, defaultValue);
        QString value = result.toString();
        return value ;
    }
    return defaultValue ;
}

int Config::getInt(const QString &key, const int defaultValue)
{
    if(NULL != theBackend) {
        if(!theBackend->contains(key)) {
            return defaultValue ;
        }
        return theBackend->value(key).toInt();
    }
    if(NULL != config) {
        QVariant result = config->value(key, defaultValue);
        int value = result.toInt();
        return value ;
    }
    return defaultValue ;
}

qreal Config::getReal(const QString &key, const qreal defaultValue)
{
    if(NULL != theBackend) {
        if(!theBackend->contains(key)) {
            return defaultValue ;
        }
        bool isOk = false;
        qreal real = theBackend->value(key).toReal(&isOk);
        if(!isOk) {
            real = defaultValue ;
        }
        return real ;
    }
    if(NULL != config) {
        QVariant result = config->value(key, defaultValue);
        bool isOk = false;
        qreal real = result.toReal(&isOk);
        if(!isOk) {
            real = defaultValue ;
        }
        return real ;
    }
    return defaultValue ;
}

QColor Config::getColor(const QString &key, const QColor &defaultValue)
{
    if(NULL != theBackend) {
        if(!theBackend->contains(key)) {
            return defaultValue ;
        }
        QVariant variant = theBackend->value(key);
        return variant.value<QColor>();
    }
    if(NULL != config) {
        QVariant result = config->value(key, defaultValue);
        QColor color = result.value<QColor>();
        return color ;
    }
    return defaultValue ;
}

bool Config::saveBool(const QString &key, const bool value)
{
    if(NULL != theBackend) {
        theBackend->insert(key, QVariant(value));
        return true ;
    }
    if(NULL != config) {
        config->setValue(key, value);
        return true;
    }
    return false ;
}

bool Config::saveString(const QString &key, const QString &value)
{
    if(NULL != theBackend) {
        theBackend->insert(key, QVariant(value));
        return true ;
    }
    if(NULL != config) {
        config->setValue(key, value);
        return true ;
    }
    return false ;
}

bool Config::saveInt(const QString &key, const int value)
{
    if(NULL != theBackend) {
        theBackend->insert(key, QVariant(value));
        return true ;
    }
    if(NULL != config) {
        config->setValue(key, value);
        return true ;
    }
    return false ;
}

bool Config::saveReal(const QString &key, const qreal value)
{
    if(NULL != theBackend) {
        theBackend->insert(key, QVariant(value));
        return true ;
    }
    if(NULL != config) {
        config->setValue(key, value);
        return true ;
    }
    return false ;
}

bool Config::saveColor(const QString &key, const QColor &value)
{
    if(NULL != theBackend) {
        theBackend->insert(key, value);
        return true ;
    }
    if(NULL != config) {
        config->setValue(key, value);
        return true ;
    }
    return false ;
}

bool Config::loadStringArray(const QString &keyBase, QStringList &result, const int maxEntries)
{
    int numValues = getInt(QString("%1_num").arg(keyBase), 0);
    if(maxEntries > 0) {
        numValues = qMin(numValues, maxEntries);
    }
    for(int i = 0 ; i < numValues ; i ++) {
        QString key = QString("%1_%2").arg(keyBase).arg(i);
        QString value = getString(key, "");
        result.append(value);
    }
    return true;
}

bool Config::saveStringArray(const QString &keyBase, QStringList &values, const int maxEntries)
{
    bool isOk = true;
    int numValues = values.size() ;
    if(maxEntries > 0) {
        numValues = qMin(numValues, maxEntries);
    }
    if(!saveInt(QString("%1_num").arg(keyBase), numValues)) {
        isOk  = false ;
    }
    for(int i = 0 ; i < numValues ; i ++) {
        QString key = QString("%1_%2").arg(keyBase).arg(i);
        if(! saveString(key, values.at(i))) {
            isOk = false;
        }
    }
    return isOk;
}

bool Config::saveIntArray(const QString &keyBase, QList<int> &values)
{
    bool isOk = true;
    int numValues = values.size() ;
    if(!saveInt(QString("%1_num").arg(keyBase), numValues)) {
        isOk  = false ;
    }
    for(int i = 0 ; i < numValues ; i ++) {
        QString key = QString("%1_%2").arg(keyBase).arg(i);
        if(! saveInt(key, values.at(i))) {
            isOk = false;
        }
    }
    return isOk;
}

bool Config::loadIntArray(const QString &keyBase, QList<int> &result)
{
    int numValues = getInt(QString("%1_num").arg(keyBase), 0);
    for(int i = 0 ; i < numValues ; i ++) {
        QString key = QString("%1_%2").arg(keyBase).arg(i);
        int value = getInt(key, 0);
        result.append(value);
    }
    return true;
}
