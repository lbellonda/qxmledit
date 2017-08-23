/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#ifndef NODESRELATIONSCONTROLLER_H
#define NODESRELATIONSCONTROLLER_H

#include <QList>
#include <QHash>
#include <QTableWidget>
#include <QTextBrowser>

class NodesRelationsDialog;
class TagNode;
class TagSpring;
class TagMarker;
class AttributesSummaryData;
class AttributeSummaryData;
class AttributesSummaryTotal;

class NodesRelationsController : public QObject
{
    Q_OBJECT
protected:
    bool enableSpring;
    int iterPerTick;
    bool isDump;
    int forceValue ;
    int radiusOfTheForce ;
    qint64 totalNodes;
    float forceFactor;
    NodesRelationsDialog *dialog;
    QList<TagNode*> nodes;
    QList<TagSpring*> springs;
    QList<TagMarker*> markers;
    QHash<QString, TagMarker*> markersByTag;
    QList<TagMarker*> hidingMarkers;

    double springStiffness;
    double springLength;
    double springDamping;

    int sideX;
    int sideY;
    bool centerMode;
    QString tagCentered;
    double curOpacity;
    double lastOpacity;

    void handleForces();
    void handleSprings();
    void redraw();
    bool updatePosition();
    void handleOneSpring(TagSpring *spring);
    void setupNode(TagNode *nd);
    void setupSprings();
    void reset(const bool isDeleteData);
    void recalcSprings();
    void calcPct();
    double getPct(TagNode *nd);
    void setBk();
    void hideOtherMarkers(TagMarker *selMarker);
    void hideOtherSprings();
    bool timedHide();
    void calculateSingleAttributeLine(QString &text, AttributeSummaryData *attributeSummaryData, const long fileSize, const bool isHTML);
    void tableAttributeData(QString & text, QMap<QString, QString> attrsKeys, AttributesSummaryData* attributesSummaryData, const QString &msg, const long sizeFile);
    QString formatSingleValue(const QString &inputText, const bool isNumber, const bool isHTML);
    void printAttributesTable(QString & text, QMap<QString, QString> attrsKeys,
                              AttributesSummaryData* attributesSummaryData,
                              AttributesSummaryTotal &total,
                              const QString &msg, const QString &msgTotal,
                              const long sizeFile);
public:
    NodesRelationsController(NodesRelationsDialog *newDialog);
    ~NodesRelationsController();

    void deleteCurrentData();

    void resetData();
    void startTimer();
    void start(QList<TagNode*> &newNodes);
    bool timedTick();
    void loadDataList(QTableWidget *dataTable);

    double force();
    void setForce(const double value);
    double radius();
    void setRadius(const double value);
    double stiffness();
    void setStiffness(const double value);
    double damping();
    void setDamping(const double value);
    double springsLength();
    void setSpringsLength(const double value);

    int markersCount();
    int springsCount();

    void setViewOnlySelectedMode(const bool newMode);

    void onMarkerClicked(TagMarker *marker);
    void saveDataToStream(QTextStream &outStream);
    void loadAttributesData(QTextBrowser *textBrowser, AttributesSummaryData* attributesSummaryData);
    void exportAttributesInCSVToStream(QTextStream &stream, AttributesSummaryData* attributesSummaryData);

signals:
    void recalc();
};

#endif // NODESRELATIONSCONTROLLER_H
