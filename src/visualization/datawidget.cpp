/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2016 by Luca Bellonda and individual contributors  *
 *    as indicated in the AUTHORS file                                    *
 *  lbellonda _at_ gmail.com                                              *
 *                                                                        *
 * This library is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU Library General Public            *
 * License as published by the Free Software Foundation; either           *
 * version 2 of the License, or (at your option) any later version.       *
 *                                                                        *
 * As a special exception, the copyright holders of QXmlEdit give you     *
 * permission to combine QXmlEdit with free software programs or libraries*
 * that are released under the GNU LGPL and with code included in the     *
 * standard release of QWT3D under the ZLib license (or modified versions *
 * of such code, with unchanged license). You may copy and distribute such*
 * a system following the terms of the GNU LGPL for QXmlEdit and the      *
 * licenses of the other code concerned.                                  *
 * Note that people who make modified versions of QXmlEdit are not        *
 * obligated to grant this special exception for their modified versions; *
 * it is their choice whether to do so. The GNU Library General Public    *
 * License gives permission to release a modified version without this    *
 * exception; this exception also makes it possible to release a modified *
 * version which carries forward this exception.                          *
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

//#define LOG_CONSOLE 1
#include "xmlEdit.h"

#include "datawidget.h"
#include "ui_datawidget.h"
#include <stdio.h>
#include <QPainter>
#include <QClipboard>
#include <QToolTip>
#include <QMenu>
#include "utils.h"

#ifdef  QWT_PLOT3D
#include "../external/qwtplot3d/include/qwt3d_global.h"
#include "../external/qwtplot3d/include/qwt3d_axis.h"
#include "../external/qwtplot3d/include/qwt3d_mapping.h"
#include "../external/qwtplot3d/include/qwt3d_gridplot.h"
#endif


#define MESH_NUMBER (100)

//---------- colors

#define RED_COLOR   qRgb(0xFF, 0x00, 0x00)
#define BLUE_COLOR  qRgb(0x40, 0x40, 0xFF)
#define PALEYELLOW_COLOR    qRgb(0xFF, 0xE0, 0x80)
#define DEFAULT_BACKGROUND_COLOR  qRgb(0xFF, 0xFF, 0xFF)
#define DEFAULT_GRAY_COLOR qRgb(0xD0, 0xD0, 0xD0)


DataWidget::DataWidget(QWidget *parent) :
    QWidget(parent),
    _contextMenuPos(0, 0),
    _dataOffset(0, 0),
    ui(new Ui::DataWidget)
{
#ifdef  QWT_PLOT3D
    _plot = NULL ;
#endif
    _bkpt = false ;
    _freeze = false;
    _sizeOfPoints = 0;
    _xPoints = 0 ;
    _yPoints = 0 ;

    _percentY = 0 ;
    _dataMap = NULL;
    _measType = Distribution;
    _debugMode = false ;
    _sliceLevel = 1 ;
    _zoom = 1 ;
    _cumulative = false;
    _colorMap = NULL ;
    _newData = false;
    _dataPoints = NULL;
    _dataPointsMask = NULL ;
    _bkColor = DEFAULT_BACKGROUND_COLOR ;
    _emptyValueColor = DEFAULT_GRAY_COLOR;
    _loudness = NoLoudness;
    _is3d = false ;

    ui->setupUi(this);
#ifdef  QWT_PLOT3D
    _plot = new Qwt3D::GridPlot(this);
    _plot->setParent(this);
    setupPlot();
    _plot->setVisible(false);
#endif
}

DataWidget::~DataWidget()
{
    delete ui;
    losePoints();
}

#ifdef  QWT_PLOT3D
void DataWidget::setupPlot()
{
    _plot->setZoom(0.8);
    _plot->setRotation(30, 0, 15);

    //_plot->setPlotStyle( Qwt3D::FILLEDMESH );
    _plot->setPlotStyle(Qwt3D::FILLED);
    _plot->setCoordinateStyle(Qwt3D::BOX);

    Qwt3D::CoordinateSystem* coordinates = _plot->coordinates();

    coordinates->axes[Qwt3D::X1].setLabelString(tr("rows (%)"));
    coordinates->axes[Qwt3D::Y1].setLabelString(tr("columns (%)"));
    coordinates->axes[Qwt3D::Z1].setLabelString(tr("value normal."));
    coordinates->axes[Qwt3D::Z1].setAutoScale(true);

    coordinates->setGridLinesColor(Qwt3D::RGBA(0, 1, 1));
    coordinates->setNumberFont("Arial", 8);
    //coordinates->adjustNumbers(5);
    //coordinates->axes[Qwt3D::Z1].setScale(Qwt3D::LOG10SCALE);

    _plot->move(0, 0);
    _plot->resize(width(), height());
    _plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _plot->updateData();
    _plot->updateGL();
}


void DataWidget::draw3d()
{
    double** data = new double*[_xPoints];
    double *newData = new double[_sizeOfPoints];
    if((NULL != data) && (NULL != newData)) {

        for(unsigned int x = 0 ; x < _xPoints ; x ++) {
            data[x] = &newData[x * _yPoints];
            //data[x] = new double[_yPoints];
        }

        /*double *dst = newData;
        float *src = _dataPoints ;
        int loop = _sizeOfPoints ;
        while( loop-- ) {
            *dst++ = *src++ ;
        }*/
        double factor = _yPoints;
        float maxVal = getMaxValue();
        if(maxVal == 0) {
            factor = 1;
        } else {
            factor /= maxVal ;
        }

        unsigned int columns = _xPoints ;
        unsigned int rows = _yPoints;
        for(unsigned i = 0; i < columns; i++) {
            for(unsigned j = 0; j < rows; j++) {
                data[i][j] = _dataPoints[(i * _xPoints) + j] * factor;
            }
        }

        //if(!_plot->loadFromData(data, _xPoints, _yPoints, 0, _dataMap->numColumns, 0, _dataMap->rows.size())) {

        /*if(!_plot->loadFromData(data, _xPoints, _yPoints, 0, 100, 0, 100)) {
            Utils::error(this, tr("Error loading data."));
        }*/

        if(_plot->createDataset(data, _xPoints, _yPoints, 0, 100, 0, 100, false) < 0) {
            Utils::error(this, tr("Error loading data."));
        }

        //_plot->coordinates()->axes[Qwt3D::Z1].setMajors(maxVal*factor/5.);
        //_plot->coordinates()->axes[Qwt3D::Z1].setMinors(maxVal*factor/10.);

        delete [] newData;

        /*for(int x = 0 ; x < _xPoints ; x ++) {
            delete [] data[x] ;
        }*/

        delete [] data;
    }
    _plot->coordinates()->axes[Qwt3D::Z1].setScale(Qwt3D::LOG10SCALE);
    _plot->repaint();

    /*    double *newData = new double[_sizeOfPoints];
        if( NULL != newData ) {
            double *dst = newData;
            float *src = _dataPoints ;
            int loop = _sizeOfPoints ;
            while( loop-- ) {
                *dst++ = *src++ ;
            }
            if( !_plot->loadFromData (&newData, _xPoints, _yPoints, 0, _xPoints, 0, _yPoints) ) {
                Utils::error(this, tr("Error loading data."));
            }
            delete [] newData;
        }*/
}

#endif


void DataWidget::losePoints()
{
    if(NULL != _dataPoints) {
        delete [] _dataPoints ;
        _dataPoints = NULL ;
    }
    if(NULL != _dataPointsMask) {
        delete [] _dataPointsMask ;
        _dataPointsMask = NULL ;
    }
}

void DataWidget::setData(ElementBase *data)
{
    _data = data;
    recalc();
}

int DataWidget::levels()
{
    return _dataMap->numColumns;
}

void DataWidget::recalc()
{
    if(_freeze) {
        return ;
    }

    if(NULL != _dataMap) {
        // percent of slide,
        int x = (_dataOffset.x() * _dataMap->numColumns) / 100.0;
        int y = (_percentY * _dataMap->rows.size()) / 100.0;
        int width = _dataMap->numColumns ; //(_dataMap.numColumns / _zoom );
        int height = _dataMap->rows.size() / _zoom ;
        //--------------------
        if(y < 0) {
            y = 0 ;
        }
        //--------------------
        if((y + height) > _dataMap->rows.size()) {
            y = _dataMap->rows.size() - height ;
        }
        //--------------------

        _dataWindow.setRect(x, y, width, height);

        //printf(" perc is %d y is %d/%d, h is %d\n", _percentY,  y, _dataMap->rows.size(), height);
        //fflush(stdout);

        _newData = true ;
#ifdef  QWT_PLOT3D
        if(_is3d) {
            generateImage();
        } else {
            update();
        }
#else
        update();
#endif
    }
}
void DataWidget::paintEvent(QPaintEvent * /*e*/)
{
    generateImage();
    if(!_is3d) {
        QPainter painter(this);
        painter.drawImage(0, 0, _cachedImage);
    }
}

void DataWidget::setColorMap(ColorMap *newMap)
{
    _colorMap = newMap;
    _bkColor = newMap->backgroundColor();
    _emptyValueColor = newMap->emptyColor();

    _newData = true ;
    repaint();
}

void DataWidget::generateImage()
{
    if(NULL == _dataMap) {
        return ;
    }
    if(_newData
            || _cachedImage.isNull()
            || (_cachedImage.size() != size())
      ) {
        if(_debugMode) {
            debugImage();
            return ;
        }
        _newData = false;
        losePoints();
        if(_dataMap->rows.count()) {
            _cachedImage = QImage(size(), QImage::Format_RGB32);

            int w = _is3d ? MESH_NUMBER : size().width();
            int h = _is3d ? MESH_NUMBER : size().height();
            int theSize = w * h ;
            _dataPoints = new float[theSize];
            _dataPointsMask = new bool[theSize];

            _sizeOfPoints = theSize;
            _xPoints = w ;
            _yPoints = h ;

            if((NULL == _dataPointsMask) || (NULL == _dataPoints)) {
                Utils::errorOutOfMem(this);
                return ;
            }
            computeImage();
            filterImage();
#ifdef  QWT_PLOT3D
            if(_is3d) {
                draw3d();
            } else {
                drawImage();
            }
#else
            drawImage();
#endif
            //dumpData();
        }
    }
}


void DataWidget::debugImage()
{
    // Calculate how many points per px.
    for(int y = 0 ; y < height() ; y ++) {
        for(int x = 0 ; x < width(); x ++) {
            ElementBase *e = getElement(x, y);
            uint color ;
            if(NULL  != e) {
                color = qRgb(0, 0, 0);
            } else {
                color = qRgb(255, 255, 255);
            }
            _cachedImage.setPixel(x, y, color);
        } // forx
    } // fory
}

/*
per il deep: per ogni riga a video trovo
a quale riga appartiene e setto i colori.

 quando esamino i dati, devo solo scoprire la riga di partenza e a quale riga appartengono
*/

void DataWidget::computeImage()
{
    // Calculate how many points per px.
    if(_bkpt) {
        _bkpt = false ;
    }
    float dataHeight = _dataWindow.height();
    float imgHeight = _yPoints ;
    float ptiPerPxY = dataHeight / imgHeight ;
    float dataWidth = _dataWindow.width();
    float imgWidth = _xPoints ;
    float ptiPerPxX = dataWidth / imgWidth ;

    int heightImage = _yPoints;
    int widthImage = _xPoints ;
    int realDataHeight = _dataMap->rows.size();

    //QVector<float> sliceValues;
    //sliceValues.reserve(imgHeight);

    float *mapp = _dataPoints;
    bool *pMask = _dataPointsMask;
    //printf("Data Wnd: (%d,%d)\n", (int)dataWidth, (int)dataHeight);
    int prevParent = -1 ;
    bool docEven = false ;

    int limy = ptiPerPxY / 2 ;
    int limx = ptiPerPxX / 2 ;

    for(int y = 0 ; y < heightImage ; y ++) {
        //printf("\nriga %d\n", y);
        float sliceValueX = 0 ;
        for(int x = 0 ; x < widthImage ; x ++) {
            // punto centrale
            int yyy = _dataWindow.top() + (y * dataHeight) / imgHeight;
            int xxx = _dataWindow.left() + (x * dataWidth) / imgWidth;
            //printf(" %d=(%d,%d)", x, xxx, yyy);
            float value = 0;
            bool found = false;
            if(yyy < realDataHeight) {
                VisDataRow *row = _dataMap->rows.at(yyy);
                if(row->_numColumns > xxx) {
                    ElementBase *e = row->_columns[xxx];
                    int values = 1;
                    found = true;

                    if(_measType == Distribution) {
                        if(xxx < _sliceLevel) {
                            value = 0 ;
                        } else {
                            bool isSlice = false;
                            if(xxx == _sliceLevel) {
                                isSlice = true ;
                            }
                            // use 0 as a threshold
                            // TODO: cache this
                            if(isSlice) {
                                int parentId = e->id;
                                if(parentId != prevParent) {
                                    docEven = !docEven ;
                                }
                                prevParent = parentId ;
                                if(docEven) {
                                    value = 1 ;
                                } else {
                                    value = -1 ;
                                }
                                sliceValueX = value;
                            } else {
                                value = sliceValueX ;//sliceValues.at(y);
                            }
                        }
                    } else {
                        if(_loudness == NoLoudness) {
                            value = getValue(e);
                            for(int yi = 0; yi < ptiPerPxY ; yi++) {
                                for(int xi = 0; xi < ptiPerPxX ; xi++) {
                                    ElementBase *e2 = getElement(xxx - limx + xi, yyy - limy + yi);
                                    if(NULL != e2) {
                                        values ++ ;
                                        value += getValue(e2);
                                    }
                                }
                            }
                            value /= values ;
                        } else {
                            value = getValue(e);
                            for(int yi = 0; yi < ptiPerPxY ; yi++) {
                                for(int xi = 0; xi < ptiPerPxX ; xi++) {
                                    ElementBase *e2 = getElement(xxx - limx + xi, yyy - limy + yi);
                                    if(NULL != e2) {
                                        float nowValue = getValue(e2);
                                        if(nowValue > value) {
                                            value = nowValue ;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            *pMask++ = found ;
            *mapp++ = value ;
        } // forx
    } // fory
}

quint64 DataWidget::getValue(ElementBase *e)
{
    qint64 value = 0 ;
    switch(_measType) {
    case Attributes:
        if(_cumulative) {
            value = e->totalAttributesCount;
        } else {
            value = e->attributesCount;
        }
        break;
    case Size:
        if(_cumulative) {
            value = e->totalSize;
        } else {
            value = e->size;
        }
        break;
    case Elements:
        if(_cumulative) {
            value = e->totalChildrenCount;
        } else {
            value = e->childrenCount;
        }
        break;
    case Payload:
        if(_cumulative) {
            value = e->totalPayload;
        } else {
            value = e->payload;
        }
        break;
    default:
        Utils::error(tr("DataWidget::getValue() type not valid: %d").arg(_measType));
        break;
    }
    return value;
}

quint64 DataWidget::getMaxValue()
{
    qint64 value = 0 ;
    switch(_measType) {
    case Distribution:
        return _data->totalSize;
        break;
    case Attributes:
        if(_cumulative) {
            value = _data->totalAttributesCount;
        } else {
            value = _dataMap->maxAttributesCount;
        }
        break;
    case Size:
        if(_cumulative) {
            value = _data->totalSize;
        } else {
            value = _dataMap->maxSize;
        }
        break;
    case Elements:
        if(_cumulative) {
            value = _data->totalChildrenCount;
        } else {
            value = _dataMap->maxChildrenCount;
        }
        break;
    case Payload:
        if(_cumulative) {
            value = _data->totalPayload;
        } else {
            value = _dataMap->maxPayload;
        }
        break;

    }
    return value;
}

bool DataWidget::event(QEvent * event)
{
    if(event->type() == QEvent::ToolTip) {
        QHelpEvent *toolTipEvent = static_cast<QHelpEvent *>(event);
        QPoint pos = toolTipEvent->pos();
        if((_dataWindow.height() > 0)  && (_dataWindow.width() > 0)) {
            int y = _dataWindow.top() + (pos.y() * _dataWindow.height()) / height();
            int x = _dataWindow.left() + (pos.x() * _dataWindow.width()) / width();
            ElementBase *e = getElement(x, y);
            ElementBase *selection = e;
            if(NULL != e) {
                // builds the chain
                QString path = e->name ;
                while(e->parent != NULL) {
                    path = e->parent->name + "/" + path ;
                    e = e->parent ;
                }
                path += tr("\nrow=%1, column=%2").arg(y).arg(x);
                path += "\n";
                path += tr("size: %1, attributes %2, children %3\n").arg(selection->size).arg(selection->attributesCount).arg(selection->childrenCount);
                path += tr("cumulative size: %1, attributes %2, children %3").arg(selection->totalSize).arg(selection->totalAttributesCount).arg(selection->totalChildrenCount);
                QToolTip::showText(toolTipEvent->globalPos(), path);
                return true;
            }
        }
    }
    return QWidget::event(event);
}

/*
int DataWidget::sliceOfElement(const int y)
{
    ElementBase *e = getElement(_sliceLevel, y);
    if(NULL != e ) {
        return e->id;
    }
    return -1;
}
*/

ElementBase *DataWidget::getElement(const int x, const int y)
{
    if((y >= 0) && (y < _dataMap->rows.count())) {
        VisDataRow *row = _dataMap->rows.at(y);
        if((x >= 0) && (row->_numColumns > x)) {
            return row->_columns[x];
        }
    }
    return NULL ;
}

void DataWidget::drawImage()
{
    //QString s;
    float *mapp = _dataPoints ;
    float maxVal = getMaxValue();
    uint *cmap = _colorMap->values();
    bool *pMask = _dataPointsMask;
    int h = height() ;
    int w = width();
    for(int y = 0 ; y < h ; y ++) {
        //s += QString("y:%1 max:%2\n").arg(y).arg(maxVal);
        for(int x = 0 ; x < w; x ++) {
            //s += QString("%1 ").arg((int)*mapp);
            //if( *mapp>=maxVal ) {
            //    s += QString("***");
            //}
            assignRealColors(mapp, maxVal, cmap, pMask, x, y);
            pMask++;
            mapp++;
        }
        //s += "\n";
    }
    //QClipboard *clipboard = QApplication::clipboard();
    //clipboard->setText(s);
}


static inline void findMax(const int x, const int y, float &maxValue, bool *mask, float *data, const int width)
{
    if(mask[width * y + x]) {
        float value = data[width * y + x];
        if(value > maxValue) {
            maxValue = value ;
        }
    }
}

void DataWidget::filterImage()
{
    if(LoudnessDilate == _loudness) {
        int h = _yPoints;
        int w = _xPoints;
        int h1 = h - 1 ;
        int w1 = w - 1;

        int theSize = _sizeOfPoints;
        float *newData = new float[theSize];
        if(NULL != newData) {

            float *src = _dataPoints;
            float *dst = newData;
            int loopCount = theSize ;
            while(loopCount--) {
                *dst++ = *src++ ;
            }

            for(int y = 1 ; y < h1 ; y ++) {
                for(int x = 1 ; x < w1; x ++) {
                    float maxVal = 0 ;
                    findMax(x - 1, y - 1, maxVal, _dataPointsMask, newData, w);
                    findMax(x, y - 1, maxVal, _dataPointsMask, newData, w);
                    findMax(x + 1, y - 1, maxVal, _dataPointsMask, newData, w);
                    //-
                    findMax(x - 1, y, maxVal, _dataPointsMask, newData, w);
                    findMax(x, y, maxVal, _dataPointsMask, newData, w);
                    findMax(x + 1, y, maxVal, _dataPointsMask, newData, w);
                    //-
                    findMax(x - 1, y + 1, maxVal, _dataPointsMask, newData, w);
                    findMax(x, y + 1, maxVal, _dataPointsMask, newData, w);
                    findMax(x + 1, y + 1, maxVal, _dataPointsMask, newData, w);
                    //-
                    //*(_dataPoints[width*y+x]) = maxVal ;
                    _dataPoints[w * y + x] = maxVal ;

                }
            }
            delete [] newData;
        }
    }
}

void DataWidget::assignRealColors(float *mapp, float maxVal, uint *cmap, bool *pMask, const int x, const int y)
{
    switch(_measType) {
    case Distribution: {
        if(*pMask) {
            float value = *mapp;
            if(value > 0) {
                _cachedImage.setPixel(x, y, RED_COLOR);
            } else if(value < 0) {
                _cachedImage.setPixel(x, y, BLUE_COLOR);
            } else {
                _cachedImage.setPixel(x, y, PALEYELLOW_COLOR);
            }
        } else {
            _cachedImage.setPixel(x, y, DEFAULT_BACKGROUND_COLOR);
        }
    }
    break;
    default: {
        if(*pMask) {
            int value = (ColorMap::MapElements * ((*mapp)  / maxVal));
            if(0 == value) {
                _cachedImage.setPixel(x, y, _emptyValueColor);
            } else {
                if(value < 0) {
                    value = 0 ;
                } else if(value >= ColorMap::MapElements) {
                    value = ColorMap::MapElements - 1;
                }
                _cachedImage.setPixel(x, y, cmap[value]);
            }
            //s += QString("[ x:%1 ] %2 %3").arg(x).arg(*mapp).arg(value);
        } else {
            _cachedImage.setPixel(x, y, _bkColor);
        }
    }
    break;
    }
}

void DataWidget::dumpData()
{
    QString s = QString("Data Map:\n");
    int yy = _dataMap->rows.size();
    int xx = _dataMap->numColumns;
    for(int y = 0 ; y < yy ; y ++) {
        s += QString("y:%1 ").arg(y);
        for(int x = 0 ; x < xx ; x ++) {
            if(NULL != getElement(x, y)) {
                s += QString("x:%1 ON ").arg(x);
            } else {
                s += QString("x:%1 - ").arg(x);
            }
        }
        s += "\n";
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(s);
}

void DataWidget::setZoom(const int newZoom)
{
    _zoom = newZoom;
    recalc();
}

void DataWidget::setSlice(const int newSliceLevel)
{
    if((newSliceLevel >= 0) && (newSliceLevel < levels())) {
        _sliceLevel = newSliceLevel;
        recalc();
    }
}

void DataWidget::setDataMap(VisDataMap *newDataMap)
{
    _dataMap = newDataMap;
    recalc();
}

void DataWidget::setWindowPerc(const int percent)
{
    _percentY = percent;
    //TRACEQ(QString("Data perc is %1").arg(percent));
    recalc();
}

void DataWidget::freeze()
{
    _freeze = true ;
}

void DataWidget::unfreeze()
{
    _freeze = false ;
    recalc();
}

void DataWidget::setVisType(const DataWidget::EMeasurementType type)
{
    switch(type) {
    default:
        break;
    case Distribution:
    case Attributes:
    case Size:
    case Elements:
    case Payload:
        _measType = type ;
        _bkpt = true ;
        recalc();
        break;
    }
}

void DataWidget::setCumulative(const bool isCumulative)
{
    _cumulative = isCumulative ;
    recalc();
}

#ifdef  QWT_PLOT3D
void DataWidget::set3D(const bool value)
{
    _is3d = value ;
    _plot->setVisible(value);
    recalc();
}
#endif

void DataWidget::writeDetails(QTextStream &outStream)
{
    if(NULL == _dataPoints) {
        outStream << tr("No data available\n");
        return ;
    }
    float *mapp = _dataPoints ;
    float maxVal = getMaxValue();
    bool *pMask = _dataPointsMask;
    int h = height() ;
    int w = width();
    outStream << tr("Measurement type: %1 %2\n").arg(_measType).arg(_cumulative ? tr("cumulative") : tr("non cumulative"));
    outStream << tr("Max value : %1 \n").arg(maxVal);
    outStream << tr("Data Window (%1,%2) -%3 x %4\n").arg(_dataWindow.left()).arg(_dataWindow.top()).arg(_dataWindow.width()).arg(_dataWindow.height());
    for(int y = 0 ; y < h ; y ++) {
        outStream << QString(tr("row :%1 \n")).arg(y).arg(maxVal);
        for(int x = 0 ; x < w; x ++) {
            outStream << QString("%1 ").arg((int)*mapp);
            pMask++;
            mapp++;
        }
        outStream << "\n";
    }
}

void DataWidget::on_actionCopyDataToClipboard_triggered()
{
    QString s;
    if(NULL == _dataPoints) {
        s = tr("No data available\n");
    } else {
        float *mapp = _dataPoints ;
        float maxVal = getMaxValue();
        bool *pMask = _dataPointsMask;
        int h = height() ;
        int w = width();
        s.append(tr("Measurement type: %1 %2\n").arg(_measType).arg(_cumulative ? tr("cumulative") : tr("non cumulative")));
        s.append(tr("Max value : %1 \n").arg(maxVal));
        s.append(tr("Data Window (%1, %2) -%3 x %4\n").arg(_dataWindow.left()).arg(_dataWindow.top()).arg(_dataWindow.width()).arg(_dataWindow.height()));
        for(int y = 0 ; y < h ; y ++) {
            s.append(QString(tr("row :%1 \n")).arg(y).arg(maxVal));
            for(int x = 0 ; x < w; x ++) {
                s.append(QString("%1 ").arg((int)*mapp));
                pMask++;
                mapp++;
            }
            s.append("\n");
        }
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(s);
    Utils::message(tr("Data copied to the clipboard."));
}

void DataWidget::on_actionCopyPathToClipboard_triggered()
{
    if((_dataWindow.height() > 0)  && (_dataWindow.width() > 0)) {
        int y = _dataWindow.top() + (_contextMenuPos.y() * _dataWindow.height()) / height();
        int x = _dataWindow.left() + (_contextMenuPos.x() * _dataWindow.width()) / width();
        ElementBase *e = getElement(x, y);
        if(NULL != e) {
            // builds the chain
            QString path = e->name ;
            while(e->parent != NULL) {
                path = e->parent->name + "/" + path ;
                e = e->parent ;
            }
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(path);
        }
    }
}

void DataWidget::on_actionExtractThisFragment_triggered()
{
    // find the element at the coordinates
    if((_dataWindow.height() > 0)  && (_dataWindow.width() > 0)) {
        int y = _dataWindow.top() + (_contextMenuPos.y() * _dataWindow.height()) / height();
        int x = _dataWindow.left() + (_contextMenuPos.x() * _dataWindow.width()) / width();
        ElementBase *e = getElement(x, y);
        if(NULL != e) {
            // Find the # of the document and emits a signal.
            QSet<int> slices;
            int rowMax = _dataMap->rows.size();
            for(int y = 0 ; y < rowMax ; y ++) {
                ElementBase *el = getElement(x, y);
                if(NULL != el) {
                    if(e->id != el->id) {
                        slices.insert(el->id);
                    } else {
                        emit extractFragment(slices.size() + 1, x);
                        return ;
                    }
                }
            }
        }
    }
}

void DataWidget::contextMenuEvent(QContextMenuEvent * event)
{
    _contextMenuPos = event->pos();
    QMenu *contextMenu = new QMenu(NULL);
    bool isMenu = false;
    if(NULL != contextMenu) {
        if(NULL != _dataPoints) {
            contextMenu->addAction(ui->actionCopyDataToClipboard);
            isMenu = true ;
        }
        if((_dataWindow.height() > 0)  && (_dataWindow.width() > 0)) {
            int y = _dataWindow.top() + (_contextMenuPos.y() * _dataWindow.height()) / height();
            int x = _dataWindow.left() + (_contextMenuPos.x() * _dataWindow.width()) / width();
            ElementBase *e = getElement(x, y);
            if(NULL != e) {
                contextMenu->addAction(ui->actionExtractThisFragment);
                contextMenu->addAction(ui->actionCopyPathToClipboard);
                isMenu = true ;
            }
        }
        if(isMenu) {
            contextMenu->exec(event->globalPos());
        }
    }
}


void DataWidget::setLoudness(const ELoudness newLoudness)
{
    _loudness = newLoudness ;
    recalc();
}

DataWidget::ELoudness DataWidget::loudness()
{
    return _loudness ;
}


void DataWidget::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
#ifdef  QWT_PLOT3D
    if(NULL != _plot) {
        _plot->resize(width(), height());
    }
#endif
}
