/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2021 by Luca Bellonda and individual contributors       *
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

#include "xml2xsdtest.h"

XML2XSDTest::XML2XSDTest(ApplicationData *appData, const QString &fileInput, const bool errorInExecution) : XMLToXSD(appData)
{
    _existsInputFile = false;
    _fileInput = fileInput ;
    _errorInExecution = errorInExecution ;
}


XML2XSDTest::~XML2XSDTest()
{
    //
}

bool XML2XSDTest::execute(const XML2XSDTest::GenXSDOption /*option*/, const int /*enumerationThreshold*/, const bool /*simpleContentTypeSmart*/)
{
    if(_errorInExecution) {
        _result->setError(true);
        return false;
    }

    QFile inputFile(sourceFilePath());
    if(!inputFile.exists()) {
        _existsInputFile = false;
        return false;
    }
    _existsInputFile = true;
    if(_fileInput.isEmpty()) {
        return false;
    }
    if(!QFile::copy(_fileInput, resultPath())) {
        return false;
    }
    return true;
}

bool XML2XSDTest::existsInputFile()
{
    return _existsInputFile ;
}
