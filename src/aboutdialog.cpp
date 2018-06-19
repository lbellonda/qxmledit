/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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

#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "qxmledit.h"
#include "xmlEdit.h"
#include "qxmeditmetainfo.h"
#include "utils.h"

AboutDialog::AboutDialog(QWidget *parent,
                         const QString &applicationName, const QString &version,
                         const QString &copyRight, const QString &license, const QString &otherLicenses,
                         const QList<AuthorInfo*> &authors) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->applicationName->setToolTip(tr("If you like or find useful this program, help others, even when you know they can’t help you back."));
    QString pgmTitle = ui->applicationName->text();
    pgmTitle.replace("QXmlEdit", applicationName);
    pgmTitle.replace("xxx", version);
    ui->applicationName->setText(pgmTitle);
    ui->copyR->setText(copyRight);
    ui->license->setText(tr("License: %1").arg(license)) ;
    ui->icons->setText(otherLicenses);
    ui->envir->setText(tr("Compiled with Qt version: %1, runtime Qt version: %2").arg(QT_VERSION_STR).arg(qVersion())) ;

    QTreeWidgetItem *developers = new QTreeWidgetItem(0);
    developers->setText(0, tr("Developers"));
    ui->authorsWidget->addTopLevelItem(developers);

    QTreeWidgetItem *contributors = new QTreeWidgetItem(0);
    contributors->setText(0, tr("Contributors"));
    ui->authorsWidget->addTopLevelItem(contributors);

    QTreeWidgetItem *translators = new QTreeWidgetItem(0);
    translators->setText(0, tr("Translators"));
    ui->authorsWidget->addTopLevelItem(translators);

    QString projectLocation = ui->lblProjectLocation->text() ;
    projectLocation.replace("$PJLOC", QXMLEDIT_PROJECT_LOCATION);
    projectLocation.replace("$JPSITE", QXMLEDIT_SITE);
    ui->lblProjectLocation->setText(projectLocation);

    QString issuesLocation = ui->lblIssues->text();
    issuesLocation.replace("$ISSUESLOC", QXMLEDIT_ISSUES_LOCATION);
    ui->lblIssues->setText(issuesLocation);

    foreach(AuthorInfo * author,  authors) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);

        if(author->role == AR_DEVELOPER) {
            item->setText(0, QString("%1 %2").arg(author->name).arg(contactInfo(author->contactInfo)));
            developers->addChild(item);
        } else if(author->role == AR_TRANSLATOR) {
            item->setText(0, QString("%1: %2 %3").arg(author->specialities).arg(author->name).arg(contactInfo(author->contactInfo)));
            translators->addChild(item);
        } else if(author->role == AR_CONTRIBUTOR) {
            item->setText(0, QString("%1 %2").arg(author->name).arg(contactInfo(author->contactInfo)));
            contributors->addChild(item);
        } else {
            delete item ;
            //TODO printf( "%s", QString("ROLE NOT FOUND %1").arg(author->name).toAscii().data() );
        }
    }
    ui->authorsWidget->expandAll();
}

QString AboutDialog::contactInfo(const QString &info)
{
    if(info.isEmpty()) {
        return "";
    }
    return QString("(%1)").arg(info);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

