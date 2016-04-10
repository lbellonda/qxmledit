/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QPointF>
#include <QTextStream>

#include "elementbase.h"
#include "visdatamap.h"
#include "colormap.h"

#ifdef  QWT_PLOT3D
#include <math.h>
#include "../external/qwtplot3d/include/qwt3d_gridplot.h"
#include "../external/qwtplot3d/include/qwt3d_function.h"
#include "../external/qwtplot3d/include/qwt3d_plot3d.h"
#endif

namespace Ui
{
class DataWidget;
}

class DataWidget : public QWidget
{
    Q_OBJECT
public:
    enum EMeasurementType {
        Distribution,
        Attributes,
        Size,
        Elements,
        Payload
    };

    enum ELoudness {
        NoLoudness,
        LoudnessMax,
        LoudnessDilate
    };

private:

    EMeasurementType _measType;
    QImage _cachedImage;

    bool _freeze ;

    QPoint _contextMenuPos;
    VisDataMap *_dataMap;
    ElementBase *_data;
    bool _newData ;
    QRect _dataWindow;
    bool _zoomInZone;
    QPointF _dataOffset;
    int _zoom;
    bool _cumulative;
    bool _isLogScale;
    float *_dataPoints;
    bool *_dataPointsMask;
    ColorMap *_colorMap;
    bool _debugMode;
    /** \brief if the data are to be divided using level n of the tree
      */
    int _sliceLevel;
    int _percentY ;
    bool _bkpt;
    uint _bkColor;
    uint _emptyValueColor;
    ELoudness _loudness;
#ifdef  QWT_PLOT3D
    Qwt3D::GridPlot *_plot;
    Qwt3D::StandardColor _dataColor;
    Qwt3D::ColorVector _cv;
#else
    QWidget *_plot;
#endif
    bool _is3d;
    bool _3dGrid;
    bool _3dPoints;

    unsigned int _sizeOfPoints, _xPoints, _yPoints;

    void debugImage();
    void recalc();

public:
    explicit DataWidget(QWidget *parent = 0);
    ~DataWidget();


    void paintEvent(QPaintEvent *e);
    void setData(ElementBase *data);
    void setColorMap(ColorMap *newMap);
    void dumpData();
    //int sliceOfElement(const int y);
    void assignRealColors(float *mapp, float maxVal, uint *cmap, bool *pMask, const int x, const int y);
    void setZoom(const int newZoom);
    int levels();
    void setSlice(const int newSliceLevel);
    void setDataMap(VisDataMap *newDataMap);
    void freeze();
    void unfreeze();
    void setWindowPerc(const int percent);
    void setVisType(const DataWidget::EMeasurementType type);
    void setCumulative(const bool isCumulative);
    void writeDetails(QTextStream &outStream);
#ifdef  QWT_PLOT3D
    void set3D(const bool is3D);
#endif
    void setLoudness(const ELoudness newLoudness);
    ELoudness loudness();
    void setUseGrid(const bool use);
    void setUsePoints(const bool use);

protected:
    virtual bool event(QEvent * event);
    virtual void contextMenuEvent(QContextMenuEvent * event);
    virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::DataWidget *ui;

    void generateImage();
    void computeImage();
    ElementBase *getElement(const int x, const int y);
    void drawImage();
    void filterImage();
    void losePoints();
    inline quint64 getValue(ElementBase *e);
    quint64 getMaxValue();
#ifdef  QWT_PLOT3D
    void setupPlot();
    void draw3d();
    void setColors3d();
    bool openColorMap(Qwt3D::ColorVector& cv, QString fname);
#endif

private slots:
    void on_actionCopyDataToClipboard_triggered();
    void on_actionExtractThisFragment_triggered();
    void on_actionCopyPathToClipboard_triggered();

signals:
    void extractFragment(const int fragment, const int depth);
};

#endif // DATAWIDGET_H
