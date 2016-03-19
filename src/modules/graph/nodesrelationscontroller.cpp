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

#include <stdio.h>
#include <math.h>
#include "nodesrelationscontroller.h"
#include "tagnodes.h"
#include "tagspring.h"
#include "nodesrelationsdialog.h"
#include "utils.h"
#include "numtablewidgetitem.h"
#include <QLinearGradient>
#include <QDesktopWidget>
#include <QApplication>

#define BACK_COLOR  QColor::fromRgb(0x10,0x80,0xC0)
#define BACK_COLOR_2  QColor::fromRgb(0x0,0x00,0x80)

class Constants
{
public:

    static const double SpringStiffness;
    static const double SpringLength ;
    static const double SpringDamping;
    static const double ForceFactor;
    static const int IterPerTick;
    static const int RadiusOfTheForce;
    static const int StepForCenter;
private:
    Constants() {}
};

const double Constants::SpringStiffness = 0.5 ;
const double Constants::SpringLength = 80 ;
const double Constants::SpringDamping = 0.9 ;
const double Constants::ForceFactor = 16.0 ;
const int Constants::IterPerTick = 30 ;
const int Constants::RadiusOfTheForce = 300 ;
const int Constants::StepForCenter = 10 ;
//-------------------------

NodesRelationsController::NodesRelationsController(NodesRelationsDialog *newDialog)
{
    centerMode = false;
    curOpacity = 1 ;
    lastOpacity = 1 ;
    // side of the window

    {
        QDesktopWidget * desktop = QApplication::desktop();
        QRect rect = desktop->screenGeometry(-1);
        sideX = qMax(rect.width(), 1);
        sideY = qMax(rect.height(), 1);
    }
    totalNodes = 0 ;
    enableSpring = true ;
    dialog = newDialog ;
    forceFactor = Constants::ForceFactor ;
    radiusOfTheForce = Constants::RadiusOfTheForce ;
    iterPerTick = Constants::IterPerTick ;
    isDump = false;
    springStiffness = Constants::SpringStiffness ;
    springLength = Constants::SpringLength;
    springDamping = Constants::SpringDamping;
}

NodesRelationsController::~NodesRelationsController()
{
    reset(true);
}

void NodesRelationsController::deleteCurrentData()
{
    reset(true);
}

void NodesRelationsController::reset(const bool isDeleteData)
{
    centerMode = false;
    curOpacity = 1;
    lastOpacity = 1 ;
    totalNodes = 0 ;
    hidingMarkers.clear();
    if(isDeleteData) {
        nodes.clear();
    }
    foreach(TagSpring * spring, springs) {
        delete spring ;
    }
    springs.clear();
    markers.clear();
    markersByTag.clear();
}

void NodesRelationsController::setBk()
{
    /*QLinearGradient gradient;
    gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
    gradient.setStart(0, 0);
    gradient.setFinalStop(0, 1);

    gradient.setColorAt(0, BACK_COLOR);
    gradient.setColorAt(1, BACK_COLOR_2);
    gradient.setSpread(QGradient::PadSpread);*/

    dialog->scene()->setBackgroundBrush(QBrush(BACK_COLOR));
}

void NodesRelationsController::resetData()
{
    reset(false);
    dialog->scene()->clear();
    dialog->scene()->setSceneRect(0, 0, sideX, sideY);
    setBk();
    calcPct();
    foreach(TagNode * nd, nodes) {
        setupNode(nd);
    }
    setupSprings();
    emit recalc();
}

void NodesRelationsController::startTimer()
{
    emit recalc();
}

void NodesRelationsController::calcPct()
{
    totalNodes = 0 ;
    foreach(TagNode * nd, nodes) {
        totalNodes += nd->count;
    }
    if((totalNodes == 0) && !nodes.isEmpty()) {
        Utils::error(tr("The total of the values is zero."));
    }
}

double NodesRelationsController::getPct(TagNode *nd)
{
    if(totalNodes == 0) {
        return 0 ;
    }
    double pct;
    pct = ((double)nd->count * 100.0) / (double)totalNodes ;
    return pct;
}

void NodesRelationsController::loadDataList(QTableWidget *dataTable)
{
    QMap<QString, TagNode*> names;
    foreach(TagNode * nd, nodes) {
        names.insert(nd->tag, nd);
    }

    dataTable->setUpdatesEnabled(false);
    dataTable->clear();
    dataTable->setColumnCount(nodes.count() + 5) ;
    dataTable->setRowCount(nodes.count());
    QStringList headers;
    headers.append(tr("Node"));
    headers.append(tr("Count"));
    headers.append(tr("Percent"));
    headers.append(tr("Incoming Links"));
    headers.append(tr("Outcoming Links"));

    int row = 0;
    foreach(QString tag, names.keys()) {
        TagNode * nd = names[tag];
        headers.append(nd->tag);

        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(nd->tag);
            dataTable->setItem(row, 0, item);
        }

        {
            NumTableWidgetItem *itemNum = new NumTableWidgetItem();
            itemNum->setText(QString::number(nd->count));
            itemNum->setTextAlignment(Qt::AlignCenter | Qt::AlignRight);
            dataTable->setItem(row, 1, itemNum);
        }

        {
            NumTableWidgetItem *itemNum = new NumTableWidgetItem();
            double pct = getPct(nd);
            itemNum->setText(QString("%1 %").arg(QString::number(pct, 'g', 2), 10));
            itemNum->setTextAlignment(Qt::AlignCenter | Qt::AlignRight);
            dataTable->setItem(row, 2, itemNum);
        }

        {
            NumTableWidgetItem * itemNum = new NumTableWidgetItem();
            itemNum->setText(QString::number(nd->linksIn));
            itemNum->setTextAlignment(Qt::AlignCenter | Qt::AlignRight);
            dataTable->setItem(row, 3, itemNum);
        }

        {
            NumTableWidgetItem *itemNum = new NumTableWidgetItem();
            itemNum->setText(QString::number(nd->linksOut));
            itemNum->setTextAlignment(Qt::AlignCenter | Qt::AlignRight);
            dataTable->setItem(row, 4, itemNum);
        }

        int innerIndex = 1;
        foreach(QString tag2, names.keys()) {
            TagNode * nd2 = names[tag2];
            TagNodeTarget *target = nd->targets.value(nd2->tag);
            if(NULL != target) {
                if(target->count > 0) {
                    NumTableWidgetItem *itemNum = new NumTableWidgetItem();
                    itemNum->setText(QString::number(target->count));
                    itemNum->setTextAlignment(Qt::AlignCenter | Qt::AlignRight);
                    dataTable->setItem(row, 4 + innerIndex, itemNum);
                }
            }
            innerIndex ++ ;
        }
        row++ ;
    }
    dataTable->setHorizontalHeaderLabels(headers);
    dataTable->setUpdatesEnabled(true);
}

void NodesRelationsController::saveDataToStream(QTextStream &outStream)
{
    QMap<QString, TagNode*> names;
    foreach(TagNode * nd, nodes) {
        names.insert(nd->tag, nd);
    }

    QString separator = ",";
    outStream << "\n\n";
    outStream << tr("Elements summary");
    outStream << "\n\n";


    if(0 == names.keys().size()) {
        outStream << tr("There are no data.");
        outStream << "\n\n";
        return ;
    }
    outStream << tr("Row");
    outStream << separator;
    outStream << tr("Node");
    outStream << separator;
    outStream << tr("Count");
    outStream << separator;
    outStream << tr("Percent");
    outStream << separator;
    outStream << tr("Incoming Links");
    outStream << separator;
    outStream << tr("Outcoming Links");
    outStream << separator;
    foreach(QString tag, names.keys()) {
        TagNode * nd = names[tag];
        outStream << nd->tag;
        outStream << separator;
        Utils::TODO_THIS_RELEASE("ultimo separatore?");
    }
    outStream << "\n";

    int row = 0;
    foreach(QString tag, names.keys()) {
        TagNode * nd = names[tag];

        outStream << row;
        outStream << separator;

        outStream << nd->tag;
        outStream << separator;

        outStream << QString::number(nd->count);
        outStream << separator;

        double pct = getPct(nd);
        outStream << QString("%1 %").arg(QString::number(pct, 'g', 2), 10);
        outStream << separator;

        outStream << QString::number(nd->linksIn);
        outStream << separator;

        outStream << QString::number(nd->linksOut);
        outStream << separator;

        int innerIndex = 1;
        foreach(QString tag2, names.keys()) {
            TagNode * nd2 = names[tag2];
            TagNodeTarget *target = nd->targets.value(nd2->tag);
            if(NULL != target) {
                if(target->count > 0) {
                    outStream << QString::number(target->count);
                }
            }
            outStream << separator;
            Utils::TODO_THIS_RELEASE("ultimo separatore?");
            innerIndex ++ ;
        }
        outStream << "\n";
        row++ ;
    }
    outStream << "\n";
    outStream << tr("End of table.");
    outStream << "\n\n";
}


void NodesRelationsController::start(QList<TagNode*> &newNodes)
{
    reset(true);
    dialog->scene()->clear();
    dialog->scene()->setSceneRect(0, 0, sideX, sideY);
    setBk();

    nodes.append(newNodes);
    calcPct();
    foreach(TagNode * nd, nodes) {
        setupNode(nd);
    }
    setupSprings();
}

bool NodesRelationsController::timedTick()
{
    int index = iterPerTick ;
    bool moving = true;
    do {
        handleForces();
        handleSprings();
        moving = updatePosition();
        redraw();
        if(timedHide()) {
            moving = true ;
        }
        index -- ;
    } while(moving && (index > 0));
    return moving ;
}

void NodesRelationsController::redraw()
{
    if(isDump) {
        printf("--------\n");
    }
    int i = 0;
    foreach(TagMarker * nd, markers) {
        // move springs
        // move items
        nd->setPos(nd->position.toPoint());
        if(isDump) {
            printf(" item %d (%d, %d)\n", i, (int)nd->position.x(), (int)nd->position.y());
            fflush(stdout);
            i++;
        }
    }
    foreach(TagSpring * spring, springs) {
        spring->updatePos();
    }
}

void NodesRelationsController::handleForces()
{
    foreach(TagMarker * src, markers) {
        foreach(TagMarker * trg, markers) {
            if(src == trg) {
                continue ;
            }
            if(centerMode && !src->isVisible()) {
                continue ;
            }
            // force is k*/distance
            float dx = trg->position.x() - src->position.x();
            float dy = trg->position.y() - src->position.y();

            float distance = sqrt(dx * dx + dy * dy);
            if(distance == 0) {
                distance = 0.001 ;
            }
            if(distance > radiusOfTheForce) {
                continue ;
            }
            float f = forceFactor / distance;
            QVector2D df(trg->position);
            df -= src->position;
            df.normalize();
            if((df.x() == 0) && (df.y() == 0)) {
                float fx = ((float)qrand() / (float)RAND_MAX) * 10. - 5;
                float fy = ((float)qrand() / (float)RAND_MAX) * 10. - 5;
                df.setX(fx);
                df.setY(fy);
            }
            df *= -f ;
            src->velocity.setX(src->velocity.x() + df.x());
            src->velocity.setY(src->velocity.y() + df.y());
        }
    }
}


bool NodesRelationsController::updatePosition()
{
    bool motion = false;
    int maxX = dialog->scene()->sceneRect().right();
    int maxY = dialog->scene()->sceneRect().bottom();
    foreach(TagMarker * src, markers) {
        if(!centerMode && src->dragged) {
            continue ;
        }
        src->velocity.setX(src->velocity.x() + src->acceleration.x());
        src->velocity.setY(src->velocity.y() + src->acceleration.y());
        float oldX = src->position.x() ;
        float oldY = src->position.y() ;
        float x;
        float y  ;

        if(centerMode && (src->tagNode->tag == tagCentered)) {
            QVector2D centerPoint(maxX / 2, maxY / 2);
            QVector2D pointer = centerPoint - src->position;
            pointer.normalize();
            pointer *= Constants::StepForCenter;
            x = oldX + pointer.x();
            y = oldY + pointer.y();

        } else {

            x = oldX + src->velocity.x() ;
            y = oldY + src->velocity.y() ;
        }

        if(x < 0) {
            x = 5 ;
            src->velocity.setX(-src->velocity.x());
        } else if(x >= maxX) {
            x = maxX - 5 ;
            src->velocity.setX(-src->velocity.x());
        }
        if(y < 0) {
            y = 5 ;
            src->velocity.setY(-src->velocity.y());
        } else if(y > maxY) {
            y = maxY - 5 ;
            src->velocity.setY(-src->velocity.y());
        }
        src->position.setX(x);
        src->position.setY(y);
        src->acceleration *= 0;
        if(!motion) {
            float dx = qAbs(x - oldX) ;
            float dy = qAbs(y - oldY) ;
            if((dx >= 1) || (dy >= 1)) {
                motion = true ;
            }
        }

        src->velocity *= 0.4;

    }
    return motion;
}

void NodesRelationsController::handleOneSpring(TagSpring *spring)
{
    if(centerMode && !spring->isVisible()) {
        return;
    }

    TagMarker *to = spring->to;
    TagMarker *from = spring->from;

    QVector2D diff = from->position - to->position ;
    diff.normalize();
    diff *= spring->length ;

    QVector2D target = from->position - diff ;
    QVector2D force = target - to->position ;

    force *= spring->k / 2.0; // stiffing and damping (half)

    to->acceleration += force;
    from->acceleration -= force;
}


void NodesRelationsController::setupNode(TagNode *nd)
{
    TagMarker *marker = new TagMarker(nd, this);
    markers.append(marker);
    //-------------------------------------------------------------
    int x = (sideX - 1) * ((float)qrand() / (float)RAND_MAX) ;
    int y = (sideY - 1) * ((float)qrand() / (float)RAND_MAX) ;
    marker ->position.setX(x);
    marker ->position.setY(y);
    marker ->originalPosition = marker->position;
    marker ->velocity.setX(0);
    marker ->velocity.setY(0);
    marker ->acceleration.setX(0);
    marker ->acceleration.setY(0);
    //-------------------------------------------------------------

    markersByTag[marker->tagNode->tag] = marker ;
    marker ->setPos(marker->originalPosition.toPoint());
    int markerRadius = 4 + (int)((getPct(nd) * 32) / 100.0);
    marker->radius = markerRadius ;
    int halfRadius = markerRadius / 2 ;
    marker ->setRect(-halfRadius, -halfRadius, markerRadius, markerRadius);
    /*if( i == 1 || i == 3 ) {
        marker ->setBrush(QBrush(QColor::fromRgb(0, 255, 0)));
    } else {
        if( i > 0 && i < 7 ) {
            marker ->setBrush(QBrush(QColor::fromRgb(255, 0, 0)));
        } else {
            marker ->setBrush(QBrush(QColor::fromRgb(0, 0, 0)));
        }
    }*/
    dialog->scene()->addItem(marker);
}

void NodesRelationsController::setupSprings()
{
    if(!enableSpring) {
        return ;
    }
    QSet<QString> springsSet;
    foreach(TagMarker * marker, markers) {

        foreach(TagNodeTarget * tnt, marker->tagNode->targets.values()) {
            if(NULL != tnt) {
                TagMarker *targetMarker = markersByTag[tnt->tag];
                QString keyToSearch = tnt->tag + "<>" + marker->tagNode->tag ;
                if(!springsSet.contains(keyToSearch)) {
                    QString keyToInsert = marker->tagNode->tag + "<>" + tnt->tag;
                    springsSet.insert(keyToInsert);
                    TagSpring *spring = new TagSpring(marker, targetMarker, springStiffness, springLength, springDamping);
                    springs.append(spring);
                }
            }
        }
    }
    //-------------------------------------------------------------
}

void NodesRelationsController::recalcSprings()
{
    foreach(TagSpring * spring, springs) {
        spring->setValues(springStiffness, springLength, springDamping);
    }
}

void NodesRelationsController::handleSprings()
{
    foreach(TagSpring * spring, springs) {
        handleOneSpring(spring);
    }
}


double NodesRelationsController::force()
{
    return forceFactor ;
}

void NodesRelationsController::setForce(const double value)
{
    forceFactor = value ;
    //printf(" force %g", value);
    //fflush(stdout);
    emit recalc();
}

double NodesRelationsController::radius()
{
    return radiusOfTheForce ;
}

void NodesRelationsController::setRadius(const double value)
{
    radiusOfTheForce = value ;
    //printf(" RADIUS %g", value);
    //fflush(stdout);

    emit recalc();
}

double NodesRelationsController::stiffness()
{
    return springStiffness;
}

void NodesRelationsController::setStiffness(const double value)
{
    springStiffness = value ;
    recalcSprings();
    emit recalc();
}

double NodesRelationsController::damping()
{
    return springDamping;
}

void NodesRelationsController::setDamping(const double value)
{
    springDamping = value ;
    recalcSprings();
    emit recalc();
}

double NodesRelationsController::springsLength()
{
    return springLength;
}

void NodesRelationsController::setSpringsLength(const double value)
{
    springLength = value ;
    recalcSprings();
    emit recalc();
}

int NodesRelationsController::markersCount()
{
    return markers.count();
}

int NodesRelationsController::springsCount()
{
    return springs.count();
}

void NodesRelationsController::setViewOnlySelectedMode(const bool newMode)
{
    centerMode = newMode ;
    tagCentered = "" ;
    if(!centerMode) {
        resetData();
    }
}

void NodesRelationsController::onMarkerClicked(TagMarker *marker)
{
    if(centerMode) {
        tagCentered = marker->tagNode->tag;
        hidingMarkers.clear();
        curOpacity = 1;
        lastOpacity = 1;
        hideOtherMarkers(marker);
        hideOtherSprings();

        emit recalc();
    }
}

void NodesRelationsController::hideOtherMarkers(TagMarker *selMarker)
{
    foreach(TagMarker * marker, markers) {
        bool isVisible = false;
        if(marker->tagNode->tag == tagCentered) {
            isVisible = true ;
        } else {
            foreach(TagNodeTarget * tnt, marker->tagNode->targets.values()) {
                if(NULL != tnt) {
                    if(tnt->tag == tagCentered) {
                        isVisible = true ;
                        break;
                    }
                }
            }
        }
        if(!isVisible) {
            if(selMarker->tagNode->targets.keys().contains(marker->tagNode->tag)) {
                isVisible = true ;
            }
        }
        if(!isVisible) {
            hidingMarkers.append(marker);
        } else {
            marker->setVisible(true);
            marker->setOpacity(1);
        }
    }
}

void NodesRelationsController::hideOtherSprings()
{
    foreach(TagSpring * spring, springs) {
        bool isVisible = false;
        if((spring->from->tagNode->tag == tagCentered) || (spring->to->tagNode->tag == tagCentered)) {
            isVisible = true ;
        }
        spring->setVisible(isVisible);
    }
}

bool NodesRelationsController::timedHide()
{
    if(hidingMarkers.isEmpty()) {
        return false;
    }
    if(curOpacity < 0.1) {
        foreach(TagMarker * marker, hidingMarkers) {
            marker->setVisible(false);
            marker->setOpacity(1);
        }
        hidingMarkers.clear();
        return false;
    } else {
        curOpacity -= 0.005;
        if(lastOpacity - curOpacity >= 0.1) {
            lastOpacity = curOpacity ;
            foreach(TagMarker * marker, hidingMarkers) {
                marker->setOpacity(curOpacity);
            }
        }
        return true;
    }
}
