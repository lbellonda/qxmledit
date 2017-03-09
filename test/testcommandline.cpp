/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "testcommandline.h"
#include "StartParams.h"

TestCommandLine::TestCommandLine()
{
}

TestCommandLine::~TestCommandLine()
{
}

bool TestCommandLine::testFast()
{
    if(!testUnit()){
        return false;
    }
    return true ;
}

extern bool decodeCommandLine(QStringList args, StartParams *params);


bool TestCommandLine::checkArgs(const QString &id, QStringList args, const bool expectedResult, const int expectedTypeInt )
{
    _testName = id;
    StartParams params;
    const bool res = params.decodeCommandLine(args);
    if( res != expectedResult ) {
        return error(QString("Command line result, expected %1, found %2").arg(expectedResult).arg(res));
    }
    if(res == params.parametersError) {
        return error(QString("Command line result, parameters error, expected %1, found %2").arg(!expectedResult).arg(params.parametersError));
    }
    const StartParams::ESPType expectedType = static_cast<StartParams::ESPType>(expectedTypeInt);
    if(params.type != expectedType ) {
        return error(QString("Command line type, expected %1, found %2").arg(expectedType).arg(params.type));
    }
    return true ;
}

bool TestCommandLine::testUnit()
{
    _testName = "testUnit";

    //no args: start or server
    if(!checkArgs("no args", QStringList(), true, StartParams::Nothing )) {
        return false;
    }
    {
        QStringList args;
        args << "pgm" ;
        if(!checkArgs("one arg", args, true, StartParams::Nothing )) {
            return false;
        }
    }

    // one argument: file
    {
        QStringList args;
        args << "pgm" << "file";
        if(!checkArgs("file", args, true, StartParams::OpenFile )) {
            return false;
        }
    }

    // arg vis senza altro: error, then exit
    {
        QStringList args;
        args << "pgm" << "-vis";
        if(!checkArgs("vis", args, false, StartParams::VisFile )) {
            return false;
        }
    }

    // arg vis: ok, open dialog
    {
        QStringList args;
        args << "pgm" << "-vis" << "file";
        if(!checkArgs("vis 1", args, true, StartParams::VisFile )) {
            return false;
        }
    }

    // anon: no arg error
    {
        QStringList args;
        args << "pgm" << "-anonymize" ;
        if(!checkArgs("anon 0", args, false, StartParams::Anonymize )) {
            return false;
        }
    }

    // arg xslt: nok, the full test is done in xslt
    {
        QStringList args;
        args << "pgm" << "-xslt" ;
        if(!checkArgs("xslt 0", args, false, StartParams::XSLExec )) {
            return false;
        }
    }


    // anon: 1 arg error
    {
        QStringList args;
        args << "pgm" << "-anonymize" << "file";
        if(!checkArgs("anon 1", args, false, StartParams::Anonymize )) {
            return false;
        }
    }

    // anon: 2 arg engage: wrong profile
    {
        QStringList args;
        args << "pgm" << "-anonymize" << "file" << "profile" ;
        if(!checkArgs("anon 2", args, false, StartParams::Anonymize )) {
            return false;
        }
    }

    // anon: 3 arg engage: non ex file in
    {
        QStringList args;
        args << "pgm" << "-anonymize" << "file" << "profile" << "file2";
        if(!checkArgs("anon 3", args, true, StartParams::Anonymize )) {
            return false;
        }
    }

    return true ;
}
