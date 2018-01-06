/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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


#include "choosestyledialog.h"
#include "ui_choosestyledialog.h"
#include "xmleditwidget.h"
#include "utils.h"

#define SAMPLE_DATA ":styles/taskStyles.xml"

ChooseStyleDialog::ChooseStyleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseStyleDialog)
{
    _selectedStyle = NULL ;
    ui->setupUi(this);
    setup();
}

ChooseStyleDialog::~ChooseStyleDialog()
{
    delete ui;
}

void ChooseStyleDialog::setup()
{
    _displaySettingsPredefined.makePredefined();

    _displaySettings[Compact].makeCompact();
    _displaySettings[Extended].makeExtended();
    _displaySettings[FullDetail].makeFullDetail();
    _displaySettings[Essential].makeEssential();

    _editorEssential = setupEditor(Essential, ui->preview0->parentWidget(), &_displaySettings[Essential]);
    _editorCompact = setupEditor(Compact, ui->preview1->parentWidget(), &_displaySettings[Compact]);
    _editorExtended = setupEditor(Extended, ui->preview2->parentWidget(), &_displaySettings[Extended]);
    _editorFullDetails = setupEditor(FullDetail, ui->preview3->parentWidget(), &_displaySettings[FullDetail]);
    //
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    //--
    on_tabStyle_currentChanged(ui->tabStyle->currentIndex());
}


XmlEditWidget *ChooseStyleDialog::setupEditor(const int index, QWidget *theParent, DisplayStyleSetting *setting)
{
    theParent->setLayout(new QVBoxLayout());
    XmlEditWidget *editor = new XmlEditWidget(theParent);
    editor->showButtons(false);
    editor->showToolbar(false);
    editor->setEncodingLabelVisible(false);
    editor->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    theParent->layout()->addWidget(editor);
    setting->applyToPaintInfo(editor->getPaintInfo());
    loadSampleData(editor);
    editor->invalidatePaintData(true);
    ui->tabStyle->setTabToolTip(index, makeDetail(setting));
    return editor;
}

void ChooseStyleDialog::loadSampleData(XmlEditWidget *editor)
{
    QFile file(SAMPLE_DATA);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            editor->setDocument(document, "", false);
            editor->setReadOnly(true);
        }
        file.close();
    }
}

void ChooseStyleDialog::on_tabStyle_currentChanged(int index)
{
    QString detail ;
    QString use;
    bool enableUse = false;
    if(index >= 0) {
        detail = makeDetail(&_displaySettings[index]);
        use = tr("Use %1").arg(_displaySettings[index].description);
        enableUse = true ;
    }
    ui->details->setText(detail);
    ui->cmdUseThis->setEnabled(enableUse);
    ui->cmdUseThis->setText(use);
}

QString ChooseStyleDialog::makeDetail(DisplayStyleSetting *setting)
{
    QString detail ;
    detail += setting->description;
    detail += ":";
    QChar dot(0x2022);
    if(setting->isOneAttrPerLine) {
        detail += tr("\n%1 One attribute per line").arg(dot);
    }
    if(setting->isOneAttrPerLine) {
        detail += tr("\n%1 Use child index").arg(dot);
    }
    if(setting->isCompactView) {
        detail += tr("\n%1 Compact view").arg(dot);
    }
    if(setting->isShowUnBase64) {
        detail += tr("\n%1 Show text base 64 coded").arg(dot);
    }
    if(setting->isShowElementSize) {
        detail += tr("\n%1 Show element size\n   and children").arg(dot);
    }
    if(setting->isShowElementTextLength) {
        detail += tr("\n%1 Show text length").arg(dot);
    }
    if(setting->isShowFullComments) {
        detail += tr("\n%1 Show full comments").arg(dot);
    }
    if(setting->isSortAttributesAlpha) {
        detail += tr("\n%1 Sort attributes").arg(dot);
    }
    if(setting->attributesColumnLimit >= 0) {
        detail += tr("\n%1 Limit attributes\n   line length").arg(dot);
    }
    return detail;
}

void ChooseStyleDialog::reject()
{
    _selectedStyle = NULL ;
    QDialog::reject();
}

void ChooseStyleDialog::on_cmdUsePredefined_clicked()
{
    _selectedStyle = &_displaySettingsPredefined;
    QDialog::accept();
}

void ChooseStyleDialog::on_cmdUseThis_clicked()
{
    int currentIndex = ui->tabStyle->currentIndex();
    if((currentIndex >= 0) && (currentIndex < MaxStyle)) {
        _selectedStyle = &_displaySettings[currentIndex];
        QDialog::accept();
    }
}

DisplayStyleSetting *ChooseStyleDialog::selectedStyle()
{
    return _selectedStyle ;
}
