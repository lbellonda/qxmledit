/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#ifndef METADATAINFODIALOG_H
#define METADATAINFODIALOG_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>

#include "modules/metadata/metadatainfo.h"


class MetadataEditRow
{
public:
    QCheckBox *check;
    QLineEdit *edit;
    PseudoAttribute *attribute;
    MetadataEditRow();
    ~MetadataEditRow();

    void setup(QCheckBox *theCheck, QLineEdit *theEdit, PseudoAttribute *theAttribute);
    void accept();

};

namespace Ui
{
class MetaDataInfoDialog;
}

class MetadataInfo;

class MetaDataInfoDialog : public QDialog
{
    Q_OBJECT

private:
    MetadataInfo *_info;
    bool _isReadOnly;
    MetadataEditRow _projectEdit;
    MetadataEditRow _copyEdit;
    MetadataEditRow _versionEdit;
    MetadataEditRow _domainEdit;
    MetadataEditRow _nameEdit;

    void setSingleDatum(QLabel *label, PseudoAttribute *attribute);
    void setupNonEditable();
    void setupEditable();
    void actionOnVariable(MetadataEditRow *metaEditRow);

public:
    explicit MetaDataInfoDialog(const bool isReadOnly, MetadataInfo *info, QWidget *parent = 0);
    ~MetaDataInfoDialog();

protected:
    void accept();
private:
    Ui::MetaDataInfoDialog *ui;

private slots:
    void on_chkProject_clicked(bool state);
    void on_chkCopy_clicked(bool state);
    void on_chkVersion_clicked(bool state);
    void on_chkDomain_clicked(bool state);
    void on_chkName_clicked(bool state);

    friend class TestMetadata;
};

#endif // METADATAINFODIALOG_H
