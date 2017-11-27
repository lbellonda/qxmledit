/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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
#include "qxmleditconfig.h"
#include "utils.h"
#include <QTextStream>
#include <QTextCodec>

#define CSS_RESOURCE_FILE   ":xsd/exportcss.css"

const QString XSDPrintInfo::EndSeparator("<div>&nbsp;<HR/></div>");

XSDPrintInfo::XSDPrintInfo()
{
    debugging = false;
    singleBoxDebugging = false;
    useLinks = false ;
    qtSyntax = true ;
    printer = NULL ;
    availableHeight = 0;
    em = 0;
    currentY = 0;
    pages = 0 ;
    isCalculating = false;
    pageBottomMargin = 0;
    totalPages = 0;
    simple = false ;
    error = false ;
}

XSDPrintInfo::~XSDPrintInfo()
{
}

bool XSDPrintInfo::init()
{
    QFile cssFile(CSS_RESOURCE_FILE);
    if(cssFile.open(QFile::ReadOnly)) {
        QString decoded = QString::fromUtf8(cssFile.readAll());
        //
        QString normalized = decoded.replace("\r\n", "\n");
        QStringList strings = normalized.split('\n');
        QStringList translated = translateCSS(strings) ;
        cssTemplate = translated.join("\n");
        //
        cssFile.close();
        return true;
    }
    return false;
}

QString XSDPrintInfo::decode(const QString &inputData)
{
    //--HQ    margin-left:***em10*px;
    QString decoded = inputData.mid(4);
    //    margin-left:***em10*px;
    int index = 0;
    bool engaged = false ;
    do {
        index ++ ;
        engaged = false ;
        const int startPos = decoded.indexOf("***em");
        if(startPos >= 0) {
            engaged = true ;
            const int endPos = decoded.indexOf("*", startPos + 5);
            if(endPos >= 0) {
                QString theNumber = decoded.mid(startPos + 5, endPos - startPos - 5);
                bool ok = false;
                const float value = theNumber.toFloat(&ok);
                if(ok) {
                    const int theValue = value * em;
                    QString replacement = QString::number(theValue);
                    decoded = decoded.replace(startPos, endPos - startPos + 1, replacement);
                }
            } else {
                decoded = decoded.replace(startPos, 5, "");
            }
        }
    } while(engaged && (index < 100));
    return decoded ;
}

QStringList XSDPrintInfo::translateCSS(QStringList inputData)
{
    QStringList result;
    foreach(const QString &str, inputData) {
        if(str.startsWith("--HQ")) {
            if(qtSyntax) {
                QString decoded = decode(str);
                result.append(decoded);
            }
        } else if(str.startsWith("--HT")) {
            if(!qtSyntax) {
                QString decoded = str.mid(4);
                result.append(decoded);
            }
        } else {
            result.append(str);
        }
    }
    return result;
}

void XSDPrintInfo::newPage()
{
    pages++;
    if(!isCalculating) {
        printer->newPage();
        printPageNumber(pages, totalPages);
    }
    availableHeight = maxAvailablePageHeight();
    currentY = printingBounds.top();
}

qreal XSDPrintInfo::maxAvailablePageHeight()
{
    return printingBounds.height() - pageBottomMargin;;
}

void XSDPrintInfo::newPageIfNeeded(const qreal requestedSpace)
{
    if(requestedSpace > availableHeight) {
        newPage();
    }
}

void XSDPrintInfo::setSimple(const bool value)
{
    simple = value ;
}

bool XSDPrintInfo::isSimple()
{
    return simple ;
}

void XSDPrintInfo::reset()
{
    pages = 1;
    availableHeight = printingBounds.height() - pageBottomMargin;
    currentY = printingBounds.top();
    isCalculating = false;
}

void XSDPrintInfo::setPrinter(QPrinter *thePrinter, QPainter *thePainter, const QRectF &printRect, const double baseResolutionDpi)
{
    printer = thePrinter ;
    painter = thePainter ;
    pageBounds = printRect ;
    printingBounds = printRect ;
    currentY = printRect.top();
    availableHeight = printingBounds.height();

    QFontMetrics fm = painter->fontMetrics();
    QString text("Page 1/2");
    QRectF measRect = fm.boundingRect(text);
    pageBottomMargin = measRect.height() ;
    availableHeight = printingBounds.height() - pageBottomMargin;

    QFont fonts(painter->font());
    fonts.setPointSize(8);
    painter->setFont(fonts);
    const bool fontEmbeddingEnabled = Config::getBool(Config::KEY_XSD_REPORT_PDF_EMBEDFONTS, true);
    printer->setFontEmbeddingEnabled(fontEmbeddingEnabled);
    // 1em is 16px assumed
    //QFontInfo fontInfo = painter->fontInfo();
    //Utils::message(QString("Pixel: %1 points:%2").arg(fontInfo.pixelSize()).arg(fontInfo.pointSizeF()));

    fm = painter->fontMetrics();
    text = "m" ;
    measRect = fm.boundingRect(text);
    em = measRect.width() ;
    const double printerDPI = printer->logicalDpiX() ;
    if(printerDPI != 0) {
        const double multFactor = baseResolutionDpi / printerDPI ;
        em *= multFactor ;
    }
    if(0 == em) {
        // for Firefox 1px=1/96 inch
        em = 16 ;
    }
    if(!init()) {
        Utils::error(QObject::tr("Error initializing the report."));
    }
}

void XSDPrintInfo::printPageNumber(const int pageNumber, const int totalPages)
{
    if(totalPages != 0) {
        QFontMetrics fm = painter->fontMetrics();
        QString text = QString(QObject::tr("Page %1/%2")).arg(pageNumber).arg(totalPages);
        QRectF measRect = fm.boundingRect(text);
        float intervalx = (printingBounds.width() - measRect.width()) / 2;
        float x = printingBounds.left() + intervalx;
        float y = printingBounds.bottom() - pageBottomMargin;
        QRectF drawRect(x, y, measRect.width() + intervalx, measRect.height());
        painter->drawLine(QPointF(printingBounds.left() + 10., y), QPointF(printingBounds.right() - 10., y));
        painter->drawText(drawRect, text);
        //printf("page number: x %g y %g w %g h %g\n", drawRect.left(), drawRect.top(), drawRect.width(), drawRect.height());
        //printf("  dest area : x %g y %g w %g h %g\n", destArea.left(), destArea.top(), destArea.width(), destArea.height());
    }
}

//-----------------

void XSDPrintInfo::printBox(const QString &theHtmlText)
{
    if(singleBoxDebugging) {
        if(!checkHeader(theHtmlText)) {
            Utils::error(QObject::tr("Inner error: HTML malformed"));
        }
    }

    QString htmlText = QString("<html><head><style>%1</style></head><body>").arg(cssFinal())
                       + theHtmlText + QString("</body></html>");
    QTextDocument document;
    setupDocument(document);
    document.setHtml(htmlText);

    if(singleBoxDebugging) {
        debugString(htmlText);
    }

    QSizeF theDocumentSize = documentSize(document);
    qreal remainingHeight = theDocumentSize.height() ;

    QAbstractTextDocumentLayout *layout = document.documentLayout();
    QTextBlock block = document.firstBlock();

    qreal docTopWindow = 0; // this is the top coordinate for the printing
    qreal remainingHeightFromPreviousPage = 0;
    int index = 0;
    int blockIndex = 0 ;
    while(remainingHeight > 0) {
        newPageIfNeeded(remainingHeight);
        // single page loop
        bool donePage = false;
        // height used in current page
        qreal heightInPage = 0 ;
        heightInPage = remainingHeightFromPreviousPage ;
        while(!donePage) {
            if(remainingHeight <= availableHeight) {
                heightInPage = remainingHeight ;
                donePage = true ;
            } else {
                // only happens on a new page when a block is broken, too tall
                if(heightInPage > availableHeight) {
                    // rompiamo il blocco a meta, non ci sono alternative;
                    // break now
                    /*if(heightInPage>maxAvailablePageHeight()) {
                        // just in case less than one line available
                        if( heightInPage < em ) {
                        }*/
                    remainingHeightFromPreviousPage = heightInPage - availableHeight ;
                    heightInPage = availableHeight ;
                    donePage = true ;
                } else {
                    if(blockIsValid(block, blockIndex)) {
                        QRectF blockBounds = blockBoundingBox(layout, block, blockIndex); // abs position
                        qreal heightIncludingCurrentBlock = blockBounds.top() + blockBounds.height() - docTopWindow;
                        if(heightIncludingCurrentBlock > availableHeight) {
                            // block on new page
                            donePage = true ;
                            if(0 == heightInPage) {
                                // new page, the block is too tall, break it
                                heightInPage = availableHeight ;
                                remainingHeightFromPreviousPage = heightInPage - availableHeight ;
                            }
                            break;
                        } else {
                            heightInPage = blockBounds.top() + blockBounds.height() - docTopWindow;
                            if(heightInPage == availableHeight) {
                                donePage = false;
                            }
                            block = block.next();
                            blockIndex ++ ;
                        }
                    } else {
                        // no more blocks
                        donePage = true ;
                    }
                }
            }
            index++;
            if(index > 1000) {
                error = true ;
                break;
            }
        }
        // drawing a page, the document here
        if(!isCalculating) {
            painter->save();
            painter->translate(0, currentY - docTopWindow);
            QRectF definitionRect(0, docTopWindow, theDocumentSize.width(), heightInPage);
            document.drawContents(painter, definitionRect);
            painter->restore();
        }
        if(heightInPage >= remainingHeight) {
            remainingHeight = 0;
        } else {
            remainingHeight -= heightInPage ;
        }
        docTopWindow += heightInPage ;
        updatePageHeight(heightInPage);
    }
}

QString XSDPrintInfo::cssFinal()
{
    return cssTemplate;
}

QString XSDPrintInfo::tableRowClassForIndex(const int index)
{
    if((index & 1) == 1) {
        return "rowodd";
    }
    return "roweven";
}

QString XSDPrintInfo::tableAttributeRowClassForIndex(const int index)
{
    if((index & 1) == 1) {
        return "rowAttrOdd";
    }
    return "rowAttrEven";
}

bool XSDPrintInfo::blockIsValid(QTextBlock block, const int /*blockIndex*/)
{
    return block.isValid();
}

QRectF XSDPrintInfo::blockBoundingBox(QAbstractTextDocumentLayout *layout, QTextBlock block, const int /*blockIndex*/)
{
    return layout->blockBoundingRect(block);
}

void XSDPrintInfo::setupDocument(QTextDocument &document)
{
    document.documentLayout()->setPaintDevice(painter->device());
    document.setPageSize(printer->pageRect().size());
}

QSizeF XSDPrintInfo::documentSize(QTextDocument &document)
{
    return document.size();
}

void XSDPrintInfo::updatePageHeight(const int heightInPage)
{
    currentY += heightInPage ;
    availableHeight -= heightInPage ;
}

void XSDPrintInfo::debugString(const QString &text)
{
    QString clp = QApplication::clipboard()->text();
    clp += text;
    QApplication::clipboard()->setText(clp);
    printf("%s\n", text.toLatin1().data());
    fflush(0);
}

bool XSDPrintInfo::checkHeader(const QString &theHtmlText)
{
    if((theHtmlText.toLower().indexOf("<body>") >= 0) || (theHtmlText.toLower().indexOf("</body>") >= 0)
            || (theHtmlText.toLower().indexOf("<html>") >= 0) || (theHtmlText.toLower().indexOf("</html>") >= 0)) {
        return false ;
    }
    return true;
}
