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
#include "xsdwindow.h"
#include "utils.h"
#include "ui_xsdwindow.h"
#include "qxmleditconfig.h"
#include "xsdeditor/widgets/xsdgenericeditor.h"
#include "xsdeditor/choosexsdviewrootitemdialog.h"
#include "modules/services/anotifier.h"
#include "modules/services/systemservices.h"
#include <QSvgGenerator>
#include <QDesktopServices>
#include <QTextStream>
#include <QTextCodec>
#include <QTemporaryFile>
#include <QFileInfo>


const QString XSDPrint::ElementAnchor("E");
const QString XSDPrint::TypeAnchor("T");
const QString XSDPrint::AttributeGroupAnchor("W");
const QString XSDPrint::AttributeAnchor("A");
const QString XSDPrint::GroupAnchor("G");
const QString XSDPrint::InnerElementsAnchor("N");
const QString XSDPrint::ReferencesAnchor("R");

//-------------------------------------------------------
XSDPrintConfig::XSDPrintConfig()
{
    diagramImageExternal = !Config::getBool(Config::KEY_XSD_REPORT_EMBEDIMAGES, true) ;
    imageType = Config::getBool(Config::KEY_XSD_REPORT_USEGRAPHVIZ, false) ? ImageDotViz : ImageRaster;
}

XSDPrintConfig::~XSDPrintConfig()
{
}

void XSDPrintConfig::init()
{
    diagramImageExternal = !Config::getBool(Config::KEY_XSD_REPORT_EMBEDIMAGES, true) ;
    imageType = Config::getBool(Config::KEY_XSD_REPORT_USEGRAPHVIZ, false) ? ImageDotViz : ImageRaster;
    if(Config::getBool(Config::KEY_XSD_REPORT_OVERRIDEGRAPHVIZ, false)) {
        dotVizPath = Config::getString(Config::KEY_XSD_REPORT_PATHGRAPHVIZ, "");
    } else {
        dotVizPath = "" ;
    }
}

//-------------------------------------------------------

#ifdef QXMLEDIT_TEST
XSDPrint::XSDPrint()
{
    _window = NULL ;
    _appData = NULL ;
}
#endif

XSDPrint::XSDPrint(XSDWindow *window, QXmlEditData *appData)
{
    _window = window ;
    _appData = appData ;
    _config.init();
}

XSDPrint::~XSDPrint()
{
}

QXmlEditData *XSDPrint::getAppData() const
{
    return _appData;
}

void XSDPrint::setAppData(QXmlEditData *appData)
{
    _appData = appData;
}

void XSDPrint::printPDFToFile(const QString &filePath, const bool isSimple)
{
    enterPrintMethod();
    printPDFToFileInternal(filePath, isSimple);
    exitPrintMethod();
}

void XSDPrint::printPDFToFileInternal(const QString &filePath, const bool isSimple)
{
    QString oldTitle = _window->windowTitle();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(filePath);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setColorMode(QPrinter::Color);
    printer.setCreator(QString("%1").arg(VERSION_STRING));
    printer.setDocName(QObject::tr("Schema %1").arg(_window->fileName()));

    QPainter painter(&printer);
    QRectF pageNumberArea;
    calculatePageRect(&painter, pageNumberArea);
    // width of a printer page in mm
    QRectF pageRectWithoutFooter = printer.pageRect();
    QRectF pageRectInDevicePoints = printer.pageRect();
    const double footerHeight = pageNumberArea.height();

    // take out page position
    double resYPrinter = printer.logicalDpiY();
    double resXPrinter = printer.logicalDpiX();
    if((0 == resYPrinter) || ((0 == resXPrinter))) {
        Utils::error(_window, QObject::tr("Error in calculating printer resolution."));
        return ;
    }
    pageRectWithoutFooter.setHeight(pageRectWithoutFooter.height() - footerHeight);

    XSDPrintInfo xsdPrintInfo ;
    QFileInfo fileInfo(_window->fileName());
    xsdPrintInfo.fileName = fileInfo.fileName();
    xsdPrintInfo.setSimple(isSimple);
    xsdPrintInfo.setPrinter(&printer, &painter, pageRectInDevicePoints, _window->logicalDpiX());

    // scene resolution
    double resXVideo = _window->physicalDpiX();
    double resYVideo = _window->physicalDpiY();
    // find the dimensions of a printer page on scene
    // abs width value * device resolution -> device points
    // la stampa deve contenere il doppio di quello che vedo a video.
    double pageWidthScene = pageRectWithoutFooter.width() * (resXVideo / resXPrinter) * 2.5;
    double pageHeightScene = pageRectWithoutFooter.height() * (resYVideo / resYPrinter) * 2.5;

    // maps
    if((0 == pageWidthScene) || (0 == pageHeightScene)) {
        Utils::error(_window, QObject::tr("Error in calculating scene dimensions."));
        return ;
    }

    // calculate the number of pages: how many times the scene can be divided by the page dimensions
    // find the number of pages
    QPointF offset = scene()->sceneRect().topLeft();
    const double dnumberOfPagesInARow = scene()->sceneRect().width() / pageWidthScene  ;
    const double dnumberOfPagesInAColumn = scene()->sceneRect().height() / pageHeightScene  ;

    // round fractional values to integer
    int numberOfPagesInARow = int(dnumberOfPagesInARow);
    int numberOfPagesInAColumn = int(dnumberOfPagesInAColumn);

    // check for factional pages
    if((dnumberOfPagesInARow - numberOfPagesInARow) > 0) {
        numberOfPagesInARow++;
    }
    if(dnumberOfPagesInAColumn - numberOfPagesInAColumn > 0) {
        numberOfPagesInAColumn++;
    }
    // last check
    if(numberOfPagesInAColumn == 0) {
        numberOfPagesInAColumn = 1;
    }
    if(numberOfPagesInARow == 0) {
        numberOfPagesInARow = 1;
    }
    qreal offsetX = 0 ;
    qreal offsetY = 0 ;
    if((1 == numberOfPagesInARow) && (1 == numberOfPagesInAColumn)) {
        // scale up to 100%
        const double maxVal = qMax(dnumberOfPagesInARow, dnumberOfPagesInAColumn);
        const bool isHorizTheMinVal = dnumberOfPagesInARow > dnumberOfPagesInAColumn;
        const double k = 1 / maxVal ;
        pageWidthScene /= k ;
        pageHeightScene /= k;
        if(isHorizTheMinVal) {
            qreal spaceLeft = pageRectWithoutFooter.width() * (1 - dnumberOfPagesInAColumn) ;
            if(spaceLeft > 0) {
                offsetX = spaceLeft;
            }
        } else {
            qreal spaceLeft = pageRectWithoutFooter.height() * (1 - dnumberOfPagesInARow) ;
            if(spaceLeft > 0) {
                offsetY = spaceLeft;
            }
        }
    }

    QBrush solidBrush(Qt::NoBrush);
    QBrush oldBrush = scene()->backgroundBrush();
    scene()->setBackgroundBrush(solidBrush);
    // for all the pages
    const int numPages = numberOfPagesInARow * numberOfPagesInAColumn ;
    printSchemaData(xsdPrintInfo, true);
    xsdPrintInfo.totalPages = numPages +  xsdPrintInfo.pages + 1 ;
    xsdPrintInfo.pages = 1 ;
    int currentPage = 0;
    QRectF sourceArea;
    painter.save();
    xsdPrintInfo.reset();
    printSchemaInfo(xsdPrintInfo, schema()->root()->schema());
    currentPage++;
    for(int pageRow = 0 ; pageRow < numberOfPagesInARow ; pageRow ++) {
        for(int pageColumn = 0 ; pageColumn < numberOfPagesInAColumn ; pageColumn ++) {
            currentPage++;
            printer.newPage();
            sourceArea.setRect(offset.x() + pageRow * pageWidthScene, offset.y() + pageColumn * pageHeightScene, pageWidthScene, pageHeightScene);
            //printf("x %g y %g w %g h %g\n", pageRow * pageWidthScene, pageColumn * pageHeightScene, pageWidthScene, pageHeightScene);
            // is next instruction useful?
            painter.fillRect(painter.window(), QColor(255, 255, 255, 0));
            QRectF dest = pageRectWithoutFooter ;
            if(offsetX > 0) {
                dest.moveTop(offsetX / 2);
            }
            if(offsetY > 0) {
                dest.moveLeft(offsetY / 2);
            }
            paintScene(&xsdPrintInfo, &painter, sourceArea, dest, currentPage, xsdPrintInfo.totalPages, pageColumn, pageRow);
        }
    }
    painter.restore();
    xsdPrintInfo.pages = currentPage ;
    printSchemaData(xsdPrintInfo, false);
    painter.end();

    scene()->setBackgroundBrush(oldBrush);
    // end print
    _window->setWindowTitle(oldTitle);
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
        Utils::message(_window, QObject::tr("The diagram was exported in PDF format, but %1 is unable to start the associated application.").arg(APPLICATION_NAME));
    } else {
        _appData->notifier()->notify(NULL, QObject::tr("Diagram exported in PDF format."));
    }
}

void XSDPrint::initPrintInfoForHTML(XSDPrintInfoHTML &xsdPrintInfo, const bool qtSyntax, const bool isSimple)
{
    xsdPrintInfo.qtSyntax = qtSyntax ;
    xsdPrintInfo.setSimple(isSimple);
    xsdPrintInfo.initForHTML(_window);
}

bool XSDPrint::saveHTMLToFile(const QString &fileName, const bool isSimple)
{
    enterPrintMethod();
    bool isOK = false ;
    XSDPrintInfoHTML xsdPrintInfo ;
    initPrintInfoForHTML(xsdPrintInfo, false, isSimple);
    QString htmlText = innerGetAsHTML(xsdPrintInfo, true, fileName, false);
    if(Utils::writeStringToFile(fileName, htmlText)) {
        if(createExternalImageForHTML(fileName)) {
            isOK = true ;
        }
    }
    exitPrintMethod();
    return isOK;
}

// this one put images inline
QString XSDPrint::getAsHTML(const bool qtSyntax, const bool insertImages, const bool isSimple)
{
    enterPrintMethod();
    QString result = getAsHTML(qtSyntax, insertImages, isSimple, true);
    exitPrintMethod();
    return result ;
}

QString XSDPrint::getAsHTML(const bool qtSyntax, const bool insertImages, const bool isSimple, const bool forceInlineImages)
{
    XSDPrintInfoHTML xsdPrintInfo ;
    initPrintInfoForHTML(xsdPrintInfo, qtSyntax, isSimple);
    return innerGetAsHTML(xsdPrintInfo, insertImages, "", forceInlineImages);
}

QString XSDPrint::innerGetAsHTML(XSDPrintInfoHTML &xsdPrintInfo, const bool insertImages, const QString &fileName, const bool forceImagesInline)
{
    QFileInfo fileInfo(_window->fileName());
    xsdPrintInfo.fileName = fileInfo.fileName();
    printSchemaInfo(xsdPrintInfo, schema()->root()->schema());
    if(insertImages) {
        QRectF rect = scene()->sceneRect();
        int pageWidthScene = rect.width();
        int pageHeightScene = rect.height();
        if((0 != pageWidthScene) && (0 != pageHeightScene)) {
            QString htmlText ;
            if(_config.diagramImageExternal && !forceImagesInline) {
                QString relativeName = relativeImageFileFromHTMLFile(fileName);
                htmlText = QString("&nbsp;<br/><img class='diagramImage' src='%1'/>").arg(relativeName);
            } else {
                QByteArray resultRawBytes = createInternalImageForHTML();
                QString imageData ;
                if(resultRawBytes.length() > 0) {
                    QByteArray resultBytes = resultRawBytes.toBase64();
                    QString imageResData(resultBytes);
                    imageData = imageResData ;
                } else {
                    Utils::error(_window, QObject::tr("Unable to generate the diagram image."));
                }
                htmlText = QString("&nbsp;<br/><img class='diagramImage' src='data:image/png;base64, %1' alt='%2'/>")
                           .arg(imageData).arg(Utils::escapeHTML(QObject::tr("Diagram Image")));
            }
            xsdPrintInfo.printBox(htmlText);
        }
    }
    printSchemaData(xsdPrintInfo, false);

    return xsdPrintInfo.text();
}

bool XSDPrint::createExternalImageForHTML(const QString &fileName)
{
    if(!_config.diagramImageExternal) {
        return true;
    }
    switch(_config.imageType) {
    case XSDPrintConfig::ImageDotViz:
        return createExternalDotVizImageForHTML(fileName);
        break;
    case XSDPrintConfig::ImageRaster:
        return createExternalRasterImageForHTML(fileName);
        break;
    default:
        Utils::error(_window, QObject::tr("Bad image type."));
        return false;
    }
}

QByteArray XSDPrint::createInternalImageForHTML()
{
    switch(_config.imageType) {
    case XSDPrintConfig::ImageDotViz:
        return createInternalDotVizImageForHTML();
        break;
    case XSDPrintConfig::ImageRaster:
        return createRasterImageForHTML();
        break;
    default:
        Utils::error(_window, QObject::tr("Bad image type."));
        return QByteArray();
    }
}

bool XSDPrint::createExternalRasterImageForHTML(const QString &fileName)
{
    bool isOk = false;
    QString imageFileAbsPath = imageFileFromHTMLFile(fileName);
    QByteArray imageData = createRasterImageForHTML();
    if(XSDPrintConfig::ImageRaster == _config.imageType) {
        if(Utils::writeDataToFile(imageFileAbsPath, imageData)) {
            isOk = true ;
        } else {
            Utils::error(_window, QObject::tr("Error generating the diagram"));
        }
    }
    return isOk ;
}

QByteArray XSDPrint::createRasterImageForHTML()
{
    QRectF rect = scene()->sceneRect();
    int pageWidthScene = rect.width();
    int pageHeightScene = rect.height();
    if((0 != pageWidthScene) && (0 != pageHeightScene)) {
        QBrush solidBrush(QColor::fromRgb(255, 255, 255));
        QBrush oldBrush = scene()->backgroundBrush();
        if(XSDPrintConfig::ImageRaster == _config.imageType) {
            scene()->setBackgroundBrush(solidBrush);

            QRectF sourceArea;
            QPixmap pixmap(pageWidthScene, pageHeightScene);
            QPainter painter(&pixmap);
            painter.save();
            sourceArea.setRect(0, 0, pageWidthScene, pageHeightScene);
            //printf("x %g y %g w %g h %g\n", pageRow * pageWidthScene, pageColumn * pageHeightScene, pageWidthScene, pageHeightScene);
            painter.fillRect(sourceArea, QColor(255, 255, 255, 0));
            QList<QGraphicsItem*> items = scene()->selectedItems();
            scene()->clearSelection();

            scene()->render(&painter);
            _window->restoreSelection(items);
            painter.restore();
            scene()->setBackgroundBrush(oldBrush);
            QBuffer imgBuffer;
            imgBuffer.open(QIODevice::WriteOnly);
            if(!pixmap.save(&imgBuffer, "PNG")) {
                Utils::error(_window, QObject::tr("Unable to generate the image."));
            }
            imgBuffer.close();
            QByteArray resultRawBytes = imgBuffer.data();
            return resultRawBytes ;
        }
    }
    return QByteArray() ;
}

void XSDPrint::calculatePageRect(QPainter *painter, QRectF &destArea)
{
    QFontMetrics fm = painter->fontMetrics();
    QString text = QString(QObject::tr("Page %1/%1")).arg(999).arg(999);
    QRectF measRect = fm.boundingRect(text);
    int x = destArea.left() + (destArea.width() - measRect.width()) / 2;
    int y = destArea.bottom() - measRect.height();
    destArea.setRect(x, y, measRect.width() + measRect.width() / 10, measRect.height());
}

//TODO: error checking
void XSDPrint::paintScene(XSDPrintInfo *xsdPrintInfo, QPainter *painter, const QRectF &sourceArea, const QRectF &destArea, const int pageNumber, const int totalPages, const int row, const int column)
{
    QList<QGraphicsItem*> items = scene()->selectedItems();
    scene()->clearSelection();

    scene()->render(painter, destArea, sourceArea);
    if((totalPages != 0) && (NULL != xsdPrintInfo)) {
        xsdPrintInfo->printPageNumber(pageNumber, totalPages);

        QFontMetrics fm = painter->fontMetrics();
        QString text = QString(QObject::tr("Row: %1 Col: %2")).arg((row + 1)).arg((column + 1));
        QRectF measRect = fm.boundingRect(text);
        float x = destArea.left() ;
        float y = destArea.bottom() ;
        QRectF drawRect(x, y, measRect.width() * 2, measRect.height());
        painter->drawText(drawRect, text);
        //printf("page number: x %g y %g w %g h %g\n", drawRect.left(), drawRect.top(), drawRect.width(), drawRect.height());
        //printf("  dest area : x %g y %g w %g h %g\n", destArea.left(), destArea.top(), destArea.width(), destArea.height());*/
    }
    _window->restoreSelection(items);
}

void XSDPrint::printSchemaData(XSDPrintInfo &xsdPrintInfo, const bool isCalculating)
{
    xsdPrintInfo.isCalculating = isCalculating ;
    printSchemaIndex(xsdPrintInfo);
    printSchemaIntroduction(xsdPrintInfo);
    printSchemaElements(xsdPrintInfo);
    printSchemaTypes(xsdPrintInfo);
    printSchemaInnerElements(xsdPrintInfo);
    printSchemaGroups(xsdPrintInfo);
    printSchemaAttributes(xsdPrintInfo);
    printSchemaAttributeGroups(xsdPrintInfo);
    printSchemaEnd(xsdPrintInfo);
}

void XSDPrint::printSchemaIndex(XSDPrintInfo &xsdPrintInfo)
{
    Q_UNUSED(xsdPrintInfo);
    QString text;
    printIndexStart(text);
    printIndexSchemaIntroduction(xsdPrintInfo, text);
    printIndexSchemaElements(xsdPrintInfo, text);
    if(!xsdPrintInfo.isSimple()) {
        printIndexSchemaTypes(xsdPrintInfo, text);
    }
    printIndexSchemaInnerElements(xsdPrintInfo, text);
    printIndexSchemaGroups(xsdPrintInfo, text);
    printIndexSchemaAttributes(xsdPrintInfo, text);
    printIndexSchemaAttributeGroups(xsdPrintInfo, text);
    printIndexEnd(xsdPrintInfo, text);
}

void XSDPrint::printIndexStart(QString &text)
{
    text = QString("<div class='indexDiv'>%1<ul>").arg(QObject::tr("Index"));
}

void XSDPrint::printIndexEnd(XSDPrintInfo &xsdPrintInfo, QString &text)
{
    text += QString("</ul></div>");
    xsdPrintInfo.printBox(text);
}

bool XSDPrint::printSchemaIndexList(XSDPrintInfo &xsdPrintInfo, QString &text, QList<XSchemaObject*> data, const QString &type, const QString &label)
{
    if(data.isEmpty()) {
        return false ;
    }
    text += "<li>";
    if(xsdPrintInfo.useLinks) {
        text += QString("<a href='#%1'>").arg(headerNameForLink(type));
    }
    text += Utils::escapeHTML(label);
    if(xsdPrintInfo.useLinks) {
        text += "</a>";
    }
    text += "</li>";
    return true ;
}

void XSDPrint::printIndexSchemaAttributeGroups(XSDPrintInfo &xsdPrintInfo, QString &text)
{
    printSchemaIndexList(xsdPrintInfo, text, schema()->root()->schema()->topLevelAttributeGroups(false), AttributeGroupAnchor, nameAttributeGroups());
}

void XSDPrint::printIndexSchemaAttributes(XSDPrintInfo &xsdPrintInfo, QString &text)
{
    printSchemaIndexList(xsdPrintInfo, text, schema()->root()->schema()->topLevelAttributes(false), AttributeAnchor, nameAttributes());
}

void XSDPrint::printIndexSchemaGroups(XSDPrintInfo &xsdPrintInfo, QString &text)
{
    printSchemaIndexList(xsdPrintInfo, text, schema()->root()->schema()->topLevelGroups(false), GroupAnchor, nameGroups());
}

void XSDPrint::printIndexSchemaInnerElements(XSDPrintInfo &xsdPrintInfo, QString &text)
{
    printSchemaIndexList(xsdPrintInfo, text, schema()->root()->schema()->allInnerElements(false), InnerElementsAnchor, nameInnerElements());
}

void XSDPrint::printIndexSchemaTypes(XSDPrintInfo &xsdPrintInfo, QString &text)
{
    printSchemaIndexList(xsdPrintInfo, text, schema()->root()->schema()->topLevelTypes(false), TypeAnchor, nameTypes());
}

void XSDPrint::printIndexSchemaElements(XSDPrintInfo &xsdPrintInfo, QString &text)
{
    printSchemaIndexList(xsdPrintInfo, text, schema()->root()->schema()->topLevelElements(false), ElementAnchor, nameElements());
}

void XSDPrint::printIndexSchemaIntroduction(XSDPrintInfo &xsdPrintInfo, QString &text)
{
    printSchemaIndexList(xsdPrintInfo, text, schema()->root()->schema()->topLevelReferences(), ReferencesAnchor, nameReferences());
}

QString XSDPrint::nameAttributeGroups()
{
    return QObject::tr("Attribute Groups");
}

QString XSDPrint::nameAttributes()
{
    return QObject::tr("Attribute Groups");
}

QString XSDPrint::nameGroups()
{
    return QObject::tr("Groups");
}

QString XSDPrint::nameInnerElements()
{
    return QObject::tr("Inner Elements");
}

QString XSDPrint::nameTypes()
{
    return QObject::tr("Types");
}

QString XSDPrint::nameElements()
{
    return QObject::tr("Elements");
}

QString XSDPrint::nameReferences()
{
    return QObject::tr("Schema includes and references");
}

void XSDPrint::printSchemaIntroduction(XSDPrintInfo &xsdPrintInfo)
{
    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelReferences();
    if(objects.isEmpty()) {
        return ;
    }
    printHeader(xsdPrintInfo, nameReferences(), ReferencesAnchor);

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
        printInclude(xsdPrintInfo, incl);
    }
    foreach(XSchemaImport*imp, imps) {
        printImport(xsdPrintInfo, imp);
    }
    foreach(XSchemaRedefine*redef, redefs) {
        printRedefine(xsdPrintInfo, redef);
    }
}

void XSDPrint::printSchemaElements(XSDPrintInfo &xsdPrintInfo)
{
    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelElements(false);
    if(objects.isEmpty()) {
        return ;
    }
    printHeader(xsdPrintInfo, nameElements(), ElementAnchor);

    objects = XSchemaObject::sortObjectsByName(objects);
    foreach(XSchemaObject * object, objects) {
        XSchemaElement* schemaObject = static_cast<XSchemaElement*>(object);
        printSingleElement(xsdPrintInfo, schemaObject);
    }
}

void XSDPrint::printSchemaInnerElements(XSDPrintInfo &xsdPrintInfo)
{
    QList<XSchemaObject*>objects = schema()->root()->schema()->allInnerElements(false);
    if(objects.isEmpty()) {
        return ;
    }
    printHeader(xsdPrintInfo, nameInnerElements(), InnerElementsAnchor);
    objects = XSchemaObject::sortObjectsByName(objects);
    QSet<QString> innerElementsPrinted;
    foreach(XSchemaObject * object, objects) {
        XSchemaElement* schemaObject = static_cast<XSchemaElement*>(object);
        if(!schemaObject->ref().isEmpty() || !schemaObject->xsdType().isEmpty()) {
            QString key = QString("%1<%2<%3").arg(schemaObject->name()).arg(schemaObject->ref()).arg(schemaObject->xsdType());
            if(innerElementsPrinted.contains(key)) {
                continue;
            }
            innerElementsPrinted.insert(key);
        }
        printSingleElement(xsdPrintInfo, schemaObject);
    }
}

void XSDPrint::printSchemaTypes(XSDPrintInfo &xsdPrintInfo)
{
    if(xsdPrintInfo.isSimple()) {
        return ;
    }

    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelTypes(false);
    if(objects.isEmpty()) {
        return ;
    }
    printHeader(xsdPrintInfo, nameTypes(), TypeAnchor);

    objects = XSchemaObject::sortObjectsByName(objects);
    foreach(XSchemaObject * object, objects) {
        XSchemaElement* schemaObject = static_cast<XSchemaElement*>(object);
        printSingleType(xsdPrintInfo, schemaObject);
    }
}

void XSDPrint::printHeader(XSDPrintInfo &xsdPrintInfo, const QString &headerText, const QString &key)
{
    xsdPrintInfo.newPage();
    QString text ;
    text = QString("<div class='header'><a name='%2'><H1 class='headerText'>%1</H1></a></div><div><br/></div>")
           .arg(headerText).arg(headerNameForLink(key));
    xsdPrintInfo.printBox(text);
}

QString XSDPrint::printAnnotationString(XSchemaObject *object, const QString className)
{
    QString annotationHtml ;
    if(NULL != object) {
        QString annotationInfo;
        if(NULL != object->annotation()) {
            annotationInfo = object->annotation()->text();
            if(!annotationInfo.isEmpty()) {
                annotationHtml = QString("<div class='%1'>%2</div>")
                                 .arg(className)
                                 .arg(Utils::escapeHTML(annotationInfo));
            }
        }
    }
    return annotationHtml;
}

/**
 * @brief XSDPrint::printSingleElement
 * @param painter
 * @param headerText
 * @return
 * ELEMENTNAME  x
 *     type:x|ref: y
 *     info: xxxxx xxx xxx
 *           xxxxxxxxxxxxxx
 *           xxxx xxxx xxx
 */
int XSDPrint::printSingleElement(XSDPrintInfo &xsdPrintInfo, XSchemaElement *element)
{
    QString text ;
    XTypeQueryInfo typeInfo ;
    element->getTypeInfoAndRestrictions(typeInfo);
    bool hasRefToType = false;
    bool hasRefToElement = false;
    QString elementText = QString("<a name='%2'><span class='nameElement'>%1</span></a>")
                          .arg(Utils::escapeHTML(element->name()))
                          .arg(htmlANameForObject(ElementAnchor, element));
    XSchemaElement *referredElement = NULL ;
    if(!element->ref().isEmpty()) {
        QString targetLink ;
        if(xsdPrintInfo.useLinks) {
            referredElement = schema()->root()->schema()->topLevelElement(element->ref());
            if(NULL != referredElement) {
                targetLink = htmlANameForObject(ElementAnchor, referredElement);
            }
        }
        elementText += QString("&nbsp;&nbsp;");
        if(!targetLink.isEmpty()) {
            elementText += QString("<a href='#%1'>").arg(targetLink);
        }
        elementText += QString("&nbsp;&nbsp;<span class='referenceElement'>%1 \"%2\"</span>").arg(QObject::tr("reference to")).arg(Utils::escapeHTML(element->ref()));
        if(!targetLink.isEmpty()) {
            elementText += QString("</a>");
        }
        hasRefToElement = true ;
    } else {
        QString typeName ;
        QString type ;
        if(!element->isTypeOrElement() && !element->xsdType().isEmpty()) {
            type += QObject::tr("type %1").arg(element->xsdType());
            typeName = element->xsdType();
            hasRefToType = true ;
        } else {
            if(typeInfo.isExtension()) {
                type += QObject::tr("extension of \"%1\"").arg(typeInfo.name());
                typeName = typeInfo.qualifiedTypeName();
            } else if(typeInfo.isRestriction()) {
                type += QObject::tr("restriction of \"%1\"").arg(typeInfo.name());
                typeName = typeInfo.qualifiedTypeName();
            }
        }
        elementText += QString("&nbsp;&nbsp;");
        QString targetLink ;
        if(xsdPrintInfo.useLinks && !typeName.isEmpty()) {
            if(!schema()->root()->schema()->isBaseType(typeName) && !xsdPrintInfo.isSimple()) {
                XSchemaElement *target = schema()->root()->schema()->topLevelType(typeName);
                if(NULL != target) {
                    targetLink = htmlANameForObject(TypeAnchor, target);
                }
            }
        }
        if(!targetLink.isEmpty()) {
            elementText += QString("<a href='#%1'>").arg(targetLink);
        }
        elementText += QString("<span class='referenceType'>%1</span>").arg(Utils::escapeHTML(type));
        if(!targetLink.isEmpty()) {
            elementText += QString("</a>");
        }
    }
    text = QString("<div>&nbsp;</div><div class='singleElementHeader'>%1</div>").arg(elementText);
    text += printAnnotationString(element, "annotationString");

    if(hasRefToElement && (NULL != referredElement)) {
        if(xsdPrintInfo.isSimple()) {
            printElementChildrenInfo(xsdPrintInfo, text, element);
            if(referredElement->xsdType().isEmpty()) {
                text += getSingleTypeInner(xsdPrintInfo, referredElement);
            } else {
                QString theTypeName = referredElement->xsdType();
                XSchemaElement *refElementType = schema()->root()->schema()->topLevelType(theTypeName);
                if(NULL != refElementType) {
                    text += getSingleTypeInner(xsdPrintInfo, refElementType);
                }
            }
        }
    } else if(hasRefToType) {
        if(xsdPrintInfo.isSimple()) {
            printElementChildrenInfo(xsdPrintInfo, text, element);
            XSchemaElement *theType = schema()->root()->schema()->topLevelType(element->xsdType());
            if(NULL != theType) {
                text += getSingleTypeInner(xsdPrintInfo, theType);
            }
        }
    } else {
        printElementChildrenInfo(xsdPrintInfo, text, element);
        if(typeInfo.hasEnumOrFacets()) {
            text += QObject::tr("<br/>Allowed values:");
            if(typeInfo.hasEnum()) {
                text += QObject::tr("<ul class='ulEnumElement'>");
                appendEnums(text, typeInfo);
                text += "</ul>";
            }
            appendOtherFacets(text, typeInfo);
        }
        text += getAttributesOfElement(xsdPrintInfo, element);
    }
    text += XSDPrintInfo::EndSeparator ;
    xsdPrintInfo.printBox(text);

    return 0;
}

void XSDPrint::appendEnums(QString &text, XTypeQueryInfo &typeInfo)
{
    foreach(const QString &facet, typeInfo.enums()) {
        text += "<li>";
        text += Utils::escapeHTML(facet);
        text += "</li>";
    }
}

void XSDPrint::appendOtherFacets(QString &text, XTypeQueryInfo &typeInfo)
{
    if(typeInfo.hasOtherFacets()) {
        const QString theText = QObject::tr("Other Restrictions");
        text += QString("<br/>%1:<ul class='ulEnumElement'>").arg(Utils::escapeHTML(theText));
        foreach(InfoFacet *facet, *typeInfo.otherFacets()) {
            text += "<li>";
            text += QString("%1: <B>%2</B>").arg(Utils::escapeHTML(facet->type())).arg(Utils::escapeHTML(facet->value()));
            text += "</li>";
        }
        text += "</ul>";
    }
}

void XSDPrint::printElementChildrenInfo(XSDPrintInfo &xsdPrintInfo, QString &elementText, XSchemaElement *element)
{
    XSchemaInquiryContext searchContext;
    searchContext.setHonorRestrictions(true);
    searchContext.setFullCollection(true);
    searchContext.setIsRecursive(false);

    XSchemaOutlineElement *xElement = new XSchemaOutlineElement(element->xsdParent(), element->root());
    xElement->setElement(searchContext, element);

    if(!xElement->getChildren().isEmpty()) {
        elementText += QString("<div>&nbsp;</div><div class='tableContainer'>\n<div class='elementChildIntro'>%1:</div><br/>\n").arg(Utils::escapeHTML(QObject::tr("children")));
        if(xElement->getChildren().size() == 1) {
            if(!addSingleChildrenElements(xsdPrintInfo, xElement, elementText)) {
                addChildrenElements(xsdPrintInfo, xElement, elementText);
            }
        } else {
            addChildrenElements(xsdPrintInfo, xElement, elementText);
        }
        elementText += "</div><br/>\n";
    }
    delete xElement ;
}

void XSDPrint::addChildrenElements(XSDPrintInfo &xsdPrintInfo, XSchemaOutlineContainer *xElement, QString &text)
{
    text += QObject::tr("<div>\n<table>\n<thead><tr><th class='tableHeader'>%1</th><th class='tableHeader'>%2</th></tr></thead>\n<tbody>\n")
            .arg(Utils::escapeHTML(QObject::tr("Name"))).arg(Utils::escapeHTML(QObject::tr("Description")));
    int index = -1 ;
    foreach(XSchemaObject *child, xElement->getChildren()) {
        index++;
        QString rowClass = xsdPrintInfo.tableRowClassForIndex(index);
        text += QString("<tr class='%1'>").arg(rowClass);
        XSchemaOutlineElement *childElement = qobject_cast<XSchemaOutlineElement*>(child);
        XSchemaObject *forAnnotation = NULL ;
        if(NULL != childElement) {
            text += QString("<td class='cellText'><span class='entityName'>");
            text += Utils::escapeHTML(childElement->schemaObject()->nameOrReference());
            forAnnotation = childElement->schemaObject();
            text += "</span>";
            if(NULL != childElement->schemaObject()) {
                if(childElement->schemaObject()->getType() == SchemaTypeElement) {
                    XSchemaElement *theChildElement = static_cast<XSchemaElement*>(childElement->schemaObject());
                    QString occurences = theChildElement->occurrencesDescr();
                    if(!occurences.isEmpty()) {
                        text += "&nbsp;&nbsp;";
                        text += "<span class='childOccurrences'>";
                        text += Utils::escapeHTML(occurences);
                        text += "</span>";
                    }
                }
            }
            text += "</td>";
            text += "<td class='cellContainerAnnotation cellAlignTop cellBorder'>";
            if(NULL != forAnnotation) {
                text += printAnnotationString(forAnnotation, "annotationString");
            }
            text += "</td>";
        } else {
            XSchemaOutlineContainer *childContainer = qobject_cast<XSchemaOutlineContainer *>(child);
            text += QString("<td class='cellContainer'>");
            paintChildContainer(xsdPrintInfo, childContainer, text);
            if(isContainerRecursive(childContainer)) {
                text += "</td><td class='cellContainer'/></tr>\n<tr><td class='cellContainer' colspan='2'>";
                addChildrenElements(xsdPrintInfo, childContainer, text);
                text += "</td>";
            }
        }
        text += QString("</tr>\n");
    }
    text += QString("<tbody>\n</table>\n</div>\n");
}

bool XSDPrint::isContainerRecursive(XSchemaOutlineContainer *container)
{
    switch(container->schemaObject()->getType()) {
    default:
        return true ;
    // for fakse
    case SchemaTypeAny:
    // fallthrough
    case SchemaTypeGroup:
        return false;
    }
}

bool XSDPrint::addSingleChildrenElements(XSDPrintInfo &xsdPrintInfo, XSchemaOutlineContainer *xElement, QString &text)
{
    if(xElement->getChildren().size() != 1) {
        return false;
    }
    XSchemaObject *child = xElement->getChildren().first();
    XSchemaOutlineContainer *childContainer = qobject_cast<XSchemaOutlineContainer *>(child);
    if(NULL == childContainer) {
        return false;
    }
    paintChildContainer(xsdPrintInfo, childContainer, text);
    if(childContainer->schemaObject()->getType() != SchemaTypeAny) {
        addChildrenElements(xsdPrintInfo, childContainer, text);
    }
    return true ;
}


void XSDPrint::paintChildContainer(XSDPrintInfo &xsdPrintInfo, XSchemaOutlineContainer *childContainer, QString &text)
{
    XSchemaObject *forAnnotation = NULL ;
    forAnnotation = childContainer->schemaObject();

    QString targetLink ;
    if(xsdPrintInfo.useLinks) {
        if(childContainer->getType() == SchemaTypeOutlineGroup) {
            if(NULL != childContainer->schemaObject()) {
                targetLink = htmlANameForObject(GroupAnchor, childContainer->schemaObject());
            }
        }
    }
    QString name ;
    if(NULL != childContainer->schemaObject()) {
        name += childContainer->schemaObject()->tagName();
        QString tName = childContainer->schemaObject()->nameOrReference();
        if(!tName.isEmpty()) {
            name += ' ';
            name += tName ;
        }
    }
    QString descr = childContainer->description();
    if(!descr.isEmpty()) {
        name += " ";
        name += descr;
    }
    if(!targetLink.isEmpty()) {
        text += QString("<a href='#%1'>").arg(targetLink);
    }
    text += "<span class='childContainer'>";
    text += Utils::escapeHTML(name);
    text += "</span>";
    if(!targetLink.isEmpty()) {
        text += QString("</a>");
    }
    if(NULL != forAnnotation) {
        text += "<br/>";
        text += printAnnotationString(forAnnotation, "annotationString");
    }
}


QString XSDPrint::getAttributesOfElement(XSDPrintInfo &xsdPrintInfo, XSchemaElement *element)
{
    QString text ;
    bool isReference = false;
    if(!element->ref().isEmpty()) {
        isReference = true ;
    }
    if(!isReference) {
        XSchemaAttributesCollection attributesCollection;
        attributesCollection.collectGroups = true ;
        element->collectAttributes(attributesCollection);
        if(!attributesCollection.attributes.isEmpty()) {
            text += QString("<div>&nbsp;</div><div class='tableContainer'>%1:<nbsp/>").arg(Utils::escapeHTML(QObject::tr("Attributes")));
            text += QObject::tr("<table>\n<thead><tr><th class='tableHeader'>%1</th><th class='tableHeader'>%2</th><th class='tableHeader'>%3</th><th class='tableHeader'>%4</th></tr></thead>\n<tbody>\n")
                    .arg(Utils::escapeHTML(QObject::tr("Name"))).arg(Utils::escapeHTML(QObject::tr("Use")))
                    .arg(Utils::escapeHTML(QObject::tr("Type"))).arg(Utils::escapeHTML(QObject::tr("Annotations")));
            int index = -1;
            foreach(const QString &attrName, attributesCollection.attributes.keys()) {
                index++;
                QString rowClass = xsdPrintInfo.tableAttributeRowClassForIndex(index);
                AttrCollectInfo* info = attributesCollection.attributes[attrName];
                text += QString("<tr class='%1'>").arg(rowClass);
                if(!info->isGroup()) {
                    text += QString("<td class='cellText cellBorder'><span class='entityName'>");
                    QString name ;
                    if(info->isReference) {
                        QString targetLink ;
                        if(xsdPrintInfo.useLinks) {
                            if(NULL != info->attribute) {
                                targetLink = htmlANameForObject(AttributeAnchor, info->attribute);
                            }
                        }
                        if(!targetLink.isEmpty()) {
                            name += QString("<a href='#%1'>").arg(targetLink);
                        }
                        name += Utils::escapeHTML(QObject::tr("reference to ") + attrName);
                        if(!targetLink.isEmpty()) {
                            name += QString("</a>");
                        }
                    } else {
                        name = Utils::escapeHTML(attrName);
                    }
                    text += name + "</span></td>";
                    QString useText ;
                    if(info->attribute->use() != XSchemaAttribute::Optional) {
                        useText = Utils::escapeHTML(XSchemaAttribute::toStringUse(info->attribute->use()));
                    }
                    if(info->attribute->isFixed()) {
                        if(!useText.isEmpty()) {
                            useText += QString("<HR/>");
                        }
                        useText += QString("<span class='attrNote'>%1:</span> <span class='attrData'>'%2'</span>").arg(Utils::escapeHTML(QObject::tr("fixed"))).arg(Utils::escapeHTML(info->attribute->fixed()));
                    } else if(info->attribute->hasDefaultValue()) {
                        if(!useText.isEmpty()) {
                            useText += QString("<HR/>");
                        }
                        useText += QString("<span class='attrNote'>%1:</span> <span class='attrData'>'%2'</span>").arg(Utils::escapeHTML(QObject::tr("default value"))).arg(Utils::escapeHTML(info->attribute->defaultValue()));
                    }
                    text += QString("<td class='cellText cellBorder'>%1</td>").arg(useText);
                    QString attrType ;
                    if(info->isTypeExtension) {
                        attrType += QObject::tr("extension of ");
                    } else if(info->isTypeRestriction) {
                        attrType += QObject::tr("restriction of ");
                    }
                    attrType += info->type ;

                    if(!info->enums.isEmpty()) {
                        attrType += QObject::tr("<HR/>&nbsp;Allowed values:<ul class='attrEnumList'>");
                        foreach(const QString &facet, info->enums) {
                            attrType += "<li>";
                            attrType += Utils::escapeHTML(facet);
                            attrType += "</li>";
                        }
                        attrType += "</ul>";
                    }

                    text += QString("<td class='cellText cellBorder'>%1</td>").arg(attrType);
                    text += "<td class='cellText cellAlignTop cellBorder'>";
                    text += printAnnotationString(info->originalAttribute, "annotationStringAttributes");
                    text += "</td>";
                } else {
                    text += "<td class='cellText cellBorder'>";
                    XSchemaAttributeGroup *ag = info->attributeGroup;
                    XSchemaAttributeGroup *agForAnnotations = info->originalAttributeGroup;

                    QString targetLink ;
                    if(xsdPrintInfo.useLinks) {
                        if(NULL != ag) {
                            targetLink = htmlANameForObject(AttributeGroupAnchor, ag);
                        }
                    }
                    if(!targetLink.isEmpty()) {
                        text += QString("<a href='#%1'>").arg(targetLink);
                    }
                    text += "Group: " + Utils::escapeHTML((NULL != ag) ? ag->name() : "?") ;
                    if(!targetLink.isEmpty()) {
                        text += QString("</a>");
                    }
                    text += "</td>";
                    text += "<td class='tableAGroupName'>---</td><td class='tableAGC1'>---</td>";
                    text += "<td class='cellText cellBorder'>";
                    QString annotation = printAnnotationString(agForAnnotations, "annotationStringAttributes");
                    if(annotation.isEmpty()) {
                        annotation = printAnnotationString(ag, "annotationStringAttributes");
                    }
                    text += annotation ;
                    text += "</td>";
                }
                text += "</tr>\n";
            }
            text += QObject::tr("</tbody>\n</table>\n</div>");
        }
    }
    return text ;
}

int XSDPrint::printSingleType(XSDPrintInfo &xsdPrintInfo, XSchemaElement *element)
{
    QString text ;
    QString elementText = QString("<a name='%2'><span class='nameType'>%1</span></a>").arg(Utils::escapeHTML(element->name())).arg(htmlANameForObject(TypeAnchor, element));
    const QString baseType = QString("&nbsp;&nbsp;<span class='%1'>%2</span>");
    if(element->isSimpleType()) {
        elementText += baseType.arg("typeTypeSimpleLabel").arg(QObject::tr("simple"));
    } else {
        elementText += baseType.arg("typeTypeComplexLabel").arg(QObject::tr("complex"));
    }

    text = QString("<div>&nbsp;</div><div class='singleTypeHeader'>%1</div>").arg(elementText);
    const QString & annotation = printAnnotationString(element, "annotationString");
    if(!annotation.isEmpty()) {
        text += annotation ;
    }
    printElementChildrenInfo(xsdPrintInfo, text, element);
    text += getSingleTypeInner(xsdPrintInfo, element);
    text += XSDPrintInfo::EndSeparator ;
    xsdPrintInfo.printBox(text);

    return 0;
}

QString XSDPrint::getSingleTypeInner(XSDPrintInfo &xsdPrintInfo, XSchemaElement *element)
{
    QString text ;

    XTypeQueryInfo typeInfo ;
    element->getTypeInfoAndRestrictions(typeInfo);
    if(typeInfo.hasEnumOrFacets()) {
        text += QString("<br/>%1:").arg(QObject::tr("Allowed values"));
        if(typeInfo.hasEnum()) {
            text += QString("<ul class='ulEnumType'>");
            appendEnums(text, typeInfo);
            text += "</ul>";
        }
        appendOtherFacets(text, typeInfo);
    } else if(typeInfo.isSimpleTypeList()) {
        text += QString("<br/>%1: '%2'")
                .arg("list").arg(Utils::escapeHTML(typeInfo.listValue()));
    } else if(typeInfo.isSimpleTypeUnion()) {
        text += QString("<br/>%1: '%2'")
                .arg("union").arg(Utils::escapeHTML(typeInfo.unionValue()));
    }
    text += getAttributesOfElement(xsdPrintInfo, element);

    return text ;
}

int XSDPrint::printInclude(XSDPrintInfo &xsdPrintInfo, XSchemaInclude *object)
{
    QString text ;
    QString elementText = QString("<span class='references'>include %1</span>").arg(Utils::escapeHTML(object->schemaLocation()));
    text = QString("<div class='divReference'>%1</div>").arg(elementText);

    text += printAnnotationString(object);
    text += XSDPrintInfo::EndSeparator;
    xsdPrintInfo.printBox(text);

    return 0;
}

int XSDPrint::printImport(XSDPrintInfo &xsdPrintInfo, XSchemaImport *object)
{
    QString text ;
    QString elementText = QString("<span class='references'>import %1</span>").arg(Utils::escapeHTML(object->schemaLocation()));
    if(!object->targetNamespace().isEmpty()) {
        elementText += QString("<span class='references'>namespace %1</span>").arg(Utils::escapeHTML(object->targetNamespace()));
    }
    text = QString("<div class='divReference'>%1</div>").arg(elementText);

    text += printAnnotationString(object);
    text += XSDPrintInfo::EndSeparator;
    xsdPrintInfo.printBox(text);

    return 0;
}

int XSDPrint::printRedefine(XSDPrintInfo &xsdPrintInfo, XSchemaRedefine *object)
{
    QString text ;
    QString elementText = QString("<span class='references'>redefine %1</span>").arg(Utils::escapeHTML(object->schemaLocation()));
    text = QString("<div class='divReference'>%1</div>").arg(elementText);

    text += printAnnotationString(object);
    text += XSDPrintInfo::EndSeparator;
    xsdPrintInfo.printBox(text);

    return 0;
}

void XSDPrint::printSchemaGroups(XSDPrintInfo &xsdPrintInfo)
{
    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelGroups(false);
    if(objects.isEmpty()) {
        return ;
    }
    printHeader(xsdPrintInfo, nameGroups(), GroupAnchor);
    objects = XSchemaObject::sortObjectsByName(objects);
    foreach(XSchemaObject * object, objects) {
        XSchemaGroup* schemaGroup = static_cast<XSchemaGroup*>(object);
        printSingleGroup(xsdPrintInfo, schemaGroup);
    }
}

int XSDPrint::printSingleGroup(XSDPrintInfo &xsdPrintInfo, XSchemaGroup *group)
{
    QString text ;
    QString name = QString("<a name='%2'><span class='groupName'>%1</span></a>")
                   .arg(Utils::escapeHTML(group->name()))
                   .arg(htmlANameForObject(GroupAnchor, group));

    text = QString("<div>&nbsp;</div><div class='group'>%1</div>").arg(name);

    text += printAnnotationString(group);

    printGroupChildrenInfo(xsdPrintInfo, text, group);

    text += XSDPrintInfo::EndSeparator;
    xsdPrintInfo.printBox(text);

    return 0;
}

void XSDPrint::printGroupChildrenInfo(XSDPrintInfo &xsdPrintInfo, QString &text, XSchemaGroup *group)
{
    XSchemaInquiryContext searchContext;
    searchContext.setHonorRestrictions(true);
    searchContext.setFullCollection(true);
    searchContext.setIsRecursive(false);

    XSchemaOutlineGroup *xGroup = new XSchemaOutlineGroup(group->xsdParent(), group->root());
    xGroup->setSchemaObject(group);
    xGroup->collectOutlineContainerChildrenGroup(searchContext);

    if(!xGroup->getChildren().isEmpty()) {
        text += QString("<div>&nbsp;</div><div class='tableContainer'>\n<div class='elementChildIntro'>%1:</div><br/>\n").arg(Utils::escapeHTML(QObject::tr("children")));
        if(xGroup->getChildren().size() == 1) {
            if(!addSingleChildrenElements(xsdPrintInfo, xGroup, text)) {
                addChildrenElements(xsdPrintInfo, xGroup, text);
            }
        } else {
            addChildrenElements(xsdPrintInfo, xGroup, text);
        }
        text += "</div><br/>\n";
    }
    delete xGroup ;
}

void XSDPrint::printSchemaAttributes(XSDPrintInfo &xsdPrintInfo)
{
    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelAttributes(false);
    if(objects.isEmpty()) {
        return ;
    }
    printHeader(xsdPrintInfo, nameAttributes(), AttributeAnchor);
    objects = XSchemaObject::sortObjectsByName(objects);
    foreach(XSchemaObject * object, objects) {
        XSchemaAttribute* schemaAttribute = static_cast<XSchemaAttribute*>(object);
        printSingleAttribute(xsdPrintInfo, schemaAttribute);
    }
}

int XSDPrint::printSingleAttribute(XSDPrintInfo &xsdPrintInfo, XSchemaAttribute *attribute)
{
    QString text ;
    QString name = QString("<a name='%2'><span class='attributeName'>%1</span></a>")
                   .arg(Utils::escapeHTML(attribute->name()))
                   .arg(htmlANameForObject(AttributeAnchor, attribute));

    text = QString("<div>&nbsp;</div><div class='attributeNameDiv'>%1</div>").arg(name);

    text += printAnnotationString(attribute);
    text += XSDPrintInfo::EndSeparator ;
    xsdPrintInfo.printBox(text);

    return 0;
}

void XSDPrint::printSchemaAttributeGroups(XSDPrintInfo &xsdPrintInfo)
{
    QList<XSchemaObject*>objects = schema()->root()->schema()->topLevelAttributeGroups(false);
    if(objects.isEmpty()) {
        return ;
    }
    printHeader(xsdPrintInfo, nameAttributeGroups(), AttributeGroupAnchor);
    objects = XSchemaObject::sortObjectsByName(objects);
    foreach(XSchemaObject * object, objects) {
        XSchemaAttributeGroup* schemaAttributeGroup = static_cast<XSchemaAttributeGroup*>(object);
        printSingleAttributeGroup(xsdPrintInfo, schemaAttributeGroup);
    }
}

int XSDPrint::printSingleAttributeGroup(XSDPrintInfo &xsdPrintInfo, XSchemaAttributeGroup *attributeGroup)
{
    QString text ;
    QString name = QString("<a name='%2'><span class='attributeGroupName'>%1</span></a>")
                   .arg(Utils::escapeHTML(attributeGroup->name()))
                   .arg(htmlANameForObject(AttributeGroupAnchor, attributeGroup));

    text = QString("<div>&nbsp;</div><div class='attributeGroupNameDiv'>%1</div>").arg(name);
    text += printAnnotationString(attributeGroup);

    {
        bool firstAttribute = true;
        QString attrNames;
        foreach(XSchemaObject * object, attributeGroup->getChildren()) {
            if(object->getType() == SchemaTypeAttribute) {
                if(!firstAttribute) {
                    attrNames += ", ";
                } else {
                    firstAttribute = false ;
                }
                QString name = object->name() ;
                bool useLink = false;
                if(!object->referencedObjectName().isEmpty()) {
                    useLink = true ;
                    name = object->referencedObjectName() ;
                }
                QString targetLink ;
                if(xsdPrintInfo.useLinks && useLink) {
                    XSchemaAttribute *target = schema()->root()->schema()->topLevelAttribute(name);
                    if(NULL != target) {
                        targetLink = htmlANameForObject(AttributeAnchor, target);
                    }
                }
                if(!targetLink.isEmpty()) {
                    attrNames += QString("<a href='#%1'>").arg(targetLink);
                }
                attrNames += Utils::escapeHTML(object->name());
                if(!targetLink.isEmpty()) {
                    attrNames += QString("</a>");
                }
            }
        }
        if(!attrNames.isEmpty()) {
            text += QString("<div class='attrOfAGDiv'><span class='attrOfAGNames'>%2:</span> %1</div>")
                    .arg(attrNames)
                    .arg(QObject::tr("Attributes"));
        }
    }
    {
        bool firstAttributeGroup = true;
        QString attrGroupNames;
        foreach(XSchemaObject * object, attributeGroup->getChildren()) {
            if(object->getType() == SchemaTypeAttributeGroup) {
                if(!firstAttributeGroup) {
                    attrGroupNames += ", ";
                } else {
                    firstAttributeGroup = false ;
                }
                QString targetLink ;
                if(xsdPrintInfo.useLinks) {
                    XSchemaAttributeGroup *target = schema()->root()->schema()->topLevelAttributeGroup(object->referencedObjectName());
                    if(NULL != target) {
                        targetLink = htmlANameForObject(AttributeGroupAnchor, target);
                    }
                }
                if(!targetLink.isEmpty()) {
                    attrGroupNames += QString("<a href='#%1'>").arg(targetLink);
                }
                attrGroupNames += Utils::escapeHTML(object->referencedObjectName());
                if(!targetLink.isEmpty()) {
                    attrGroupNames += QString("</a>");
                }
            }
        }
        if(!attrGroupNames.isEmpty()) {
            text += QString("<div class='agOfAGDiv'><span class='agOfAGNames'>%2:</span> %1</div>")
                    .arg(attrGroupNames)
                    .arg(QObject::tr("Attribute groups"));
        }
    }
    text += XSDPrintInfo::EndSeparator;
    xsdPrintInfo.printBox(text);

    return 0;
}

void XSDPrint::printSchemaEnd(XSDPrintInfo & /*xsdPrintInfo*/)
{
    // nothing
}

int XSDPrint::printSchemaInfo(XSDPrintInfo &xsdPrintInfo, XSDSchema *schema)
{
    QDateTime now = QDateTime::currentDateTime();
    QString nowString = now.toString("yyyy-MM-dd HH:mm");
    QString text ;
    QFileInfo fileInfo(_window->fileName());
    QString name = QString("<br/><br/><span class='siFile'>%1: <!--PS1-->%2<!--PE1--></span><br/><br/><span>%3 <!--PS0-->%4<!--PE0--></span><br/><br/>")
                   .arg(QObject::tr("Filename")).arg(fileInfo.fileName())
                   .arg(QObject::tr("Printed on"))
                   .arg(nowString);
    name += QString("<span class='siNS'>%1: %3</span><br/><br/><span>%2: %4</span><br/><br/>")
            .arg(QObject::tr("TargetNamespace"))
            .arg(QObject::tr("Default namespace"))
            .arg(Utils::escapeHTML(schema->targetNamespace()))
            .arg(schema->defaultNamespace());
    name += QString("<span class='siNS'>attributeFormDefault: %1</span><br/><br/><span>elementFormDefault: %2</span><br/><br/><br/>")
            .arg(Utils::escapeHTML(schema->attributesQualifiedString()))
            .arg(Utils::escapeHTML(schema->elementsQualifiedString()));
    text = QString("<div>&nbsp;</div><div class='siSep'>&nbsp;</div><div class='siCenterBlock'>%1</div><div class='siSep'>&nbsp;</div>")
           .arg(name);

    xsdPrintInfo.printBox(text);

    return 0;
}

QString XSDPrint::htmlANameForObject(const QString &theType, XSchemaObject *element)
{
    QString address = QString::number((quintptr)element, pointerSize() * 2);
    QString result = QString("%1%2").arg(theType).arg(address);
    return result ;
}

QString XSDPrint::headerNameForLink(const QString &theType)
{
    QString result = QString("HDR_%1").arg(theType);
    return result ;
}

XSchemaObject *XSDPrint::schema() const
{
    return _window->schema();
}

XSDScene *XSDPrint::scene()
{
    return _window->scene() ;
}

// byte array is 0 length on error
QByteArray XSDPrint::createInternalDotVizImageForHTML()
{
    QTemporaryFile temporaryFile;
    if(temporaryFile.open()) {
        temporaryFile.close();
    } else {
        return QByteArray() ;
    }
    if(innerCreateExternalDotVizImageForHTML(temporaryFile.fileName())) {
        QByteArray result = Utils::readBytesFromFile(temporaryFile.fileName());
        return result;
    }
    return QByteArray() ;
}

bool XSDPrint::createExternalDotVizImageForHTML(const QString &fileName)
{
    QString imageFileName = imageFileFromHTMLFile(fileName);
    return innerCreateExternalDotVizImageForHTML(imageFileName);
}

QStringList XSDPrint::createDotVizCommandLine(const QString &commandFileName, const QString &imageFileName)
{
    // dot -Tpng dotFile.dot -o graphname.png
    QStringList arguments ;
    arguments << "-Tpng" ;
    arguments << commandFileName ;
    arguments << "-o" ;
    arguments << imageFileName ;
    return arguments ;
}

QString XSDPrint::dotVizPath()
{
    QString dotVizPath = _config.dotVizPath ;
    if(dotVizPath.isEmpty()) {
        dotVizPath = "dot";
        if(SystemServices::isWindows()) {
            dotVizPath += ".exe";
        }
    }
    return dotVizPath;
}

bool XSDPrint::innerCreateExternalDotVizImageForHTML(const QString &imageFileName)
{
    QString dotVizCommands = createDotVizCommands();
    QTemporaryFile fileTempViz;
    if(!Utils::writeStringToFile(&fileTempViz, dotVizCommands)) {
        Utils::error(_window, QObject::tr("Error creating chart"));
        return false;
    }
    QString theDotVizPath = dotVizPath();
    QProcess dotVizProcess;
    // dot -Tpng dotFile.dot -o graphname.png
    QStringList arguments = createDotVizCommandLine(fileTempViz.fileName(), imageFileName);
    dotVizProcess.start(theDotVizPath, arguments);
    if(!dotVizProcess.waitForStarted()) {
        Utils::error(_window, QObject::tr("GraphViz engine not started."));
        return false;
    }
    dotVizProcess.closeWriteChannel();
    if(!dotVizProcess.waitForFinished(TimeoutDotViz)) {
        Utils::error(_window, QObject::tr("Errors executing GraphViz."));
        return false;
    }
    QProcess::ExitStatus status = dotVizProcess.exitStatus();
    if(QProcess::NormalExit == status) {
        if(0 != dotVizProcess.exitCode()) {
            QString errorOutput;
            QByteArray errData = dotVizProcess.readAllStandardError();
            if(errData.length() > 500) {
                errData.truncate(500);
            }
            errorOutput = QString(errData);
            Utils::error(_window, QObject::tr("GraphViz returned error: %1\n\nDetails:\n%2").arg(dotVizProcess.exitCode()).arg(errorOutput));
            return false;
        }
    }
    return true;
}

QString XSDPrint::imageFileFromHTMLFile(const QString &fileName)
{
    return fileName + ".png";
}

QString XSDPrint::relativeImageFileFromHTMLFile(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    QString relativeName = imageFileFromHTMLFile(fileInfo.fileName());
    return relativeName ;
}

QString XSDPrint::createDotVizCommands()
{
    QString chart;
    XSDItem *rootItem = _window->root();
    if(NULL == rootItem) {
        return "" ;
    }
    chart += "digraph xsd {\n" ;
    chart += " rankdir=LR;\n";
    chart += " node [shape = box, style=\"rounded, filled\", fillcolor=\"#E0E0E0\"];\n";
    chart += followItem(rootItem, 0);
    chart += "}\n" ;
    /* debug: if(!Utils::writeToFile("/tmp/a/1.dot", chart)) {
        Utils::error(_window, QObject::tr("Error creating chart"));
        return false;
    }*/
    // dot -Tpng dotFile.dot -o <graphname>.png");
    return chart ;
}

QString XSDPrint::indentLine(const int level)
{
    QString result;
    result = result.leftJustified(level, ' ');
    return result ;
}

QString XSDPrint::escapeStringForDot(const QString &input)
{
    QString result = input;
    return result.replace("\"", "\\\"") ;
}

QString XSDPrint::followItem(XSDItem *item, const int level)
{
    const QString indent = indentLine(level);
    QString result ;
    QString address = QString::number((quintptr)item, pointerSize() * 2);
    QString label ;
    QString labelName ;
    labelName = item->itemLabelForChart();
    QColor color = item->itemColorForChart();
    label = QString(" [label=\"%1\" fillcolor=\"%2\" ] ").arg(escapeStringForDot(labelName)).arg(color.name());
    result += QString("%1\"%2\"%3;\n").arg(indent).arg(address).arg(label);
    bool hasChildren = false ;
    if(NULL != item->rChildren()) {
        if(!item->rChildren()->children().isEmpty()) {
            hasChildren = true ;
        }
    }
    if(hasChildren) {
        foreach(RChild * child, item->rChildren()->children()) {
            QString addressChild = QString::number((quintptr)child->item(), pointerSize() * 2);
            result += QString("%1\"%2\"->\"%3\"\n").arg(indent).arg(address).arg(addressChild);
            result += followItem(child->item(), level + 1);
        }
    }
    result += QString("\n");
    return result ;
}

void XSDPrint::enterPrintMethod()
{
    if(NULL != _window) {
        _window->setEnabled(false);
        _window->setUpdatesEnabled(false);
    }
    Utils::showWaitCursor();
}


void XSDPrint::exitPrintMethod()
{
    if(NULL != _window) {
        _window->setEnabled(true);
        _window->setUpdatesEnabled(true);
    }
    Utils::restoreCursor();
}

int XSDPrint::pointerSize()
{
#ifdef  QT_POINTER_SIZE
    return QT_POINTER_SIZE ;
#else
    return sizeof(void*);
#endif
}
