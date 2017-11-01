/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2017 by Luca Bellonda and individual contributors  *
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
 * standard release of QwtPlot3D under the ZLib license (or modified      *
 * versions of such code, with unchanged license).                        *
 * You may copy and distribute such a system following the terms of the   *
 * GNU LGPL for QXmlEdit and the licenses of the other code concerned.    *
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
//#define ENABLE_TIMING_LOG

#include "datawidget.h"
#include "ui_datawidget.h"
#include <stdio.h>
#include <QPainter>
#include <QClipboard>
#include <QToolTip>
#include <QMenu>
#include <QElapsedTimer>
#include "utils.h"
#include "modules/services/uidservices.h"
#include "modules/services/threadservices.h"

#ifdef  QWT_PLOT3D
#include "../external/qwtplot3d/include/qwt3d_global.h"
#include "../external/qwtplot3d/include/qwt3d_axis.h"
#include "../external/qwtplot3d/include/qwt3d_mapping.h"
#include "../external/qwtplot3d/include/qwt3d_gridplot.h"
#endif

#define THRESHOLD_SECONDS   (2)
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
    _mtEnabled = true ;
    _plot = NULL ;
    _bkpt = false ;
    _freeze = false;
    _sizeOfPoints = 0;
    _xPoints = 0 ;
    _yPoints = 0 ;

    _percentY = 0 ;
    _dataMapRows = 0;
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
    _3dGrid = true ;
    _3dPoints = false ;

#ifdef  QWT_PLOT3D
    _plot = new Qwt3D::GridPlot(this);
    _plot->setParent(this);
    setupPlot();
    _plot->setVisible(false);
    _plot->setToolTip(tr("Use arrows with ctrl, shift and alt and mouse wheel to pan, zoom and rotate."));
#endif
}

DataWidget::~DataWidget()
{
    delete ui;
    losePoints();
}

void DataWidget::setMainWindow(QWidget *newMainWindow)
{
    _mainWindow = newMainWindow ;
}

QWidget *DataWidget::mainWindow()
{
    return _mainWindow;
}


#ifdef  QWT_PLOT3D
void DataWidget::setupPlot()
{
    _plot->setZoom(0.8);
    _plot->setRotation(30, 0, 15);

    _plot->setCoordinateStyle(Qwt3D::BOX);

    _plot->setPlotStyle(_3dPoints ? Qwt3D::POINTS : Qwt3D::FILLED);

    Qwt3D::CoordinateSystem* coordinates = _plot->coordinates();

    coordinates->setAutoScale(true);
    for(unsigned i = 0; i != _plot->coordinates()->axes.size(); ++i) {
        coordinates->axes[i].setMajors(25);
        coordinates->axes[i].setMinors(10);
        coordinates->axes[i].setLabelColor(Qwt3D::RGBA(0, 0, 0.4));
    }
    coordinates->setLabelFont("Arial", 8, QFont::Bold);
    coordinates->setLabelColor(Qwt3D::RGBA(0, 0, 0.9));

    QString xLabel = tr("rows (%)") ;
    QString yLabel = tr("columns (%)");
    QString zLabel = tr("normalized value");

    coordinates->axes[Qwt3D::X1].setLabelString(xLabel);
    coordinates->axes[Qwt3D::Y1].setLabelString(yLabel);
    coordinates->axes[Qwt3D::Z1].setLabelString(zLabel);
    coordinates->axes[Qwt3D::X2].setLabelString(xLabel);
    coordinates->axes[Qwt3D::Y2].setLabelString(yLabel);
    coordinates->axes[Qwt3D::Z2].setLabelString(zLabel);
    coordinates->axes[Qwt3D::X3].setLabelString(xLabel);
    coordinates->axes[Qwt3D::Y3].setLabelString(yLabel);
    coordinates->axes[Qwt3D::Z3].setLabelString(zLabel);
    coordinates->axes[Qwt3D::X4].setLabelString(xLabel);
    coordinates->axes[Qwt3D::Y4].setLabelString(yLabel);
    coordinates->axes[Qwt3D::Z4].setLabelString(zLabel);

    coordinates->setGridLinesColor(Qwt3D::RGBA(0.8, 0.8, 0.8));
    coordinates->setNumberFont("Arial", 8);
    coordinates->adjustNumbers(5);
    coordinates->setGridLines(_3dGrid, false, Qwt3D::BACK | Qwt3D::RIGHT);

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
        }

        /*double *dst = newData;
        float *src = _dataPoints ;
        int loop = _sizeOfPoints ;
        while( loop-- ) {
            *dst++ = *src++ ;
        }*/
        // use a form factor to have a cubic image if possible
        // let's say that the height is equal to the rows

        double factor = _yPoints;
        float maxVal = getMaxValue();
        if(maxVal == 0) {
            factor = 1;
        } else {
            // finalpt = max* (rows/max) = rows
            factor = 100 / maxVal ;
        }

        unsigned int columns = _xPoints ;
        unsigned int rows = _yPoints;
        for(unsigned i = 0; i < columns; i++) {
            int colBase = (i * _xPoints);
            for(unsigned j = 0; j < rows; j++) {
                data[i][j] = _dataPoints[colBase + j] * factor;
            }
        }

        if(_plot->createDataset(data, _xPoints, _yPoints, 0, 100, 0, 100, false) < 0) {
            Utils::error(this, tr("Error loading data."));
        }
        setColors3d();

//----

        setupPlot();
//----

        delete [] newData;

        delete [] data;
    }
    _plot->repaint();
}


void DataWidget::setColors3d()
{
    if(NULL != _plot) {
        Qwt3D::RGBA rgb;
        _cv.clear();
        QList<uint> cols;

        rgb.r = 0.95;
        rgb.g = 0.95;
        rgb.b = 0.95;
        _cv.push_back(rgb);

        uint *vt = _colorMap->values();
        FORINT(i, _colorMap->MapElements) {
            uint u = *vt ;
            vt ++;
            rgb.r = (u >> 16) & 0x00FF ;
            rgb.g  = (u >> 8) & 0x00FF ;
            rgb.b = (u) & 0x00FF ;
            rgb.a = 1;
            rgb.r /= 255;
            rgb.g /= 255;
            rgb.b /= 255;
            _cv.push_back(rgb);
        }
        _dataColor.setColorVector(_cv);

        _plot->appearance(0).setDataColor(_dataColor);
    }
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
    _dataMapRows = 0;
    if(NULL != _dataMap) {
        _dataMapRows = _dataMap->rows.count();
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
    if(!_is3d) {
        QPainter painter(this);
        painter.drawStaticText(width() / 2, height() / 2, tr("Recalculating..."));
    }
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
#ifdef  QWT_PLOT3D
    setColors3d();
    if((NULL != _plot) && _plot->isVisible()) {
        _plot->updateData();
        _plot->updateNormals();
        _plot->updateGL();
    }
#endif
    repaint();
}

void DataWidget::generateImage(const bool forceStandard)
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
#ifdef ENABLE_TIMING_LOG
            Utils::TODO_THIS_RELEASE("togli");
            printf("Start painting\n"); fflush(stdout);
            QTime timer;
            timer.start();
            int secs = 0;
#endif
            if(forceStandard) {
                computeImageStandard();
            } else {
                if(_mtEnabled && (Distribution != _measType)) {
                    computeImageThreaded();
                } else {
                    computeImage();
                }
            }
#ifdef ENABLE_TIMING_LOG
            secs = timer.elapsed() / 1000;
            printf("End threads %d secs\n", secs); fflush(stdout);
#endif
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
#ifdef ENABLE_TIMING_LOG
            secs = timer.elapsed() / 1000;
            printf("End paint %d secs\n", secs); fflush(stdout);
#endif
        }
    }
}


bool DataWidget::isMtEnabled() const
{
    return _mtEnabled;
}

void DataWidget::setMtEnabled(bool mtEnabled)
{
    _mtEnabled = mtEnabled;
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
    computeImageSlice(0, _yPoints);
}

void DataWidget::computeImageStandard()
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

QList<int> DataWidget::computeSlices(const int idealThreadCount, const int heightImage)
{
    const int windowsHeight = heightImage / idealThreadCount ;
    const int lastWindowFraction = heightImage - (windowsHeight * idealThreadCount);
    QList<int> items;
    FORINT(i, idealThreadCount) {
        items.append(windowsHeight);
    }
    const int lastItem = items.size() - 1;
    items[lastItem] += lastWindowFraction ;
    return items;
}

int DataWidget::computeWindowsHeight(const int idealThreadCount)
{
    const int heightImage = _yPoints;
    const int windowsHeight = heightImage / idealThreadCount ;
    return windowsHeight;
}

void DataWidget::computeImageThreaded()
{
    // Calculate how many points per px.
    if(_bkpt) {
        _bkpt = false ;
    }

    //const int heightImage = _yPoints;

    int threads = QThread::idealThreadCount() - 1;
    if(threads <= 0) {
        threads = 1 ;
    }
    const int windowsHeight = computeWindowsHeight(threads) ;
    //const int lastWindowFraction = heightImage - (windowsHeight*threads);
    QList<int> items = computeSlices(threads, _yPoints);
    //FORINT(i, threads) {
    //    items.append(windowsHeight);
    //}
    //const int lastItem = items.size()-1;
    //items[lastItem] += lastWindowFraction ;
    int starty = 0 ;
    QList<QFuture<void> > results;

    foreach(int thisWindowHeight, items) {
        int endy = starty + thisWindowHeight ;
        results.append(QtConcurrent::run(this, &DataWidget::computeImageSlice, starty, endy));
        starty += windowsHeight ;
    }
    waitCalcImage(results);
}

void DataWidget::waitCalcImage(QList<QFuture<void> > &threads)
{
    QElapsedTimer timer;
    timer.start();
    bool done = false;
    UIDesktopServices uiServices(mainWindow());
    while(!done) {
        ThreadServices::msleep(250);
        if(timer.elapsed() > (THRESHOLD_SECONDS * 1000)) {
            uiServices.startIconProgressBar();
            uiServices.setIconProgressBar(50);
        }
        done = true ;
        foreach(QFuture<void> future, threads) {
            if(!future.isFinished()) {
                done = false;
                break;
            }
        }
    }
    uiServices.endIconProgressBar();
}

void DataWidget::computeImageSlice(const int paramStartY, const int paramEndY)
{
    // Calculate how many points per px.
    float dataHeight = _dataWindow.height();
    float imgHeight = _yPoints ;
    float ptiPerPxY = dataHeight / imgHeight ;
    float dataWidth = _dataWindow.width();
    float imgWidth = _xPoints ;
    float ptiPerPxX = dataWidth / imgWidth ;

    /*PtrToValue getValueP = getGetValueFunction();
    if(NULL == getValueP) {
        return ;
    }*/
    //qint64 (DataWidget::*getValue1)(ElementBase*) = getGetValueFunction();

    //int heightImage = _yPoints;
    int widthImage = _xPoints ;
    int realDataHeight = _dataMap->rows.size();

    float *mapp = &_dataPoints[widthImage * paramStartY];
    bool *pMask = &_dataPointsMask[widthImage * paramStartY];
    //printf("Data Wnd: (%d,%d)\n", (int)dataWidth, (int)dataHeight);
    int prevParent = -1 ;
    bool docEven = false ;

    int limy = ptiPerPxY / 2 ;
    int limx = ptiPerPxX / 2 ;

#ifdef ENABLE_TIMING_LOG
    void *p = QThread::currentThreadId();
    printf("Thread id:%p (%d,%d)\n", p, (int)paramStartY, (int)paramEndY);
    fflush(stdout);
#endif
    for(int y = paramStartY ; y < paramEndY ; y ++) {
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
                            //value = ((this)->*getValueP)(e);
                            value = getValue(e);
                            for(int yi = 0; yi < ptiPerPxY ; yi++) {
                                VisDataRow *row = getRowAt(yyy - limy + yi);
                                if(NULL != row) {
                                    int startX = qMax(0, xxx - limx);
                                    int endX = qMin((int)(xxx - limx + ptiPerPxX), row->_numColumns);
                                    ElementBase *ex = getElementRow(row, startX);
                                    if(NULL != ex) {
                                        //for(int xi = 0; xi < ptiPerPxX ; xi++) {
                                        for(int xi = startX; xi < endX ; xi++) {
                                            ElementBase *e2 = ex ++ ;
                                            //ElementBase *e2 = getElementRow(row, xxx - limx + xi);
                                            if(NULL != e2) {
                                                values ++ ;
                                                value += getValue(e2);//((this)->*getValueP)(e2);
                                            }
                                        }
                                    }
                                }
                            }
                            value /= values ;
                        } else {
                            value = getValue(e);
                            for(int yi = 0; yi < ptiPerPxY ; yi++) {
                                VisDataRow *row = getRowAt(yyy - limy + yi);
                                if(NULL != row) {
                                    int startX = qMax(0, xxx - limx);
                                    int endX = qMin((int)(xxx - limx + ptiPerPxX), row->_numColumns);
                                    ElementBase *ex = getElementRow(row, startX);
                                    if(NULL != ex) {
                                        //for(int xi = 0; xi < ptiPerPxX ; xi++) {
                                        for(int xi = startX; xi < endX ; xi++) {
                                            ElementBase *e2 = ex ++ ;
                                            //ElementBase *e2 = getElementRow( row, xxx - limx + xi);
                                            if(NULL != e2) {
                                                //float nowValue = ((this)->*getValueP)(e2);
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
                }
            }
            *pMask++ = found ;
            *mapp++ = value ;
        } // forx
    } // fory
#ifdef ENABLE_TIMING_LOG
    printf("End Thread id:%p (%d,%d)\n", p, (int)paramStartY, (int)paramEndY);
    fflush(stdout);
#endif
}

inline quint64 DataWidget::getValue(ElementBase *e)
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
            value = e->totalText;
        } else {
            value = e->text;
        }
        break;
    default:
        Utils::error(tr("DataWidget::getValue() type not valid: %d").arg(_measType));
        break;
    }
    return value;
}

DataWidget::PtrToValue DataWidget::getGetValueFunction()
{
    switch(_measType) {
    case Attributes:
        if(_cumulative) {
            return & DataWidget::getValueAttributesCountCumulative;
        } else {
            return & DataWidget::getValueAttributesCount;
        }
        break;
    case Size:
        if(_cumulative) {
            return & DataWidget::getValueSizeCumulative;
        } else {
            return & DataWidget::getValueSize;
        }
        break;
    case Elements:
        if(_cumulative) {
            return & DataWidget::getValueElementsCumulative;
        } else {
            return & DataWidget::getValueElements;
        }
        break;
    case Payload:
        if(_cumulative) {
            return & DataWidget::getValuePayloadCumulative;
        } else {
            return & DataWidget::getValuePayload;
        }
        break;
    default:
        Utils::error(tr("DataWidget::getValue() type not valid: %d").arg(_measType));
        break;
    }
    return NULL;
}

quint64 DataWidget::getValueAttributesCount(ElementBase *e)
{
    return e->attributesCount;
}

quint64 DataWidget::getValueAttributesCountCumulative(ElementBase *e)
{
    return e->totalAttributesCount;
}

quint64 DataWidget::getValueSize(ElementBase *e)
{
    return e->size;
}

quint64 DataWidget::getValueSizeCumulative(ElementBase *e)
{
    return e->totalSize;
}

quint64 DataWidget::getValueElements(ElementBase *e)
{
    return e->childrenCount;
}

quint64 DataWidget::getValueElementsCumulative(ElementBase *e)
{
    return e->totalChildrenCount;
}

quint64 DataWidget::getValuePayload(ElementBase *e)
{
    return e->text;
}

quint64 DataWidget::getValuePayloadCumulative(ElementBase *e)
{
    return e->totalText;
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
            value = _data->totalText;
        } else {
            value = _dataMap->maxText;
        }
        break;

    }
    return value;
}

bool DataWidget::event(QEvent * event)
{
    if(event->type() == QEvent::ToolTip) {
        if((NULL == _plot) || !_plot->isVisible()) {
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

inline ElementBase *DataWidget::getElement(const int x, const int y)
{
    if((y >= 0) && (y < _dataMapRows)) {
        VisDataRow *row = _dataMap->rows.at(y);
        if((x >= 0) && (row->_numColumns > x)) {
            return row->_columns[x];
        }
    }
    return NULL ;
}

inline ElementBase *DataWidget::getElementRow(VisDataRow *row, const int x)
{
    if(NULL != row) {
        if((x >= 0) && (row->_numColumns > x)) {
            return row->_columns[x];
        }
    }
    return NULL ;
}

inline VisDataRow *DataWidget::getRowAt(const int y)
{
    if((y >= 0) && (y < _dataMapRows)) {
        VisDataRow *row = _dataMap->rows.at(y);
        return row;
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

void DataWidget::on_actionCopyImageToClipboard_triggered()
{
    copyImageToClipboard();
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
            contextMenu->addAction(ui->actionCopyImageToClipboard);
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

void DataWidget::setUseGrid(const bool value)
{
    _3dGrid = value ;
#ifdef  QWT_PLOT3D
    if((NULL != _plot) && _is3d) {
        setupPlot();
    }
#endif
}

void DataWidget::setUsePoints(const bool value)
{
    _3dPoints = value ;
#ifdef  QWT_PLOT3D
    if((NULL != _plot) && _is3d) {
        setupPlot();
    }
#endif
}

void DataWidget::copyImageToClipboard()
{
    QApplication::clipboard()->setImage(_cachedImage);
}
