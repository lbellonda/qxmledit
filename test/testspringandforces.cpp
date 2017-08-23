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


#include "testspringandforces.h"
#include "modules/graph/nodesrelationscontroller.h"
#include "modules/graph/nodesrelationsdialog.h"
#include "modules/graph/tagnodes.h"
#include "modules/graph/tagmarker.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define FILE_TEST_1 "../test/data/relations.xml"

TestSpringAndForces::TestSpringAndForces()
{
}

TestSpringAndForces::~TestSpringAndForces()
{
}

class NRC : public NodesRelationsController
{
public:
    NRC(NodesRelationsDialog *newDialog);
    void setup();
    TagMarker *addNode( const QString &tag, int posx, int posy );
    bool iter();
};

NRC::NRC(NodesRelationsDialog *newDialog): NodesRelationsController(newDialog)
{

}

void NRC::setup()
{
    enableSpring = false;
}

TagMarker *NRC::addNode( const QString &tag, int posx, int posy )
{
    TagNode *nd = new TagNode(tag, nodes.count()+1);
    nodes.append(nd);
    TagMarker *mrk = new TagMarker(nd, this);
    mrk->position.setX(posx);
    mrk->position.setY(posy);
    markers.append(mrk);
    return mrk ;
}

bool NRC::iter()
{
    int index = 20 ;
    bool moving = true;
    do {
        handleForces();
        handleSprings();
        moving = updatePosition();
        redraw();
        index -- ;
    } while(moving && (index > 0));
    return moving ;
}
//----------------------------

bool TestSpringAndForces::likeAngle( const double angle, const double expected )
{
    double delta = qAbs(angle-expected);
    if( delta > 3 ) {
        return false;
    }
    return true ;
}

//----------------------------

bool TestSpringAndForces::testUnitForces1()
{
    _testName = "testUnitForces1";
    QList<TagNode*> dataList;
    NodesRelationsDialog newDialog(false, dataList, NULL);
    NRC ctl(&newDialog);
    ctl.setup();

    TagMarker *tag0 = ctl.addNode( "a", 100, 50 );
    TagMarker *tag1 = ctl.addNode( "b", 100, 100 );

    ctl.iter();

    if( tag0->position.x() != tag1->position.x() ) {
        return error("testUnitForces x");
    }
    if( tag0->position.y() >= 50 ) {
        return error("testUnitForces y 1");
    }
    if( tag1->position.y() <= 100 ) {
        return error("testUnitForces y 2");
    }
    return true ;
}

bool TestSpringAndForces::checkTestUnitForces2(TagMarker *tag, const double expected, const int number, int x0, int y0)
{
    double dx = tag->position.x() - x0 ;
    double dy = tag->position.y() - y0 ;
    double angle = atan2(dy, dx ) * 180/M_PI ;
    if( angle < 0 ) {
        angle += 360;
    } else if( angle > 360 ) {
        angle = (int)angle % 360;
    }
    if( !likeAngle(angle, expected ) ) {
        return error( QString("case %2, expected - found %1 ").arg(angle).arg(number));
    }
    return true;
}

bool TestSpringAndForces::testUnitForces2()
{
    _testName = "testUnitForces2";
    QList<TagNode*> dataList;
    NodesRelationsDialog newDialog(false, dataList, NULL);
    NRC ctl(&newDialog);
    ctl.setup();

    int radius = 50 ;
    int x0 = 400;
    int y0 = 400;
    int x1 = x0+radius*cos(M_PI*90.0/180.0);
    int y1 = y0+radius*sin(M_PI*90.0/180.0);
    TagMarker *tag0 = ctl.addNode( "a", x1, y1 );
    int x2 = x0+radius*cos(M_PI*210.0/180.0);
    int y2 = y0+radius*sin(M_PI*210.0/180.0);
    TagMarker *tag1 = ctl.addNode( "b", x2, y2 );
    int x3 = x0+radius*cos(M_PI*330.0/180.0);
    int y3 = y0+radius*sin(M_PI*330.0/180.0);
    TagMarker *tag2 = ctl.addNode( "a", x3, y3 );

    ctl.iter();

    if( !checkTestUnitForces2(tag0, 90, 0, x0, y0) ) {
        return false;
    }
    if( !checkTestUnitForces2(tag1, 210, 1, x0, y0) ) {
        return false;
    }
    if( !checkTestUnitForces2(tag2, 330, 2, x0, y0) ) {
        return false;
    }
    return true ;
}

/*
<root>
 <item>
  <object>
   <item/>
  </object>
 </item>
</root>
*/

bool TestSpringAndForces::testLoadSampleData()
{
    _testName = "testLoadSampleData";
    QList<TagNode*> dataList;
    NodesRelationsDialog newDialog(true, dataList, NULL);
    if( !newDialog.loadFile(FILE_TEST_1) ) {
        return error("loading file");
    }

    if(newDialog.getController()->markersCount() != 3 ) {
        return error("node number");
    }

    if(newDialog.getController()->springsCount() != 2 ) {
        return error("spring number");
    }

    QTableWidget *tabWidget = newDialog.getTableWidget();

    int numRows = tabWidget->rowCount();
    if( numRows != 3 ) {
        return error("row number");
    }

    int numColumns = tabWidget->columnCount();
    if( numColumns != 8 ) {
        return error("column number");
    }

    for( int row = 0 ; row < numRows ; row ++ ) {
        if( rowIs(tabWidget, row, "root") ) {
            if(! checkRow(tabWidget, row, "1", "25 %", "0", "1",    "1", "", "" ) ) {
                return error("row root");
            }
        } else if( rowIs(tabWidget, row, "item") ) {
            if(! checkRow(tabWidget, row, "2", "50 %", "2", "1",    "", "1", "" ) ) {
                return error("row item");
            }
        } else if( rowIs(tabWidget, row, "object") ) {
            if(! checkRow(tabWidget, row, "1", "25 %", "1", "1",   "1", "", "" ) ) {
                return error("row object");
            }
        } else {
            return error("row not registered");
        }
    }
    return true ;
}

//-------------------------

bool TestSpringAndForces::rowIs(QTableWidget *table, const int row, const QString &name)
{
    QTableWidgetItem *item = table->item(row, 0);
    if( NULL == item ) {
        return true ;
    }
    QString itemText = item->text() ;
    if( name == itemText ) {
        return true ;
    }
    return false ;
}

bool TestSpringAndForces::checkRow(QTableWidget *table, const int row, const QString &countCol, const QString &percentCol, const QString &incomingLinksCol,
                                   const QString &outcomingLinksCol, const QString &itemCol, const QString &objectCol, const QString &rootCol )
{
    if( !checkItem(table, row, 1, countCol ) ) {
        return false;
    }
    if( !checkItem(table, row, 2, percentCol ) ) {
        return false;
    }
    if( !checkItem(table, row, 3, incomingLinksCol ) ) {
        return false;
    }
    if( !checkItem(table, row, 4, outcomingLinksCol ) ) {
        return false;
    }
    if( !checkItem(table, row, 5, itemCol ) ) {
        return false;
    }
    if( !checkItem(table, row, 6, objectCol ) ) {
        return false;
    }
    if( !checkItem(table, row, 7, rootCol ) ) {
        return false;
    }
    return true ;
}

bool TestSpringAndForces::checkItem(QTableWidget *table, const int row, const int column, const QString &text)
{
    QTableWidgetItem *item = table->item(row, column);
    if( NULL == item ) {
        if( text.isEmpty() ) {
            return true ;
        }
        return false;
    }
    QString itemText = item->text().trimmed() ;
    if( text != itemText ) {
        return false;
    }
    return true ;
}

//-----------------------

bool TestSpringAndForces::testUnitForces()
{
    if( !testUnitForces1()) {
        return false;
    }
    if( !testUnitForces2()) {
        return false;
    }
    if( !testLoadSampleData()) {
        return false;
    }

    return true;
}
