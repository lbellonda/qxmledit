/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#include "StartParams.h"

StartParams::StartParams()
{
    fileName = "" ;
    type = Nothing;
    parametersError = false ;
    forceSaxon = false;
}

StartParams::~StartParams()
{
}

bool StartParams::decodeCommandLine(QStringList args)
{
    if(args.size() > 1) {
        QString arg1 = args.at(1);
        const int size = args.size() ;

        if(arg1 == QString("-vis")) {
            this->type = StartParams::VisFile ;
            if(args.size() > 2) {
                this->fileName = args.at(2);
                if(this->fileName.isEmpty()) {
                    this->parametersError = true ;
                    this->errorMessage = QObject::tr("The input file name is empty.");
                    return false;
                }
            } else {
                this->parametersError = true ;
                this->errorMessage = QObject::tr("Missing parameters for the data visualization option.");
                return false;
            }
        } else if(arg1 == QString("-anonymize")) {
            this->type = StartParams::Anonymize ;
            if(args.size() > 4) {
                this->fileName = args.at(2);
                this->arg1 = args.at(3);
                this->arg2 = args.at(4);
                if(this->fileName.isEmpty()) {
                    this->parametersError = true ;
                    this->errorMessage = QObject::tr("The input file name is empty.");
                    return false;
                }
                if(this->arg1.isEmpty()) {
                    this->parametersError = true ;
                    this->errorMessage = QObject::tr("The profile name is empty.");
                    return false;
                }
                if(this->arg2.isEmpty()) {
                    this->parametersError = true ;
                    this->errorMessage = QObject::tr("The anonymized file name (output) is empty.");
                    return false;
                }
                this->type = StartParams::Anonymize ;
            } else {
                this->parametersError = true ;
                this->errorMessage = QObject::tr("Missing parameters for the anonymize option.");
                return false;
            }
        } else if(arg1 == QString("-xslt")) {
            this->type = StartParams::XSLExec ;
            QString xslFile ;
            QString inputFile ;
            QString outputFile;
            for(int i = 2 ; i < size ; i ++) {
                const QString &parameter = args.at(i);
                if(parameter.indexOf("-p") == 0) {
                    QString cmdLineValue = parameter.mid(2);
                    QStringList pairs = cmdLineValue.split("=");
                    QString name, value;
                    if(pairs.size() > 0) {
                        name = pairs.at(0);
                        if(pairs.size() > 1) {
                            value = pairs.at(1);
                        }
                    }
                    this->params.append(QPair<QString, QString>(name, value));
                } else if(parameter.indexOf("-xsl=") == 0) {
                    xslFile = parameter.mid(5);
                } else if(parameter.indexOf("-output=") == 0) {
                    outputFile = parameter.mid(8);
                } else if(parameter.indexOf("-saxon") == 0) {
                    this->forceSaxon = true ;
                } else {
                    inputFile = parameter ;
                }
            }
            if(xslFile.isEmpty()) {
                this->parametersError = true ;
                this->errorMessage = QObject::tr("XSL file not specified.");
                return false;
            }
            if(inputFile.isEmpty()) {
                this->parametersError = true ;
                this->errorMessage = QObject::tr("Input file not specified.");
                return false;
            }
            if(outputFile.isEmpty()) {
                this->parametersError = true ;
                this->errorMessage = QObject::tr("Output file not specified.");
                return false;
            }
            this->fileName = inputFile ;
            this->xsl = xslFile;
            this->outputFile = outputFile ;
        } else {
            this->fileName = args.at(1);
            if(!this->fileName.isEmpty()) {
                this->type = StartParams::OpenFile ;
            } else {
                this->parametersError = true ;
                this->errorMessage = QObject::tr("The input file name is empty.");
                return false;
            }
        }
    }
    return true;
}

