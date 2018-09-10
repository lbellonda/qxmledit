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

#include "xsdeditor/xsdwindow.h"
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
    QString htmlText = QString("%3<html><head><title>%1</title>\n<meta charset=\"UTF-8\"/><style type=\"text/css\">%2</style></head>\n<body>\n")
                       .arg(Utils::escapeHTML((fileName.isEmpty() ? "XSD" : fileName))).arg(cssFinal())
                       .arg("<!DOCTYPE html>\n")
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


