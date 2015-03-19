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

#include "testvis.h"
#include "app.h"
#include "visualization/vismapdialog.h"

#define DATA_FILE_1 "../test/data/vis/testvis.xml"
#define DATA_FILE_2 "../test/data/vis/testvis2.xml"

TestVis::TestVis()
{
}

bool TestVis::error()
{
    return false;
}


bool TestVis::test()
{
    if( ! testBaseElement() ) {
        return error();
    }

    if( ! testDialog() ) {
        return error();
    }
    return true;
}

/** \brief test list mechanism
  */

bool TestVis::testBaseElement()
{
    ElementBase *root = new ElementBase( NULL, "root") ;
    ElementBase *base = new ElementBase( root, "base") ;
    /*ElementBase *child1 = */new ElementBase( base, "child1") ;
    ElementBase *child2 = new ElementBase( base, "child2") ;
    /*ElementBase *child3 = */new ElementBase( base, "child3") ;
    /*ElementBase *child4 = */new ElementBase( child2, "child4") ;

    // 1 - count
    int total = 0 ;
    count(root, total );
    if( total != 6 ) {
        return error();
    }
    // 2 - ordered scan
    ElementBase *child = root->firstChild->firstChild;
    if( (NULL == child ) || (child->name !=  QString("child1")) ) {
        return error();
    }
    child = child->next;
    if( (NULL == child ) || (child->name !=  QString("child2")) ) {
        return error();
    }
    child = child->next;
    if( (NULL == child ) || (child->name !=  QString("child3")) ) {
        return error();
    }
    child = child->next;
    if( NULL != child ) {
        return error();
    }
    delete root;
    return true;
}

void TestVis::count(ElementBase *e, int &total )
{
    total++;
    ElementBase * child = e->firstChild;
    while( child != NULL ) {
       count(child, total );
       child = child->next;
    }
}

//-------------------------------------------------- Test dialog.

bool TestVis::testDialog()
{
    App app;
    if(!app.init() ) {
        return error();
    }
    VisMapDialog visMapDialog(app.data(), app.mainWindow(), "");
    // ckeck data.
    visMapDialog.loadFile(DATA_FILE_1);

    // check data
    if( visMapDialog._summary.totalSize != 87 ) {
        return error();
    }
    if( visMapDialog._summary.levels != 3 ) {
        return error();
    }
    if( visMapDialog._summary.totalElements != 6 ) {
        return error();
    }
    if( visMapDialog._summary.totalAttributes != 3) {
        return error();
    }
    if( visMapDialog._summary.maxAttributes != 2) {
        return error();
    }
    if( visMapDialog._summary.maxChildren != 3 ) {
        return error();
    }
    if( visMapDialog._summary.maxSize != 54) {
        return error();
    }
    if( visMapDialog._summary.maxPayload != 50) {
        return error();
    }
    if( visMapDialog._summary.totalPayload != 50) {
        return error();
    }

    // change level and check the recalc
    visMapDialog.calcSlice(0);
    if( visMapDialog._summary.totalFragments != 1) {
        return error();
    }
    visMapDialog.calcSlice(1);
    if( visMapDialog._summary.totalFragments != 3) {
        return error();
    }
    visMapDialog.calcSlice(2);
    if( visMapDialog._summary.totalFragments != 2) {
        return error();
    }

    // loads a smaller file and checks if data are changing
    visMapDialog.loadFile(DATA_FILE_2);

    if( visMapDialog._summary.totalSize != 29 ) {
        return error();
    }
    if( visMapDialog._summary.levels != 3 ) {
        return error();
    }
    if( visMapDialog._summary.totalElements != 4) {
        return error();
    }
    if( visMapDialog._summary.totalAttributes != 1) {
        return error();
    }
    if( visMapDialog._summary.maxAttributes != 1) {
        return error();
    }
    if( visMapDialog._summary.maxChildren != 2) {
        return error();
    }
    if( visMapDialog._summary.maxSize != 14 ) {
        return error();
    }
    if( visMapDialog._summary.maxPayload != 10) {
        return error();
    }
    if( visMapDialog._summary.totalPayload != 10) {
        return error();
    }
    return true;
}
