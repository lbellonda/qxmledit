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


#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

int do_exec(const QString fileName);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if( argc > 1 ) {
        QString fileName = argv[1];
        printf( "Reading input file: %s\n", fileName.toLatin1().data());
        int ret = do_exec(fileName);
        if( ret != 0 ) {
            printf( "****error \n");
        }
        return ret ;
    } else {
        printf( "Need an input file\n");
        return -1;
    }

    //return a.exec();
}


int do_exec(const QString fileName)
{
    QFile file(fileName);
    if( file.open(QFile::ReadOnly) ) {
        QTextStream stream(&file);
        QString line;
        printf("<root>\n");
        do {
            line = stream.readLine();
            line = line.trimmed();
            if( !line.startsWith("#") && (line.length()>0)) {
                QStringList fields = line.split(";");
                QString charValue = fields[0];
                QString name1 = fields[1];
                QString name2 = fields[10];
                QString name ;
                bool ok = false;
                int value = charValue.toInt(&ok, 16);
                if( value < 32 ) {
                    name = name1 + " "+ name2 ;
                } else {
                    name = name1 ;
                }
                printf( "<char code=\"%s\" value=\"%s\"/>\n", charValue.toLatin1().data(), name.toHtmlEscaped().toLatin1().data());
            }

        } while (!line.isNull());
        printf("</root>");
        file.close();
        return 0;
    } else {
        return -1;
    }
}
