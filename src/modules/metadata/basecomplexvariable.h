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


#ifndef BASECOMPLEXVARIABLE_H
#define BASECOMPLEXVARIABLE_H

#include <QString>

class PseudoAttribute
{
private:
    /**
     * @brief _exists the variable exists in the source form
     */
    bool _exists;
    /**
     * @brief _used: the variable is used in the output form
     */
    bool _used;
    /**
     * @brief _name: the name of the variable
     */
    QString _name;
    /**
     * @brief _row of the element
     */
    int _row;
    /**
     * @brief _position: ordinal position of the variable in the definition line
     */
    int _position;
    bool _isSingleOrDoubleQuote;
    QString _value;
public:
    PseudoAttribute();
    ~PseudoAttribute();
    //-
    bool exists() const;
    void setExists(bool exists);
    bool used() const;
    void setUsed(bool used);
    QString name() const;
    void setName(const QString &name);
    QString value() const;
    void setValue(const QString &value);
    int position() const;
    void setPosition(int position);
    int row() const;
    void setRow(int row);

    void copyTo(PseudoAttribute *target);
    QString toString();
    QString toStringStd();
    QString toStringComplete();
    void reset(const QString &newValue);
    void reset();

    void read(PseudoAttribute*src);

};

#endif // BASECOMPLEXVARIABLE_H
