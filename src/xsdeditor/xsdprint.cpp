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
#include "xsdwindow.h"
#include "utils.h"
#include "ui_xsdwindow.h"
#include "qxmleditconfig.h"
#include "xsdeditor/widgets/xsdgenericeditor.h"
#include "xsdeditor/choosexsdviewrootitemdialog.h"
#include <QSvgGenerator>

XSDPrintInfo::XSDPrintInfo()
{
    printer = NULL ;
    availableHeight = 0;
    em = 0;
    currentY = 0;
    pages = 0 ;
    isCalculating = false;
    pageBottomMargin = 0;
    totalPages = 0;
}

XSDPrintInfo::~XSDPrintInfo()
{
}

void XSDPrintInfo::newPage()
{
    pages++;
    if(!isCalculating) {
        printer->newPage();
        printPageNumber(pages, totalPages);
    }
    availableHeight = printingBounds.height() - pageBottomMargin;
    currentY = printingBounds.top();
}

void XSDPrintInfo::newPageIfNeeded(const qreal requestedSpace)
{
    if(requestedSpace>availableHeight ) {
        newPage();
    }
}

void XSDPrintInfo::reset()
{
    pages = 1;
    availableHeight = printingBounds.height() - pageBottomMargin;
    currentY = printingBounds.top();
    isCalculating = false;
}

void XSDPrintInfo::setPrinter(QPrinter *thePrinter, QPainter *thePainter, const QRectF &printRect )
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

    em = fm.averageCharWidth();
}

void XSDPrintInfo::printPageNumber(const int pageNumber, const int totalPages)
{
    if(totalPages != 0) {
        QFontMetrics fm = painter->fontMetrics();
        QString text = QString(QObject::tr("Page %1/%2")).arg(pageNumber).arg(totalPages);
        QRectF measRect = fm.boundingRect(text);
        float intervalx = (printingBounds.width() - measRect.width()) / 2;
        float x = printingBounds.left() + intervalx;
        float y = printingBounds.bottom() ;
        QRectF drawRect(x, y, measRect.width() + intervalx, measRect.height());
        painter->drawLine(QPointF(printingBounds.left() + 10., y), QPointF(printingBounds.right() - 10., y));
        painter->drawText(drawRect, text);
        //printf("page number: x %g y %g w %g h %g\n", drawRect.left(), drawRect.top(), drawRect.width(), drawRect.height());
        //printf("  dest area : x %g y %g w %g h %g\n", destArea.left(), destArea.top(), destArea.width(), destArea.height());
    }
}

//-----------------

XSDPrintInfoStyle::XSDPrintInfoStyle()
{
    Utils::TODO_THIS_RELEASE("forse non serve");
    backgroundColor = QColor::fromRgb(255,255,255);
    color = QColor::fromRgb(0,0,0);
    marginLeft = 0;
    marginRight = 0 ;
}

XSDPrintInfoStyle::~XSDPrintInfoStyle()
{
}

//-----------------

void XSDWindow::printPDF()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export as PDF"),
                       Utils::changeFileType(fileName, ".pdf"),
                       tr("PDF documents (*.pdf);;All files (*)"));
    if(filePath.isEmpty()) {
        return ;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(filePath);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setColorMode(QPrinter::Color    );

    /**
      what I impose is: 100 points on video (since 100pts is a width of a badge) are 2cm on paper
      */
    QPainter painter(&printer);
    //painter.begin(&printer);
    QRectF pageNumberArea;
    calculatePageRect(&painter, pageNumberArea);
    // width of a printer page in mm
    QRectF pageRect = printer.pageRect(QPrinter::Inch);
    QRectF pageRectInDevicePoints = printer.pageRect();

    // take out page position
    pageRectInDevicePoints.setHeight(pageRectInDevicePoints.height() - pageNumberArea.height());
    double resYPrinter = printer.logicalDpiY();
    if(0 == resYPrinter) {
        Utils::error(this, tr("Error in calculating printer resolution."));
        return ;
    }
    double pageHeightPrinter = pageNumberArea.height() / resYPrinter ;
    pageRect.setHeight(pageRect.height() - pageHeightPrinter);

    Utils::TODO_THIS_RELEASE("incapsulare");
    XSDPrintInfo xsdPrintInfo ;
    xsdPrintInfo.setPrinter(&printer, &painter, pageRectInDevicePoints);
    /*xsdPrintInfo.printer = &printer;
    xsdPrintInfo.pageBounds = pageRectInDevicePoints ;
    xsdPrintInfo.printingBounds = pageRectInDevicePoints ;
    xsdPrintInfo.currentY = pageRectInDevicePoints.top();
    QFontMetrics fm = painter.fontMetrics();
    xsdPrintInfo.em = fm.averageCharWidth();*/

    Utils::TODO_THIS_RELEASE("setta i margini?");

    // scene resolution
    double resXVideo = logicalDpiX();
    double resYVideo = logicalDpiY();
    // find the dimensions of a printer page on scene
    // abs width value * device resolution -> device points
    // 10pt size video -> 8pt size printer
    Utils::TODO_THIS_RELEASE("non mi torna");
    double pageWidthScene = pageRect.width() * resXVideo * 3;
    double pageHeightScene = pageRect.height() * resYVideo * 3 ;

    // maps
    if((0 == pageWidthScene) || (0 == pageHeightScene)) {
        Utils::error( this, tr("Error in calculating scene dimensions."));
        return ;
    }

    // calculate the number of pages: how many times the scene can be divided by the page dimensions
    // find the number of pages
    double dnumberOfPagesInARow = _scene->sceneRect().width() / pageWidthScene  ;
    double dnumberOfPagesInAColumn = _scene->sceneRect().height() / pageHeightScene  ;

    // round fractional values to integer
    int numberOfPagesInARow = int(dnumberOfPagesInARow);
    int numberOfPagesInAColumn = int(dnumberOfPagesInAColumn);

    // check for factional pages
    if((dnumberOfPagesInARow - numberOfPagesInARow) > 0) {
        numberOfPagesInARow++;
    }
    if(dnumberOfPagesInAColumn - dnumberOfPagesInAColumn > 0) {
        numberOfPagesInAColumn++;
    }
    // last check
    if(numberOfPagesInAColumn == 0) {
        numberOfPagesInAColumn = 1;
    }
    if(numberOfPagesInARow == 0) {
        numberOfPagesInARow = 1;
    }

    QBrush solidBrush(Qt::NoBrush);
    QBrush oldBrush = _scene->backgroundBrush();
    setUpdatesEnabled(false);
    _scene->setBackgroundBrush(solidBrush);

    // for all the pages
    const int numPages = numberOfPagesInARow * numberOfPagesInAColumn ;
    printSchemaData(&painter, xsdPrintInfo, true);
    xsdPrintInfo.totalPages = numPages +  xsdPrintInfo.pages + 1 ;
    xsdPrintInfo.pages = 1 ;
    int currentPage = 0;
    QRectF sourceArea;
    painter.save();
    xsdPrintInfo.reset();
    printSchemaInfo(&painter, xsdPrintInfo, schema()->root()->schema());
    currentPage++;
    for(int pageRow = 0 ; pageRow < numberOfPagesInARow ; pageRow ++) {
        for(int pageColumn = 0 ; pageColumn < numberOfPagesInAColumn ; pageColumn ++) {
            currentPage++;
            printer.newPage();
            sourceArea.setRect(pageRow * pageWidthScene, pageColumn * pageHeightScene, pageWidthScene, pageHeightScene);
            //printf("x %g y %g w %g h %g\n", pageRow * pageWidthScene, pageColumn * pageHeightScene, pageWidthScene, pageHeightScene);
            // is next instruction useful?
            painter.fillRect(painter.window(), QColor(255, 255, 255, 0));
            paintScene( &xsdPrintInfo, &painter, sourceArea, pageRectInDevicePoints, currentPage, xsdPrintInfo.totalPages, pageRow, pageColumn);
        }
    }
    painter.restore();
    xsdPrintInfo.pages = currentPage ;
    Utils::TODO_THIS_RELEASE("aggiorna il nuemro di pagine");
    printSchemaData(&painter, xsdPrintInfo, false);
    painter.end();

    _scene->setBackgroundBrush(oldBrush);
    setUpdatesEnabled(true);

    // end print
    setWindowTitle(_title);
    Utils::message( this, tr("Diagram exported in PDF format."));
}

//TODO: error checking
void XSDWindow::paintScene(XSDPrintInfo *xsdPrintInfo, QPainter *painter, const QRectF &sourceArea, const QRectF &destArea, const int pageNumber, const int totalPages, const int row, const int column)
{
    QList<QGraphicsItem*> items = _scene->selectedItems();
    _scene->clearSelection();

    _scene->render(painter, destArea, sourceArea);
    if((totalPages != 0) && (NULL != xsdPrintInfo) ){
        xsdPrintInfo->printPageNumber(pageNumber, totalPages);
        Utils::TODO_THIS_RELEASE("SOstituire");

        QFontMetrics fm = painter->fontMetrics();
        QString text = QString(tr("Row: %1 Col: %2")).arg((row+1)).arg((column+1));
        QRectF measRect = fm.boundingRect(text);
        float x = destArea.left() ;
        float y = destArea.bottom() ;
        QRectF drawRect(x, y, measRect.width()*2, measRect.height());
        painter->drawText(drawRect, text);
        //printf("page number: x %g y %g w %g h %g\n", drawRect.left(), drawRect.top(), drawRect.width(), drawRect.height());
        //printf("  dest area : x %g y %g w %g h %g\n", destArea.left(), destArea.top(), destArea.width(), destArea.height());*/
    }
    restoreSelection(items);
}

void XSDWindow::printSchemaData(QPainter *painter, XSDPrintInfo &xsdPrintInfo, const bool isCalculating)
{
    Utils::TODO_THIS_RELEASE("finire");
    xsdPrintInfo.isCalculating = isCalculating ;
    printSchemaIntroduction(painter, xsdPrintInfo);
    printSchemaElements(painter, xsdPrintInfo);
    printSchemaTypes(painter, xsdPrintInfo);
    printSchemaGroups(painter, xsdPrintInfo);
    printSchemaAttributes(painter, xsdPrintInfo);
    printSchemaAttributeGroups(painter, xsdPrintInfo);
    printSchemaEnd(painter, xsdPrintInfo);
}

/*
TODO: in utils
QList<XSchemaObject*> NavigationTree::sortObjectsByName(const QList<XSchemaObject*> &objects)
{
    QList<XSchemaObject*> result ;
    QMultiMap<QString, XSchemaObject*> map;
    QList<QString> keys;
    foreach(XSchemaObject * object, objects) {
        QString nameLowerCase = object->name().toLower();
        map.insert(nameLowerCase, object);
        keys.append(nameLowerCase);
    }
    qSort(keys);
    foreach(QString key, keys) {
        QList<XSchemaObject*> values = map.values(key);
        result.append(values);
    }
    return result;
}
*/

void XSDWindow::printSchemaIntroduction(QPainter *painter, XSDPrintInfo &xsdPrintInfo)
{
    Utils::TODO_THIS_RELEASE("finire");
    Utils::TODO_THIS_RELEASE(" elimina dichiarazioni senza corpo QList<XSchemaObject*>objects = schema()->topLevelListByName(SchemaTypeElement, true);");

    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelReferences();
    if(objects.isEmpty()) {
        return ;
    }
    printHeader(painter, xsdPrintInfo, tr("Schema includes and references"));

    Utils::TODO_THIS_RELEASE("objects = sortObjectsByName(schema->topLevelTypes(useOnlyThisSchemaOrAll));");
    QList<XSchemaImport*> imps;
    QList<XSchemaInclude*> incls;
    QList<XSchemaRedefine*> redefs;

    foreach(XSchemaObject * object, objects) {
        switch(object->getType()) {
        case SchemaTypeInclude:
            incls.append(static_cast<XSchemaInclude*>(object));
            break;
        case SchemaTypeRedefine:
            redefs.append(static_cast<XSchemaRedefine*>(object));
            break;
                case SchemaTypeImport:
            imps.append(static_cast<XSchemaImport*>(object));
            break;
        default:
            break;
        }
    }
    foreach(XSchemaInclude*incl, incls) {
        printInclude(painter, xsdPrintInfo, incl);
    }
    foreach(XSchemaImport*imp, imps) {
        printImport(painter, xsdPrintInfo, imp);
    }
    foreach(XSchemaRedefine*redef, redefs) {
        printRedefine(painter, xsdPrintInfo, redef);
    }
}

void XSDWindow::printSchemaElements(QPainter *painter, XSDPrintInfo &xsdPrintInfo)
{
    printHeader(painter, xsdPrintInfo, tr("Elements"));
    Utils::TODO_THIS_RELEASE("finire");
    Utils::TODO_THIS_RELEASE(" elimina dichiarazioni senza corpo QList<XSchemaObject*>objects = schema()->topLevelListByName(SchemaTypeElement, true);");
    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelElements(true);

    Utils::TODO_THIS_RELEASE("objects = sortObjectsByName(schema->topLevelTypes(useOnlyThisSchemaOrAll));");
    foreach(XSchemaObject * object, objects) {
        XSchemaElement* schemaObject = static_cast<XSchemaElement*>(object);
        /*QIcon icon = typeComplexIcon ;
        if(schemaObject->isSimpleType()) {
            icon = typeSimpleIcon;
        }*/
        printSingleElement(painter, xsdPrintInfo, schemaObject);
    }
}

void XSDWindow::printSchemaTypes(QPainter *painter, XSDPrintInfo &xsdPrintInfo)
{
    printHeader(painter, xsdPrintInfo, tr("Types"));
    Utils::TODO_THIS_RELEASE("finire");
    Utils::TODO_THIS_RELEASE(" elimina dichiarazioni senza corpo QList<XSchemaObject*>objects = schema()->topLevelListByName(SchemaTypeElement, true);");
    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelTypes(true);

    Utils::TODO_THIS_RELEASE("objects = sortObjectsByName(schema->topLevelTypes(useOnlyThisSchemaOrAll));");
    foreach(XSchemaObject * object, objects) {
        XSchemaElement* schemaObject = static_cast<XSchemaElement*>(object);
        /*QIcon icon = typeComplexIcon ;
        if(schemaObject->isSimpleType()) {
            icon = typeSimpleIcon;
        }*/
        printSingleType(painter, xsdPrintInfo, schemaObject);
    }
}

void XSDWindow::printHeader(QPainter *painter, XSDPrintInfo &xsdPrintInfo, const QString &headerText)
{
    Utils::TODO_THIS_RELEASE("finire");
    xsdPrintInfo.newPage();
    QString text ;
    text = QString("<H1 style='background-color:#000000;color:#FFFFFF'>%1</H1><div><br/></div>").arg(headerText);
    QString htmlText = "<html><body>"+text+"</body></html>";
    printBox(painter, xsdPrintInfo, htmlText );
}

QString XSDWindow::printAnnotationString(XSDPrintInfo &xsdPrintInfo, XSchemaObject *object)
{
    Utils::TODO_THIS_RELEASE("finire");

    QString annotationHtml ;

    QString annotationInfo;
    if(NULL != object->annotation()) {
        annotationInfo = object->annotation()->text();
        if(!annotationInfo.isEmpty()) {
            int em3 = xsdPrintInfo.em * 10;
            int em = xsdPrintInfo.em ;
            annotationHtml = QString("<div style='margin-left:%2px;margin-right:%3px;font-size:small;font-style:italic;color:#0000C0;'>%1</div>")
                    .arg(Utils::escapeHTML(annotationInfo))
                    .arg(em3).arg(em);
        }
    }
    return annotationHtml;
}
/**
 * @brief XSDWindow::printSingleElement
 * @param painter
 * @param headerText
 * @return
 * ELEMENTNAME  x
 *     type:x|ref: y
 *     info: xxxxx xxx xxx
 *           xxxxxxxxxxxxxx
 *           xxxx xxxx xxx
 */
int XSDWindow::printSingleElement(QPainter *painter, XSDPrintInfo &xsdPrintInfo, XSchemaElement *element)
{
    Utils::TODO_THIS_RELEASE("finire");

    QString text ;
    QString elementText = QString("<span style=''>%1</span>").arg(Utils::escapeHTML(element->name()));
    if(!element->xsdType().isEmpty()) {
        elementText += QString("&nbsp;&nbsp;<span style='color:#00C000;font-style:italic'>%1 \"%2\"</span>").arg(tr("type")).arg(Utils::escapeHTML(element->xsdType()));
    } else if(!element->ref().isEmpty()) {
        elementText += QString("&nbsp;&nbsp;<span style='color:#00C0C0;font-style:italic'>%1 \"%2\"</span>").arg(tr("reference to")).arg(Utils::escapeHTML(element->ref()));
    }

    text = QString("<div>&nbsp;</div><div style=''>%1</div>").arg(elementText);

    text += printAnnotationString(xsdPrintInfo, element);
    /*QString annotationInfo;
    if(NULL != element->annotation()) {
        annotationInfo = element->annotation()->text();
        if(!annotationInfo.isEmpty()) {
            int em3 = xsdPrintInfo.em * 10;
            int em = xsdPrintInfo.em ;
            QString annotationHtml = QString("<div style='margin-left:%2px;margin-right:%3px;font-size:small;font-style:italic;color:#0000C0;'>%1</div>")
                    .arg(Utils::escapeHTML(annotationInfo))
                    .arg(em3).arg(em);
            text += printAnnotationString(xsdPrintInfo, element);
        }
    }*/
    text += "<div><HR/>&nbsp;</div>";
    QString htmlText = "<html><body>"+text+"</body></html>";
    printBox(painter, xsdPrintInfo, htmlText );

    return 0;
}

int XSDWindow::printSingleType(QPainter *painter, XSDPrintInfo &xsdPrintInfo, XSchemaElement *element)
{
    Utils::TODO_THIS_RELEASE("finire");

    QString text ;
    QString elementText = QString("<span style=''>%1</span>").arg(Utils::escapeHTML(element->name()));
    if(!element->isSimpleType()) {
        elementText += QString("&nbsp;&nbsp;<span style='color:#00C000;font-style:italic'>%1</span>").arg(tr("simple"));
    } else {
        elementText += QString("&nbsp;&nbsp;<span style='color:#00C0C0;font-style:italic'>%1</span>").arg(tr("complex"));
    }

    text = QString("<div>&nbsp;</div><div style=''>%1</div>").arg(elementText);

    text += printAnnotationString(xsdPrintInfo, element);
    text += "<div><HR/>&nbsp;</div>";
    QString htmlText = "<html><body>"+text+"</body></html>";
    printBox(painter, xsdPrintInfo, htmlText );

    return 0;
}

int XSDWindow::printInclude(QPainter *painter, XSDPrintInfo &xsdPrintInfo, XSchemaInclude *object)
{
    Utils::TODO_THIS_RELEASE("finire");

    QString text ;
    QString elementText = QString("<span style=''>include %1</span>").arg(Utils::escapeHTML(object->schemaLocation()));
    text = QString("<div style=''>%1</div>").arg(elementText);

    text += printAnnotationString(xsdPrintInfo, object);
    text += "<div><HR/>&nbsp;</div>";
    QString htmlText = "<html><body>"+text+"</body></html>";
    printBox(painter, xsdPrintInfo, htmlText );

    return 0;
}

int XSDWindow::printImport(QPainter *painter, XSDPrintInfo &xsdPrintInfo, XSchemaImport *object)
{
    Utils::TODO_THIS_RELEASE("finire");

    QString text ;
    QString elementText = QString("<span style=''>import %1</span>").arg(Utils::escapeHTML(object->schemaLocation()));
    if(!object->targetNamespace().isEmpty()) {
        elementText += QString("<span style=''>namespace %1</span>").arg(Utils::escapeHTML(object->targetNamespace()));
    }
    text = QString("<div style=''>%1</div>").arg(elementText);

    text += printAnnotationString(xsdPrintInfo, object);
    text += "<div><HR/>&nbsp;</div>";
    QString htmlText = "<html><body>"+text+"</body></html>";
    printBox(painter, xsdPrintInfo, htmlText );

    return 0;
}

int XSDWindow::printRedefine(QPainter *painter, XSDPrintInfo &xsdPrintInfo, XSchemaRedefine *object)
{
    Utils::TODO_THIS_RELEASE("finire");

    QString text ;
    QString elementText = QString("<span style=''>redefine %1</span>").arg(Utils::escapeHTML(object->schemaLocation()));
    text = QString("<div style=''>%1</div>").arg(elementText);

    text += printAnnotationString(xsdPrintInfo, object);
    text += "<div><HR/>&nbsp;</div>";
    QString htmlText = "<html><body>"+text+"</body></html>";
    printBox(painter, xsdPrintInfo, htmlText );

    return 0;
}

int XSDWindow::printSingleElementOld(QPainter *painter, XSDPrintInfo &xsdPrintInfo, XSchemaElement *element)
{
    Utils::TODO_THIS_RELEASE("eliminare?");

    QString elementText = QString("<span style=''>%1</span>").arg(Utils::escapeHTML(element->name()));
    if(!element->xsdType().isEmpty()) {
        elementText += QString("&nbsp;&nbsp;<span style='color:#00C000:font-style:italic'>%1 \"%2\"</span>").arg(tr("type")).arg(Utils::escapeHTML(element->xsdType()));
    } else if(!element->ref().isEmpty()) {
        elementText += QString("&nbsp;&nbsp;<span style='color:#00C0C0;font-style:italic'>%1 \"%2\"</span>").arg(tr("reference to")).arg(Utils::escapeHTML(element->ref()));
    }

    QString text = QString("<div style=''>%1</div>").arg(elementText);
    QString htmlText = "<html><body>"+text+"</body></html>";
    printBox(painter, xsdPrintInfo, htmlText );

    QString annotationInfo;
    if(NULL != element->annotation()) {
        annotationInfo = element->annotation()->text();
        if(!annotationInfo.isEmpty()) {
            int em3 = xsdPrintInfo.em * 3;
            int em = xsdPrintInfo.em ;
            QString annotationHtml = QString("<p style='margin-left:%2px;margin-right:%3px;font-size:small;font-style:italic;color:#0000C0;'>%1</p>")
                    .arg(Utils::escapeHTML(annotationInfo)
                    .arg(em3).arg(em));
            Utils::TODO_THIS_RELEASE("gestire i margini con la scala");
            annotationHtml = QString("<div style='margin-left:3000px;margin-right:10px;font-size:small;font-style:italic;color:#0000C0;'>%1</div>")
                    .arg(Utils::escapeHTML(annotationInfo)
                    .arg(em3).arg(em));
            QString htmlAnnotationText = "<html><body>"+annotationHtml+"</body></html>";
            printBox(painter, xsdPrintInfo, htmlAnnotationText );
        }
    }
    QString endText = "<html><body style='font-size:2pt'><HR/></body></html>";
    printBox(painter, xsdPrintInfo, endText );
    return 0;
}

// returns the height of the box
int XSDWindow::printBox(QPainter *painter, XSDPrintInfo &xsdPrintInfo, const QString &htmlText)
{
    Utils::TODO_THIS_RELEASE("finire");

    QTextDocument document;
    document.documentLayout()->setPaintDevice(painter->device());
    //document.setTextWidth(printWidth);
    document.setHtml(htmlText);
    QSizeF documentSize = document.size();
    const qreal heightUsed = documentSize.height() ;

    xsdPrintInfo.newPageIfNeeded(heightUsed);
    painter->save();
    painter->translate(0, xsdPrintInfo.currentY);
    document.setPageSize(xsdPrintInfo.printer->pageRect().size());
    if(!xsdPrintInfo.isCalculating) {
        document.drawContents(painter);
    }

    painter->restore();

    xsdPrintInfo.currentY += heightUsed ;
    xsdPrintInfo.availableHeight -= heightUsed ;
    return documentSize.height();
}

void XSDWindow::printSchemaGroups(QPainter *painter, XSDPrintInfo &xsdPrintInfo)
{
    Utils::TODO_THIS_RELEASE("finire");
    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelGroups(true);
    if(objects.isEmpty()) {
        return ;
    }
    printHeader(painter, xsdPrintInfo, tr("Groups"));
    Utils::TODO_THIS_RELEASE("objects = sortObjectsByName(schema->topLevelTypes(useOnlyThisSchemaOrAll));");
    foreach(XSchemaObject * object, objects) {
        XSchemaGroup* schemaGroup = static_cast<XSchemaGroup*>(object);
        printSingleGroup(painter, xsdPrintInfo, schemaGroup);
    }
}

int XSDWindow::printSingleGroup(QPainter *painter, XSDPrintInfo &xsdPrintInfo, XSchemaGroup *group)
{
    Utils::TODO_THIS_RELEASE("finire");

    QString text ;
    QString name = QString("<span style=''>%1</span>").arg(Utils::escapeHTML(group->name()));

    text = QString("<div>&nbsp;</div><div style=''>%1</div>").arg(name);

    text += printAnnotationString(xsdPrintInfo, group);
    text += "<div><HR/>&nbsp;</div>";
    QString htmlText = "<html><body>"+text+"</body></html>";
    printBox(painter, xsdPrintInfo, htmlText );

    return 0;
}

void XSDWindow::printSchemaAttributes(QPainter *painter, XSDPrintInfo &xsdPrintInfo)
{
    Utils::TODO_THIS_RELEASE("finire");
    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelAttributes(true);
    if(objects.isEmpty()) {
        return ;
    }
    printHeader(painter, xsdPrintInfo, tr("Attributes"));
    Utils::TODO_THIS_RELEASE("objects = sortObjectsByName(schema->topLevelTypes(useOnlyThisSchemaOrAll));");
    foreach(XSchemaObject * object, objects) {
        XSchemaAttribute* schemaAttribute = static_cast<XSchemaAttribute*>(object);
        printSingleAttribute(painter, xsdPrintInfo, schemaAttribute);
    }
}


int XSDWindow::printSingleAttribute(QPainter *painter, XSDPrintInfo &xsdPrintInfo, XSchemaAttribute *attribute)
{
    Utils::TODO_THIS_RELEASE("finire");

    QString text ;
    QString name = QString("<span style=''>%1</span>").arg(Utils::escapeHTML(attribute->name()));

    text = QString("<div>&nbsp;</div><div style=''>%1</div>").arg(name);

    text += printAnnotationString(xsdPrintInfo, attribute);
    text += "<div><HR/>&nbsp;</div>";
    QString htmlText = "<html><body>"+text+"</body></html>";
    printBox(painter, xsdPrintInfo, htmlText );

    return 0;
}

void XSDWindow::printSchemaAttributeGroups(QPainter *painter, XSDPrintInfo &xsdPrintInfo)
{
    Utils::TODO_THIS_RELEASE("finire");
    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelAttributeGroups(true);
    if(objects.isEmpty()) {
        return ;
    }
    printHeader(painter, xsdPrintInfo, tr("Attribute Groups"));
    Utils::TODO_THIS_RELEASE("objects = sortObjectsByName(schema->topLevelTypes(useOnlyThisSchemaOrAll));");
    foreach(XSchemaObject * object, objects) {
        XSchemaAttributeGroup* schemaAttributeGroup = static_cast<XSchemaAttributeGroup*>(object);
        printSingleAttributeGroup(painter, xsdPrintInfo, schemaAttributeGroup);
    }
}

int XSDWindow::printSingleAttributeGroup(QPainter *painter, XSDPrintInfo &xsdPrintInfo, XSchemaAttributeGroup *attributeGroup)
{
    Utils::TODO_THIS_RELEASE("finire");

    QString text ;
    QString name = QString("<span style=''>%1</span>").arg(Utils::escapeHTML(attributeGroup->name()));

    text = QString("<div>&nbsp;</div><div style=''>%1</div>").arg(name);
    text += printAnnotationString(xsdPrintInfo, attributeGroup);

    {
        bool firstAttribute = true;
        QString attrNames;
        foreach(XSchemaObject * object, attributeGroup->getChildren()) {
            if( object->getType() == SchemaTypeAttribute ) {
                if(!firstAttribute) {
                    attrNames += ", ";
                } else {
                    firstAttribute = false ;
                }
                attrNames += object->name();
            }
        }
        if(!attrNames.isEmpty()) {
            int em3 = xsdPrintInfo.em * 3;
            int em = xsdPrintInfo.em ;
            text += QString("<div style='margin-left:%2px;margin-right:%3px;font-size:small;font-style:italic;color:#000000;'><span style='font-weight:bold'>%4:</span> %1</div>")
                    .arg(Utils::escapeHTML(attrNames))
                    .arg(em3).arg(em)
                    .arg(tr("Attributes"));
        }
    }
    {
        bool firstAttributeGroup = true;
        QString attrGroupNames;
        foreach(XSchemaObject * object, attributeGroup->getChildren()) {
            if( object->getType() == SchemaTypeAttributeGroup ) {
                if(!firstAttributeGroup) {
                    attrGroupNames += ", ";
                } else {
                    firstAttributeGroup = false ;
                }
                attrGroupNames += object->referencedObjectName();
            }
        }
        if(!attrGroupNames.isEmpty()) {
            int em3 = xsdPrintInfo.em * 3;
            int em = xsdPrintInfo.em ;
            text += QString("<div style='margin-left:%2px;margin-right:%3px;font-size:small;font-style:italic;color:#000000;'><span style='font-weight:bold'>%4:</span> %1</div>")
                    .arg(Utils::escapeHTML(attrGroupNames))
                    .arg(em3).arg(em)
                    .arg(tr("Attribute groups"));
        }
    }
    text += "<div><HR/>&nbsp;</div>";
    QString htmlText = "<html><body>"+text+"</body></html>";
    printBox(painter, xsdPrintInfo, htmlText );

    return 0;
}

//void XSDWindow::printSchemaEnd(QPainter *painter, XSDPrintInfo &xsdPrintInfo)
    void XSDWindow::printSchemaEnd(QPainter *, XSDPrintInfo &)
{
    Utils::TODO_THIS_RELEASE("finire");
}


int XSDWindow::printSchemaInfo(QPainter *painter, XSDPrintInfo &xsdPrintInfo, XSDSchema *schema)
{
    Utils::TODO_THIS_RELEASE("finire");

    QDateTime now = QDateTime::currentDateTime();
    QString nowString = now.toString("yyyy-MM-dd HH:mm");
    QString text ;
    QString name = QString("<span style=''>%1: %3</span><br/><span>%2 %4</span><br/><span>%5: %6</span><br/>")
            .arg(tr("Schema")).arg(tr("Printed on"))
            .arg(Utils::escapeHTML(schema->location()))
                 .arg(nowString)
            .arg(tr("Filename")).arg(fileName);
    name += QString("<span style=''>%1: %3</span><br/><span>%2: %4</span><br/>")
            .arg(tr("TargetNamespace"))
            .arg(tr("Default namespace"))
            .arg(Utils::escapeHTML(schema->targetNamespace()))
                 .arg(schema->defaultNamespace());
    text = QString("<div>&nbsp;</div><div style=''>%1</div>").arg(name);

    QString htmlText = "<html><body>"+text+"</body></html>";
    printBox(painter, xsdPrintInfo, htmlText );

    return 0;
}