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

#ifndef QXMLEDITWIDGET_H_UTI
#define QXMLEDITWIDGET_H_UTI

#include "libQXmlEdit_global.h"
#include <QObject>
#include <QComboBox>
#include <QDomDocument>
#include <QTreeWidgetItem>
#include <QToolButton>
#include <QByteArray>
#include <QIODevice>
#include <QFile>

class Regola;
class QMainWindow;

class LIBQXMLEDITSHARED_EXPORT Utils : public QObject
{
    Q_OBJECT
private:
    Utils() {}

public:
    static const int ReasonableIterationCount ;
    static bool isUnitTest ;
    static int silenceMessages1;
    static bool filler1; // filler to avoid memory corruption if possible
    static bool filler2;
    static int silenceMessages2;

    static void setBatch(const bool asBatch);
    static bool isSilenceMode();
    // --
    static void error(const QString& message);
    static void error(QWidget *parent, const QString & message);
    static void warning(const QString& message);
    static void warning(QWidget *parent, const QString & message);
    static void message(const QString& message);
    static void message(QWidget *parent, const QString & message);
    static bool askYN(QWidget *parent, const QString & message);
    static bool askYN(const QString & message);

    // Specialized errors
    static void errorNoSel(QWidget *parent);
    static void errorOutOfMem(QWidget *parent);
    static void errorReadingUserSettings();
    static void errorSavingUserSettings();
    static void errorQtVersion(QWidget *parent);
    static void errorFilePathInvalid(QWidget *parent);
    static void errorAccessingFile(QWidget *parent);

    static QString msgOutOfMem();
    static QString errorNoSelString();

    static void todo(const QString &message);
#if defined(QXMLEDIT_VERSION_IS_SNAPSHOT)
    static void TODO_THIS_RELEASE(const QString &message);
#endif
#define UNTESTED(msg) TODO_THIS_RELEASE( QString("UNTESTED:%1 %2").arg(__FILE__).arg(_LINE__))
    static void TODO_CHECK_FIX(const QString &message);
    static void TODO_NEXT_RELEASE(const QString &inputMessage);
    //------
    static void showWaitCursor();
    static void restoreCursor();

    //-- utility
    static void selectComboValue(QComboBox *combo, const int value);
    static int comboSelectedCodeAsInt(QComboBox *combo, const int defaultValue);
    static void selectComboText(QComboBox *combo, const QString &text);
    static void updateEditableCombo(QComboBox *combo, const QStringList &values);
    static void updateNonEditableCombo(QComboBox *combo, const QStringList &values);
    static void setComboCurrentText(QComboBox *combo, const QString &text);

    static bool decodeBoolean(const QString &str);
    static int decodeInt(const QString &str, const int defaultVale);

    static QString fromBase64(const QString &text);
    static QString fromBase64Xml(const QString &text);
    static QString toBase64(const QString &text);

    static void appendTextNode(QDomDocument &document, QDomElement &parent, const QString &tag, const QString &text);
    static QDomElement appendNode(QDomDocument &document, QDomElement &parent, const QString &tag);

    static QString readUTF8FileString(const QString &filePath, bool& error);
    static QStringList readUTF8FileStringList(const QString &filePath, bool& error);
    static bool writeXDocumentToFile(QDomDocument &document, const QString &filePath, const bool isOverWrite);

    static bool checkXMLName(const QString &theTag);
    static bool checkNsPrefix(const QString &prefix, const bool emptyIsLegal = false);
    static bool canStartXmlName(const QChar &ch);
    static bool canFollowXmlStartName(const QChar &ch);

    static void deleteQWIList(QList<QTreeWidgetItem *> &list);

    static QString changeFileType(const QString &inputString, const QString &newExtension);
    static QString normalizeStringXML(const QString &value);

    static void loadComboTextArray(QStringList strings, QList<QComboBox*> &combos);
    static void loadComboTextArrays(QComboBox *combo, const QString &selectedValue, const QStringList &texts, const QStringList &values);
    static void loadComboCodedArrays(QComboBox *combo, const int selectedValue, const QStringList &texts, const QList<int> &values);
    static void loadButtonMenu(QToolButton *button, const QStringList &texts, const QStringList &values, QObject *target, const char *method);
    static void setupComboEncoding(QComboBox *combo);
    static void setupComboEncoding(QComboBox *combo, const bool allowEmpty, const QString &currentSelection);

    static QString getSizeForPresentation(const qint64 size);

    static QString decodeXMLError(const QString &error);
    static QString escapeHTML(const QString &msg);
    static QString tableCellHeader(const QString &text);
    static QString tableCell(const QString &text, const bool alignRight = false);

    static void setBkColorToWidget(QWidget *widget, const QColor &color);
    static void setBkColorAndBorderToWidget(QWidget *widget, const QColor &color);

    static QString valueStringCSV(const QString &value, const bool isSkipCommaBefore);
    static QString separatorStringCSV();

    static QString getFileFilterForOpenFile();
    static QString getFileFilterForJavaSourceOpenFile();
    static QString getFileFilterForCSV();
    static QString getFileFilterForCSVOrText();

    static void dumpStr(const QString &i1, const QString &i2);

    static void addMaximizeToDialog(QDialog *dialog);
    static void enableAndShowWidget(QWidget *widget, const bool how);
    static bool caseInsensitiveLessThanCompareFunction(const QString &s1, const QString &s2);

    static bool fileIsXSLT(Regola *regola);
    static bool fileIsSCXML(Regola *regola);

    static QAction* findAction(QMainWindow *window, const QString &actionName);
    static QAction* findAction(QList<QMenu*> menus, const QString &actionName);

    static QString escape(const QString &str);
    static QString fmtNumber(const bool isHex, const int number, const int fieldWidth, const bool usePrefix = true);
    static QString convertHtmlToPlainText(const QString &inValue);

    static const int InputTextFileSizeLimit = 1024 * 1024 ;
    static QString loadTextFile(QWidget *window, const QString &filePath, bool &isError, bool isAbort);
    static bool isStartingWithDataImage(const QString &data);
    static QString formatTextForTooltipHtml(const QString &input);
    static QString normalizeFilePath(const QString &inputPath);
    //--- encoding
    static bool isEncoding8bitNotASCII(const QString &encoding);
    static bool isAsciiCompatible(const QByteArray &encoding);
    static QByteArray translateData(const QString &string, const QByteArray &encoding);
    static bool is8BitEncodingHonoredForStreamWriter(const QString &encoding);
    //
    static void replaceWidget(QLayout *layout, QWidget *oldWidget, QWidget *newWidget, const bool setAlignment = false, const Qt::Alignment alignment = 0, const bool preferredSize = false);
    static bool askYNRemapNamespaces(QWidget *parent);
    static bool isRFC4288(const QString &name);
    static bool isRegNameRFC4288(const QString &name);
    static void copyPairsToHash(QHash<QString, QString> &hash, QList<QPair<QString, QString> > &pairs);
    static qint64 infoSizeAboutLocalDevice(QIODevice *device, const QString &fileName);
    //
    static QString pushCurrentElementPath(const QString &current, const QString &name);
    static QString popCurrentElementPath(const QString &current);
    //
    static bool writeDataToFile(const QString &file, const QByteArray &dataIn);
    static bool writeStringToFile(QFile *dataFile, const QString &dataString, const QString &encoding = "UTF-8");
    static bool writeStringToFile(const QString &file, const QString &dataString, const QString &encoding = "UTF-8");
    static QByteArray readBytesFromFile(const QString &file);
};


#endif // QXMLEDITWIDGET_s
