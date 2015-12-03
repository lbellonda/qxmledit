/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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

#include "testcolor.h"
#include "app.h"
#include "qxmleditconfig.h"
#include "testhelpers/testconfigcolors.h"


TestColor::TestColor()
{
}

TestColor::~TestColor()
{
}


bool TestColor::testExport()
{
    _testName = "testExport" ;
    App app;
    app.data()->colorManager()->colorFromKey(Config::KEY_COLOR_TAGS)->setColor(QColor::fromRgb(10,0,10));
    app.data()->colorManager()->colorFromKey(Config::KEY_COLOR_DECL_PROCINSTR)->setColor(QColor::fromRgb(0,0,1));
    app.data()->colorManager()->colorFromKey(Config::KEY_COLOR_ATTRNAMES)->setColor(QColor::fromRgb(0,0,2));
    app.data()->colorManager()->colorFromKey(Config::KEY_COLOR_ATTRVALUES)->setColor(QColor::fromRgb(0,0,3));
    app.data()->colorManager()->colorFromKey(Config::KEY_COLOR_COMMENTS)->setColor(QColor::fromRgb(0,0,4));
    app.data()->colorManager()->colorFromKey(Config::KEY_COLOR_INFO)->setColor(QColor::fromRgb(0,0,5));
    app.data()->colorManager()->colorFromKey(Config::KEY_COLOR_TEXTDECL)->setColor(QColor::fromRgb(0,0,6));

    app.data()->colorManager()->colorFromKey(Config::KEY_COLOR_ANONELPREVIEW)->setColor(QColor::fromRgb(0,0,7));
    app.data()->colorManager()->colorFromKey(Config::KEY_COLOR_ANONTEXTPREVIEW)->setColor(QColor::fromRgb(0,0,8));
    app.data()->colorManager()->colorFromKey(Config::KEY_COLOR_ANONATTRPREVIEW)->setColor(QColor::fromRgb(0,0,9));

    QBuffer buffer;
    TestConfigColors colors;
    colors.init(app.data());
    buffer.open(QIODevice::WriteOnly);
    colors.exportTo(&buffer);

    buffer.close();
    buffer.open(QIODevice::ReadOnly);
    QTextStream stream(&buffer);
    QString readed = stream.readAll();
    QString expected = buildData();
    if( expected != readed ) {
        return error(QString("Found '%1', expected '%2'").arg(readed).arg(expected));
    }
    return true;
}


bool TestColor::testImport()
{
    _testName = "testImport" ;
    App app;

    QBuffer buffer;
    QString cfg = buildData();

    QByteArray bytes = cfg.toUtf8();
    buffer.setData(bytes);

    TestConfigColors colors;
    colors.init(app.data());
    buffer.open(QIODevice::ReadOnly);
    colors.importFrom(&buffer);

    buffer.close();
    if(! compareData(&app) ) {
        return false ;
    }
    return true ;
}


bool TestColor::compareData(App *app)
{
    if(!compareColor( app, Config::KEY_COLOR_TAGS, QColor::fromRgb(10,0,10)) ){
        return false;
    }
    if(!compareColor( app, Config::KEY_COLOR_DECL_PROCINSTR, QColor::fromRgb(0,0,1)) ){
        return false;
    }
    if(!compareColor( app, Config::KEY_COLOR_ATTRNAMES, QColor::fromRgb(0,0,2)) ){
        return false;
    }
    if(!compareColor( app, Config::KEY_COLOR_ATTRVALUES, QColor::fromRgb(0,0,3)) ){
        return false;
    }
    if(!compareColor( app, Config::KEY_COLOR_COMMENTS, QColor::fromRgb(0,0,4)) ){
        return false;
    }
    if(!compareColor( app, Config::KEY_COLOR_INFO, QColor::fromRgb(0,0,5)) ){
        return false;
    }
    if(!compareColor( app, Config::KEY_COLOR_TEXTDECL, QColor::fromRgb(0,0,6)) ){
        return false;
    }
    if(!compareColor( app, Config::KEY_COLOR_ANONELPREVIEW, QColor::fromRgb(0,0,7)) ){
        return false;
    }
    if(!compareColor( app, Config::KEY_COLOR_ANONTEXTPREVIEW, QColor::fromRgb(0,0,8)) ){
        return false;
    }
    if(!compareColor( app, Config::KEY_COLOR_ANONATTRPREVIEW, QColor::fromRgb(0,0,9)) ){
        return false;
    }

    return true;
}


bool TestColor::compareColor(App *app, const QString &key, const QColor &clr)
{
    if(app->data()->colorManager()->colorFromKey(key)->color() != clr) {
        return error(QString("Color failed %1").arg(key));
    }
    return true;
}

QString TestColor::buildData()
{
    QString result;
    result += "\"colors/tags\",\"0A000A\"\n";
    result += "\"colors/declProcInstruction\",\"000001\"\n";
    result += "\"colors/attrnames\",\"000002\"\n";
    result += "\"colors/attrvalues\",\"000003\"\n";
    result += "\"colors/comments\",\"000004\"\n";
    result += "\"colors/info\",\"000005\"\n";
    result += "\"colors/textDecl\",\"000006\"\n";

    result += "\"colors/anonElPreview\",\"000007\"\n";
    result += "\"colors/anonTextPreview\",\"000008\"\n";
    result += "\"colors/anonAttrPreview\",\"000009\"\n";

    return result;
}



