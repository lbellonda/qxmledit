/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017-2018 by Luca Bellonda and individual contributors  *
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

#include <xmlEdit.h>
#include "xsdeditor/xsdwindow.h"
#include <QDateTime>
#include "utils.h"

XSDPrintInfoHTML::XSDPrintInfoHTML()
{
    useLinks = true ;
}

XSDPrintInfoHTML::~XSDPrintInfoHTML()
{
    // nothing
}

void XSDPrintInfoHTML::initForHTML(QWidget *aPainter)
{
    useLinks = true ;
    QFontMetrics fm = aPainter->fontMetrics();
    QString text("Page 1/2");
    QRectF measRect = fm.boundingRect(text);
    pageBottomMargin = measRect.height() ;
    availableHeight = printingBounds.height() - pageBottomMargin;

    em = fm.averageCharWidth();
    if(!init()) {
        Utils::error(QObject::tr("Error initializing the report."));
    }
}

//-----------------

QString XSDPrintInfoHTML::text()
{
    const QString title = fileName.isEmpty() ? "XSD" : fileName;
    QString htmlText = QString("<!DOCTYPE html>\n<html>\n<head>\n<title>%1</title>\n<meta charset=\"UTF-8\"/>")
                       .arg(Utils::escapeHTML(title));
    QDateTime now = QDateTime::currentDateTime();
    QString nowString = now.toString("yyyy-MM-dd HH:mm");

    const QString metaOther1 = QString("<meta name=\"title\" content=\"%1\" />\n").arg(Utils::escapeHTML(title));
    const QString metaOther2 = QString("<meta name=\"generator\" content=\"%1 %2\" />\n").arg(Utils::escapeHTML(APPLICATION_NAME)).arg(Utils::escapeHTML(VERSION_BASE));
    const QString metaOther3 = QString("<meta name=\"dcterms.created \" content=\"%1\" />\n").arg(Utils::escapeHTML(nowString));
    htmlText += metaOther1 + metaOther2 + metaOther3 ;
    htmlText += QString("<style type=\"text/css\">%1</style></head>\n<body>\n")
                .arg(cssFinal())
                + _text + QString("</body></html>");

    //--- debug code, do not remove
    if(debugging) {
        debugString(htmlText);
    }
    //-- end debug code
    return htmlText ;
}

void XSDPrintInfoHTML::reset()
{
    _text = "" ;
    XSDPrintInfo::reset();
}

void XSDPrintInfoHTML::newPage()
{
    // nothing
}

void XSDPrintInfoHTML::newPageIfNeeded(const qreal)
{
    // nothing
}

void XSDPrintInfoHTML::printBox(const QString &theHtmlText)
{
    //--- debug code, do not remove
    if(singleBoxDebugging) {
        debugString(theHtmlText);
    }
    //--- end debug code, do not remove

    if(singleBoxDebugging) {
        if(!checkHeader(theHtmlText)) {
            Utils::error(QObject::tr("Inner error: HTML malformed"));
        }
    }
    _text += theHtmlText ;
}


