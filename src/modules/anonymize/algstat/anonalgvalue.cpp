/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2022 by Luca Bellonda and individual contributors       *
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

#include "anonalgvalue.h"

#ifdef  QXML_DEBUG
#define DD(x)    do{x;}while(0)
#else
#define DD(x)
#endif

AnonStatAlgValue::AnonStatAlgValue() : _splitRegExp("\\s+")
{
    //
}


AnonStatAlgValue::~AnonStatAlgValue()
{
    //
}

AnonStatAlgWordChain *AnonStatAlgValue::chainForWords(const int size)
{
    void *target = childForSize(size);
    return (AnonStatAlgWordChain *) target;
}

AnonAlgFreqTarget* AnonStatAlgValue::newChild()
{
    return new AnonStatAlgWordChain();
}

bool AnonStatAlgValue::scan(AnonAlgStatContext &context, const QString &input)
{
    DD(qDebug() << QString("  scan input=%1").arg(input));
    const QStringList tokens = input.split(_splitRegExp, QString::SkipEmptyParts);
    AnonStatAlgWordChain* chain = chainForWords(tokens.size());
    if(NULL == chain) {
        return context.setError(QObject::tr("Unable to scan a value"), QObject::tr("New chain failed for %1").arg(tokens.size()));
    }
    newHit(chain);
    chain->scan(context, tokens);
    return !context.isError() ;
}

QString AnonStatAlgValue::value(AnonAlgStatContext &context, AnonProducer *producer)
{
    DD(qDebug() << QString("  value"));
    AnonStatAlgWordChain* chain = (AnonStatAlgWordChain*)randomChoice();
    if(NULL == chain) {
        context.setError(QObject::tr("Unable to find chain value"), QObject::tr("State is: %1").arg(dumpAsString()));
        return "xxxx";
    }
    const QString result = chain->value(context, producer);
    if(producer->isError()) {
        context.setError(QObject::tr("Producer error"), "");
    }
    return result ;
}

void AnonStatAlgValue::dump(const QString &message)
{
    qDebug() << QString("dump of %1").arg(message);
    qDebug() << dumpAsString();
    qDebug() << "   end";
}

QString AnonStatAlgValue::dumpAsString()
{
    QString result ;
    result += QString(" Value scan: size: %1\n").arg(children().size());
    foreach(int key, children().keys()) {
        result += QString(" SCAN: Key: %1: %2\n").arg(key).arg(children().value(key)->count());
        AnonStatAlgWordChain *chain = chainForWords(key);
        result += chain->dumpAsString();
    }
    result += "end\n";
    return result;
}
