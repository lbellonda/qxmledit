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

#include "anonalgchain.h"
#include "modules/anonymize/xmlanonutils.h"

#ifdef  QXML_DEBUG
#define DD(x)    do{x;}while(0)
#else
#define DD(x)
#endif

AnonStatAlgWordChain::AnonStatAlgWordChain()
{
    _type = AnonProducer::ASCII;
}

AnonStatAlgWordChain::~AnonStatAlgWordChain()
{
    EMPTYPTRLIST(_words, AnonStatAlgWord);
}

int AnonStatAlgWordChain::wordsCount()
{
    return _words.size();
}

QList<AnonStatAlgWord*> AnonStatAlgWordChain::words()
{
    return _words;
}

AnonStatAlgWord* AnonStatAlgWordChain::wordForIndex(const int index)
{
    if(index < _words.size()) {
        return _words[index];
    } else if(_words.size() == index) {
        AnonStatAlgWord *word = new AnonStatAlgWord();
        _words.append(word);
        return word;
    } else {
        return NULL;
    }
}

void AnonStatAlgWordChain::scan(AnonAlgStatContext &context, const QStringList &tokens)
{
    int index = 0;
    foreach(const QString &token, tokens) {
        AnonStatAlgWord *word = wordForIndex(index);
        if(NULL == word) {
            context.setError(QObject::tr("Unable to scan a word"), QObject::tr("wordForIndex for %1").arg(index));
            return ;
        }
        word->scan(context, token);
        setTypeIfNotSet(word->type());
        index++;
    }
}

QString AnonStatAlgWordChain::value(AnonAlgStatContext &context, AnonProducer *producer)
{
    DD(qDebug() << QString(" WORD CHAIN BEFORE GET '%1'").arg(dumpAsString()));
    QString result ;
    foreach(AnonStatAlgWord* word, _words) {
        const QString wordValue = word->value(context, producer, _type);
        if(!wordValue.isEmpty()) {
            if(!result.isEmpty()) {
                result += " ";
            }
            result += wordValue;
        }
        DD(qDebug() << QString(" WORD VALUE '%1'").arg(wordValue));
    }
    DD(qDebug() << QString(" WORD VALUE TOTAL '%1'").arg(result));
    return result ;
}

bool AnonStatAlgWordChain::setTypeIfNotSet(const AnonProducer::ESeqType aType)
{
    return XmlAnonUtils::setTypeIfNotSet(_type, aType);
}

AnonProducer::ESeqType AnonStatAlgWordChain::type() const
{
    return _type ;
}

QString AnonStatAlgWordChain::dumpAsString()
{
    QString result ;
    result += QString("CHAIN WORDS: Count :%1\n").arg(_words.size());
    foreach(AnonStatAlgWord *word, _words) {
        result += QString("  word: %1\n").arg(word->dumpAsString());
    }
    result += "--end\n";
    return result ;
}
