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

#include "xmlEdit.h"
#include "xmleditglobals.h"
#include "editelement.h"
#include "edittextnode.h"
#include "regola.h"
#include "utils.h"
#include "qxmleditdata.h"
#include "xmlutils.h"
#include "modules/namespace/namespacemanagementdialog.h"
#include "modules/utils/base64utils.h"
#include "modules/delegates/automitemeditorfactory.h"

#define MOD_WIDTH   (8)

EditElement::EditElement(QWidget * parent) : QDialog(parent)
{
    _parent = NULL ;
    _autoItemEditorFactory = NULL ;
    _namespaceManager = NULL ;
    _textModified = false;
    isStarted = false;
    modColor = QColor::fromRgb(255, 128, 128);
    isMixedContent = false ;
    ui.setupUi(this);
    ui.elementTable->setColumnWidth(T_COLUMN_MOD, MOD_WIDTH);
    ui.attrTable->setColumnWidth(A_COLUMN_MOD, MOD_WIDTH);
    _attributeTextDelegate = new AttributeColumnItemDelegate(ui.attrTable, A_COLUMN_TEXT, ui.attrTable);
    _attributeNameDelegate = new AttributeColumnItemDelegate(ui.attrTable, A_COLUMN_NAME, ui.attrTable);
    _autoItemEditorFactory = new AutoItemEditorFactory();
    _attributeNameDelegate->setItemEditorFactory(_autoItemEditorFactory);
    ui.attrTable->setItemDelegateForColumn(A_COLUMN_TEXT, _attributeTextDelegate);
    ui.attrTable->setItemDelegateForColumn(A_COLUMN_NAME, _attributeNameDelegate);
    target = NULL;
    checkNamespace();
    enableOK();
    //this->showMaximized();
    this->setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    Utils::TODO_NEXT_RELEASE("insert namespace for attribute");
    Utils::TODO_NEXT_RELEASE("remove namespace for attribute");
}

EditElement::~EditElement()
{
    _attributeNameDelegate->setItemEditorFactory(NULL);
    if(NULL != _autoItemEditorFactory) {
        delete _autoItemEditorFactory ;
    }
}

void EditElement::enableOK()
{
    bool isEnabled = true ;
    QString theTag = ui.editTag->text();
    if(theTag.isEmpty()) {
        isEnabled = false ;
    } else {
        isEnabled = checkTagSyntax(theTag) ;
    }
    ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isEnabled);
}


bool EditElement::checkTagSyntax(const QString &theTag)
{
    return Utils::checkXMLName(theTag);
}

void EditElement::on_editTag_textChanged(const QString & /*theText*/)
{
    checkNamespace();
    enableOK();
}


QHash<QString, QString> EditElement::getNsFromAttributes()
{
    QHash<QString, QString> nss;
    int rows = ui.attrTable->rowCount();
    for(int row = 0 ; row < rows ; row ++) {
        QTableWidgetItem *itemName = ui.attrTable->item(row, A_COLUMN_NAME);
        QTableWidgetItem *itemValue = ui.attrTable->item(row, A_COLUMN_TEXT);
        if((NULL != itemName) && (NULL != itemValue)) {
            QString name = itemName->text().trimmed() ;
            QString value = itemValue->text().trimmed() ;
            QString prefix;
            if(XmlUtils::getNsPrefix(name, prefix)) {
                nss.insert(prefix, value);
            } // if is declaring
        }
    } // for each attribute
    return nss ;
}

void EditElement::setTarget(Element* pTarget, Element *parent)
{
    target = pTarget ;
    _parent = parent;
    isMixedContent = target->isMixedContent();
    Element *theParent = (NULL != _parent) ? _parent : target->parent();
    QString path;
    if(NULL != theParent) {
        QStringList pathList = theParent->path();
        path = pathList.join("/");
        path = "/" + path ;

        _visibleNamespaces = target->findVisibleNamespaces();
    }
    path += "/" + target->tag();
    ui.path->setText(path);
    QString initialValue = target->tag();
    if(NULL != theParent) {
        if(initialValue.isEmpty()) {
            QString parentName;
            QString parentPrefix;
            XmlUtils::decodeQualifiedName(theParent->tag(), parentPrefix, parentName);
            initialValue = parentPrefix;
            if(!initialValue.isEmpty()) {
                initialValue += ":";
            }
        }
    }
    ui.editTag->setText(initialValue);
    ui.attrTable->setUpdatesEnabled(false);
    // sort alphabetically the attributes
    Regola *regola = (NULL != theParent) ? theParent->getParentRule() : NULL;
    if(((NULL == regola) && Regola::isSaveSortAlphaAttribute())
            || ((NULL != regola) && regola->isSavingSortingAttributes())) {
        QList<Attribute*> attributes = Element::sortAttributesList(target->attributes);
        foreach(Attribute * attr, attributes) {
            appendAttrNodeInTable(ui.attrTable, -1, attr->name, attr->value);
        }
    } else {
        foreach(Attribute * attr, target->attributes) {
            appendAttrNodeInTable(ui.attrTable, -1, attr->name, attr->value);
        }
    }
    ui.attrTable->resizeColumnsToContents();
    ui.attrTable->setUpdatesEnabled(true);
    // to free resources
    ui.elementTable->setUpdatesEnabled(false);
    if(!isMixedContent) {
        QVectorIterator<TextChunk*> tt(target->getTextChunks());
        while(tt.hasNext()) {
            TextChunk *tOrig = tt.next();
            appendTextNodeInTable(ui.elementTable, true, -1, tOrig->isCDATA, tOrig->text, NULL);
        }
    }
    foreach(Element * child, target->getItems()) {
        if(child->getType() == Element::ET_TEXT) {
            appendTextNodeInTable(ui.elementTable, true, -1, child->isCDATA(), child->text, NULL);
        } else {
            QString textToShow ;
            switch(child->getType()) {
            default:
                textToShow = tr("** child **");
                break;
            case Element::ET_COMMENT:
                textToShow = tr("** comment **");
                break;
            case Element::ET_ELEMENT:
                textToShow = tr("** element: <%1>").arg(child->tag());
                break;
            case Element::ET_PROCESSING_INSTRUCTION:
                textToShow = tr("** processing instruction: %1").arg(child->getPITarget());
                break;
            }
            appendTextNodeInTable(ui.elementTable, false, -1, false, textToShow, child);
        }
    }
    ui.elementTable->setUpdatesEnabled(true);
    ui.elementTable->resizeRowsToContents();
    if(NULL != regola) {
        _attributeNameDelegate->setSourceData(regola->attributeNamesPoolByValue());
    }
    enableOK();
    checkNamespace();
    _textModified = false;
    isStarted = true ;
}

void EditElement::accept()
{
    if(!updateTarget(target)) {
        return;
    }
    QDialog::accept();
}

bool EditElement::updateTarget(Element *targetElement)
{
    if(NULL == targetElement) {
        error(tr("No element"));
        return false;
    }
    QString tag = ui.editTag->text();
    if(0 == tag.length()) {
        error(tr("Tag text is invalid"));
        return false ;
    }
    if(NULL != targetElement->getParentRule()) {
        targetElement->_tag = targetElement->getParentRule()->addNameToPool(tag) ;
    } else {
        targetElement->_tag = tag;
    }
    // rebuild attributes
    targetElement->clearAttributes();
    int rows = ui.attrTable->rowCount();
    for(int row = 0 ; row < rows ; row ++) {
        QTableWidgetItem *itemName = ui.attrTable->item(row, A_COLUMN_NAME);
        QString name = itemName->text().trimmed() ;
        if(!validateAttr(name)) {
            error(tr("An attribute is invalid at row %1").arg(row));
            return false ;
        }
    }
    Regola *regola = (NULL != _parent) ? _parent->getParentRule() : NULL;
    for(int row = 0 ; row < rows ; row ++) {
        QTableWidgetItem *itemName = ui.attrTable->item(row, A_COLUMN_NAME);
        QTableWidgetItem *itemValue = ui.attrTable->item(row, A_COLUMN_TEXT);
        QString name = itemName->text().trimmed() ;
        if(NULL != regola) {
            name = regola->getAttributeNameString(name);
        }
        QString value = itemValue->text().trimmed() ;
        targetElement->addAttribute(name, value);
    }

    Utils::TODO_NEXT_RELEASE("this part must be tested very well");
    //if(_textModified) {
    targetElement->clearTextNodes();

    isMixedContent = false ;
    bool isElement = false;
    bool isText = false;

    rows = ui.elementTable->rowCount();
    for(int row = 0 ; row < rows ; row ++) {
        QTableWidgetItem *itemCDATA = ui.elementTable->item(row, T_COLUMN_CDATA);
        Element *element = getUserData(itemCDATA);
        if((NULL == element) || ((NULL != element) && (element->getType() == Element::ET_TEXT))) {
            isText = true ;
            if(isElement) {
                break;
            }
        } else {
            isElement = true ;
            if(isText) {
                break;
            }
        }
    }
    if(isElement && isText) {
        isMixedContent = true ;
    }
    foreach(Element * child, targetElement->getItems()) {
        if(child->getType() == Element::ET_TEXT) {
            child->autoDelete(true);
        }
    }

    if(isMixedContent) {
        targetElement->getChildItems()->clear();
        rows = ui.elementTable->rowCount();
        for(int row = 0 ; row < rows ; row ++) {
            QTableWidgetItem *itemCDATA = ui.elementTable->item(row, T_COLUMN_CDATA);
            QTableWidgetItem *itemValue = ui.elementTable->item(row, T_COLUMN_TEXT);
            Element *element = getUserData(itemCDATA);
            if((NULL == element) || ((NULL != element) && (element->getType() == Element::ET_TEXT))) {
                Element *newElement = new Element(targetElement->getParentRule(), Element::ET_TEXT, targetElement);
                newElement->setTextOfTextNode(textFromItem(itemValue), itemCDATA->checkState() == Qt::Checked);
                newElement->markEdited();
                targetElement->getItems().append(newElement);
                newElement->caricaFigli(targetElement->getUI()->treeWidget(), targetElement->getUI(), targetElement->getParentRule()->getPaintInfo(), true, row);
            } else {
                targetElement->getItems().append(element);
            }
        }
    } else {
        rows = ui.elementTable->rowCount();
        for(int row = 0 ; row < rows ; row ++) {
            QTableWidgetItem *itemCDATA = ui.elementTable->item(row, T_COLUMN_CDATA);
            QTableWidgetItem *itemValue = ui.elementTable->item(row, T_COLUMN_TEXT);
            Element *element = getUserData(itemCDATA);
            if((NULL == element) || ((NULL != element) && (element->getType() == Element::ET_TEXT))) {
                TextChunk *newText = new TextChunk(itemCDATA->checkState() == Qt::Checked, textFromItem(itemValue));
                targetElement->addTextNode(newText);
            }
        }
    }

    targetElement->markEdited();
    return true;
}

void EditElement::error(const QString& message)
{
    Utils::error(this, message);
}

bool EditElement::validateAttr(const QString &name)
{
    return Utils::checkXMLName(name);
}

void EditElement::setUpdatedAttr(const int row)
{
    if(isStarted && (row >= 0)) {
        QTableWidgetItem * item = ui.attrTable->item(row, A_COLUMN_MOD);
        if(NULL != item) {
            item->setBackgroundColor(modColor);
        }
        checkNamespace();
    }
}

void EditElement::setAttrFocus(const int row)
{
    if(isStarted && (row >= 0)) {
        ui.attrTable->setCurrentCell(row, 1, QItemSelectionModel::ClearAndSelect);
        ui.attrTable->setFocus();
    }
}

void EditElement::setUpdatedElement(const int row)
{
    if(isStarted && (row >= 0)) {
        QTableWidgetItem * item = ui.elementTable->item(row, A_COLUMN_MOD);
        if(NULL != item) {
            item->setBackgroundColor(modColor);
        }
    }
}

void EditElement::on_newAttribute_clicked()
{
    int currentRow = ui.attrTable->currentRow();
    int newRow = getNextRow(currentRow);
    newRow = appendAttrNodeInTable(ui.attrTable, newRow, "", "");
    setUpdatedAttr(newRow);
    setAttrFocus(newRow);
}

void EditElement::on_attrTable_itemChanged(QTableWidgetItem * item)
{
    setUpdatedAttr(ui.attrTable->row(item));
    ui.attrTable->resizeColumnsToContents();
    ui.attrTable->horizontalHeader()->setStretchLastSection(true);
}

void EditElement::on_delAttribute_clicked()
{
    QList<QTableWidgetItem *> selectedItems = ui.attrTable->selectedItems();
    QSet<int> rows;
    foreach(QTableWidgetItem * item, selectedItems) {
        rows.insert(item->row());
    }
    if(!rows.isEmpty()) {
        QList<int> rowList = rows.toList() ;
        qSort(rowList.begin(), rowList.end(), qGreater<int>());
        const int rowCount = rows.count();
        if(rowCount > 0) {
            // remove from the last row
            foreach(const int row, rowList) {
                ui.attrTable->removeRow(row);
            }
        }
        checkNamespace();
    } else {
        Utils::error(this, tr("No attribute selected"));
    }
}

void EditElement::errorNoAttrSel()
{
    Utils::error(this, tr("No attribute selected, can't execute command."));
}

void EditElement::errorNoAttrData()
{
    Utils::error(this, tr("Please, insert name and value for attribute."));
}


void EditElement::on_textEdit_clicked()
{
    int currentRow = ui.elementTable->currentRow();
    if(currentRow >= 0) {
        QString filePath = "" ;
        if((NULL != target) && (NULL != target->getParentRule())) {
            filePath = target->getParentRule()->fileName();
        }
        EditTextNode editDialog(false, filePath, this);
        editDialog.setWindowModality(Qt::WindowModal);
        editDialog.setText(textFromItem(ui.elementTable->item(currentRow, T_COLUMN_TEXT)));
        if(editDialog.exec() == QDialog::Accepted) {
            _textModified = true ;
            setTextToItem(ui.elementTable->item(currentRow, T_COLUMN_TEXT), editDialog.getText());
            setUpdatedElement(currentRow);
            ui.elementTable->resizeRowToContents(currentRow);
        }
    }
}

void EditElement::on_elementTable_itemSelectionChanged()
{
    int currentRow = ui.elementTable->currentRow();
    int rows = ui.elementTable->rowCount();
    bool isSelected = false;
    if(currentRow >= 0) {
        isSelected = true ;
    }
    ui.textEdit->setEnabled(isSelected);
    if(isMixedContent) {
        QTableWidgetItem *item = ui.elementTable->item(currentRow, 0);
        if(isElementText(item)) {
            ui.textDel->setEnabled(isSelected);
        } else {
            ui.textDel->setEnabled(false);
        }
    } else {
        ui.textDel->setEnabled(isSelected);
    }

    if(currentRow > 0) {
        ui.textUp->setEnabled(true);
    } else {
        ui.textUp->setEnabled(false);
    }
    if((currentRow >= 0) && (rows > 0) && (currentRow < (rows - 1)))   {
        ui.textDown->setEnabled(true);
    } else {
        ui.textDown->setEnabled(false);
    }
}

void EditElement::on_attrTable_itemSelectionChanged()
{
    int currentRow = ui.attrTable->currentRow();
    bool isSel = false;
    if(currentRow >= 0) {
        isSel = true ;
    }
    ui.delAttribute->setEnabled(isSel);
    ui.cmdToBase64->setEnabled(isSel);
    ui.cmdFromBase64->setEnabled(isSel);
    ui.cmdLoadFileBase64->setEnabled(isSel);
    ui.cmdSaveFileBase64->setEnabled(isSel);
}

void EditElement::on_textDel_clicked()
{
    int currentRow = ui.elementTable->currentRow();
    if(currentRow >= 0) {
        if(isMixedContent) {
            QTableWidgetItem *item = ui.elementTable->item(currentRow, 0);
            if(! isElementText(item)) {
                return ;
            }
        }
        if(!Utils::isUnitTest) {
            if(QMessageBox::No == QMessageBox::question(this, QXmlEditGlobals::appTitle(),
                    tr("This operation will destroy the text node. Do you really want to continue ?"),
                    QMessageBox::Yes | QMessageBox::No)) {
                return ;
            }
        }
        ui.elementTable->removeRow(currentRow);
        _textModified = true ;
    } else {
        Utils::error(this, tr("No text node selected"));
    }
}

void EditElement::on_textAdd_clicked()
{
    int currentRow = ui.elementTable->currentRow();
    int newRow = getNextRow(currentRow);
    newRow = appendTextNodeInTable(ui.elementTable, true, newRow, false, "", NULL);
    setUpdatedElement(newRow);
    _textModified = true ;
}

void EditElement::setTextToItem(QTableWidgetItem *item, const QString &text)
{
    QString textToShow = text;
    if(textToShow.length() > 100) {
        textToShow = textToShow.left(100);
        textToShow += "...";
    }
    item->setText(textToShow);
    item->setData(Qt::UserRole + 1, text);
}

QString EditElement::textFromItem(QTableWidgetItem *item)
{
    return item->data(Qt::UserRole + 1).toString();
}

void EditElement::setEnableAllControls(const bool enableAllControls)
{
    ui.elementTable->setEnabled(enableAllControls);
    ui.textAdd->setEnabled(enableAllControls);
    ui.attrTable->setEnabled(enableAllControls);
    ui.newAttribute->setEnabled(enableAllControls);
}

int EditElement::appendTextNodeInTable(QTableWidget *table, const bool isEditable, const int desiredRow, const bool isCDATA, const QString &text, void *userData)
{
    int row = desiredRow;
    if(row >= 0) {
        table->insertRow(row);
    } else {
        row = table->rowCount();
        table->setRowCount(row + 1);
    }
    QTableWidgetItem *itemMod = new QTableWidgetItem("");
    QTableWidgetItem *itemCDATA = new QTableWidgetItem();
    itemCDATA->setCheckState(isCDATA ? Qt::Checked : Qt::Unchecked);
    itemCDATA->setFlags(itemCDATA->flags() & (~Qt::ItemIsEditable));
    itemCDATA->setData(Qt::UserRole, qVariantFromValue((void*)userData));
    itemMod->setFlags(itemMod->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable));
    itemMod->setData(Qt::UserRole, qVariantFromValue((void*)userData));
    if(!isEditable) {
        itemCDATA->setFlags(itemCDATA->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable));
        itemCDATA->setBackgroundColor(QColor::fromRgb(0xC0, 0xC0, 0xC0));
    }

    QTableWidgetItem *itemText = new QTableWidgetItem("");
    setTextToItem(itemText, text);

    itemText->setFlags(itemText->flags() & (~Qt::ItemIsEditable));
    table->setItem(row, T_COLUMN_CDATA, itemCDATA);
    table->setItem(row, T_COLUMN_TEXT, itemText);
    table->setItem(row, T_COLUMN_MOD, itemMod);
    itemText->setData(Qt::UserRole, qVariantFromValue((void*)userData));
    if(!isEditable) {
        itemText->setFlags(itemText->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable));
        itemText->setBackgroundColor(QColor::fromRgb(0xC0, 0xC0, 0xC0));
    }
    _textModified = true;
    return row ;
}

int EditElement::appendAttrNodeInTable(QTableWidget *table, const int desiredRow, const QString &name, const QString &value)
{
    int row = desiredRow;
    if(row >= 0) {
        table->insertRow(row);
    } else {
        row = table->rowCount();
        table->setRowCount(row + 1);
    }
    QTableWidgetItem *itemName = new QTableWidgetItem(name);
    QTableWidgetItem *itemValue = new QTableWidgetItem(value);
    QTableWidgetItem *itemMod = new QTableWidgetItem("");
    itemMod->setFlags(itemMod->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable));
    table->setItem(row, A_COLUMN_NAME, itemName);
    table->setItem(row, A_COLUMN_TEXT, itemValue);
    table->setItem(row, A_COLUMN_MOD, itemMod);
    return row ;
}

void EditElement::on_elementTable_cellClicked(int row, int column)
{
    QTableWidgetItem *itemText = ui.elementTable->itemAt(row, column);
    if(NULL != itemText) {
        // if in the edit zone
        if(T_COLUMN_CDATA != column) {
            if(isMixedContent) {
                Element * element = getUserData(itemText);
                if(NULL != element) {
                    if(element->getType() == Element::ET_TEXT) {
                        on_textEdit_clicked();
                    }
                }
            } else {
                on_textEdit_clicked();
            }
        }
    }
}

void EditElement::on_textUp_clicked()
{
    moveUp(ui.elementTable);
    _textModified = true ;
}

void EditElement::on_textDown_clicked()
{
    moveDown(ui.elementTable);
    _textModified = true ;
}


void EditElement::swapRow2(QTableWidget *table, const int rowStart, const int rowEnd)
{
    QTableWidgetItem *i1 = table->takeItem(rowStart, 0);
    QTableWidgetItem *i2 = table->takeItem(rowStart, 1);
    QTableWidgetItem *i3 = table->takeItem(rowStart, 2);
    table->setItem(rowStart, 0, table->takeItem(rowEnd, 0));
    table->setItem(rowStart, 1, table->takeItem(rowEnd, 1));
    table->setItem(rowStart, 2, table->takeItem(rowEnd, 2));
    table->setItem(rowEnd, 0, i1);
    table->setItem(rowEnd, 1, i2);
    table->setItem(rowEnd, 2, i3);
}

void EditElement::moveDown(QTableWidget *table)
{
    int currentRow = table->currentRow();
    int rows = table->rowCount();
    if((currentRow >= 0) && (rows > 0) && (currentRow < (rows - 1)))   {
        QTableWidgetItem *currentItem = table->currentItem();
        swapRow2(table, currentRow, currentRow + 1);
        table->setCurrentCell(currentRow + 1, T_COLUMN_CDATA);
        table->setCurrentItem(currentItem);
        setUpdatedElement(currentRow + 1);
    }
}

void EditElement::moveUp(QTableWidget *table)
{
    int currentRow = table->currentRow();
    if(currentRow >= 0) {
        QTableWidgetItem *currentItem = table->currentItem();
        swapRow2(table, currentRow, currentRow - 1);
        table->setCurrentCell(currentRow - 1, T_COLUMN_CDATA);
        table->setCurrentItem(currentItem);
        setUpdatedElement(currentRow - 1);
    }
}

int EditElement::getNextRow(const int currentRow)
{
    if(currentRow < 0) {
        return -1 ;
    }
    return currentRow + 1;
}

Element * EditElement::getUserData(QTableWidgetItem *item)
{
    QVariant data = item->data(Qt::UserRole);
    Element *pElement = (Element *)data.value<void*>();
    return pElement ;
}

bool EditElement::isElementText(QTableWidgetItem *item)
{
    if(NULL != item) {
        Element * element = getUserData(item);
        if(NULL != element) {
            if(element->getType() == Element::ET_TEXT) {
                return true ;
            }
        } else {
            return true ;
        }
    }
    return false;
}

void EditElement::sendSelect(const int row)
{
    this->ui.elementTable->setCurrentCell(row, 1/*, QItemSelectionModel::Select*/);
}

void EditElement::sendDeleteCommand()
{
    on_textDel_clicked();
}

void EditElement::sendModifyCommand(const QString &newData)
{
    int currentRow = ui.elementTable->currentRow();
    if(currentRow >= 0) {
        setTextToItem(ui.elementTable->item(currentRow, T_COLUMN_TEXT), newData);
        ui.elementTable->resizeRowToContents(currentRow);
    }
}

void EditElement::sendAddCommand(const QString &newData)
{
    int currentRow = ui.elementTable->currentRow();
    appendTextNodeInTable(ui.elementTable, true, getNextRow(currentRow), false, newData, NULL);
}

void EditElement::sendMoveUpCommand()
{
    on_textUp_clicked();
}

void EditElement::sendMoveDownCommand()
{
    on_textDown_clicked();
}

void EditElement::doBase64Operation(const bool isFromBase64)
{
    int currentRow = ui.attrTable->currentRow();
    int currentColumn = ui.attrTable->currentColumn();
    QTableWidgetItem *currentItem = ui.attrTable->currentItem() ;
    if((NULL != currentItem) && (currentRow >= 0) && ((A_COLUMN_NAME == currentColumn) || (A_COLUMN_TEXT == currentColumn))) {
        //ui.attrTable->closePersistentEditor(currentItem);
        QString text = currentItem->text();
        QString newText ;
        if(isFromBase64) {
            newText = Utils::fromBase64(text);
        } else {
            newText = Utils::toBase64(text);
        }
        currentItem->setText(newText);
        setUpdatedAttr(currentRow);
        ui.attrTable->setCurrentItem(currentItem);
        ui.attrTable->setFocus();
    }

}

void EditElement::on_cmdToBase64_clicked()
{
    doBase64Operation(false);
}

void EditElement::on_cmdFromBase64_clicked()
{
    doBase64Operation(true);
}

void EditElement::checkNamespace()
{
    QString name = ui.editTag->text().trimmed();
    QString prefix;
    QString localName;
    XmlUtils::decodeQualifiedName(name, prefix, localName);
    bool nsOk = prefix.isEmpty() ;

    if(!nsOk) {
        QHash<QString, QString> thisDeclarations = getNsFromAttributes();
        if(!prefix.isEmpty() && thisDeclarations.contains(prefix)) {
            nsOk = true ;
        }
        if(!nsOk) {
            nsOk = true ;
            if(!prefix.isEmpty() && !_visibleNamespaces.contains(prefix)) {
                nsOk = false;
            }
        }
    }
    ui.lblWarnNs->setVisible(!nsOk);
}

void EditElement::on_cmdNamespaces_clicked()
{
    Element element(ui.editTag->text(), "", NULL, NULL);
    // rebuild attributes
    int rows = ui.attrTable->rowCount();
    for(int row = 0 ; row < rows ; row ++) {
        QTableWidgetItem *itemName = ui.attrTable->item(row, A_COLUMN_NAME);
        QString name = itemName->text().trimmed() ;
        QTableWidgetItem *itemValue = ui.attrTable->item(row, A_COLUMN_TEXT);
        QString value = itemValue->text() ;
        element.addAttribute(name, value);
    }
    element.setParent((NULL != _parent) ? _parent : target->parent());
    NamespaceManagementDialog dlg(this, &element, _namespaceManager);
    if(dlg.exec() == QDialog::Accepted) {
        NamespaceCommands *commands = dlg.getCommands();
        if(NULL != commands) {
            applyNamespaceOper(commands);
            delete commands;
        }
    }
    element.setParent(NULL);
}

void EditElement::applyNamespaceOper(NamespaceCommands *commands)
{
    if(NULL != commands) {
        QList<NamespaceSpec*> allNs ;
        updateElementTag(commands->useNsElement(), commands->tagSpec(), commands->declareNs(), allNs);
        allNs += commands->namespaces();
        applyOtherNamespaces(allNs);
    }
}

void EditElement::updateElementTag(const bool useNamespace, NamespaceSpec* namespaceSpec, const NamespaceCommands::ENSDecl decl, QList<NamespaceSpec*> &allNs)
{
    QString prefix;
    QString localName;
    XmlUtils::decodeQualifiedName(ui.editTag->text(), prefix, localName);
    if(prefix != namespaceSpec->prefix) {
        QString qualifiedName = XmlUtils::makeQualifiedName(namespaceSpec->prefix, localName);
        ui.editTag->setText(qualifiedName);
    }
    if(useNamespace) {
        if(decl == NamespaceCommands::DeclareInElement) {
            allNs.append(namespaceSpec);
        }
    }
}

void EditElement::applyOtherNamespaces(QList<NamespaceSpec*> namespaces)
{
    ui.attrTable->setUpdatesEnabled(false);
    // delete ns declared, but not present in list;
    QList<NamespaceSpec*> presentInList;
    int rows = ui.attrTable->rowCount();
    for(int row = rows - 1 ; row >= 0 ; row --) {
        QTableWidgetItem *itemName = ui.attrTable->item(row, A_COLUMN_NAME);
        QTableWidgetItem *itemValue = ui.attrTable->item(row, A_COLUMN_TEXT);
        QString name = itemName->text().trimmed() ;
        QString value = itemValue->text().trimmed() ;
        if(XmlUtils::isDeclaringNS(name)) {
            QString prefix;
            XmlUtils::getNsPrefix(name, prefix);
            NamespaceSpec * existingCommand = findNsCommand(prefix, value, namespaces);
            if(NULL == existingCommand) {
                // remove the declaration
                ui.attrTable->removeRow(row);
            } else {
                // mark for adding
                presentInList.append(existingCommand);
            }
        }
    }
    // now insert new values
    foreach(NamespaceSpec * cmd, namespaces) {
        if(! presentInList.contains(cmd)) {
            int row = appendAttrNodeInTable(ui.attrTable, -1, XmlUtils::makeNSDeclaration(cmd->prefix), cmd->uri);
            setUpdatedAttr(row);
        }
    }
    //------
    ui.attrTable->setUpdatesEnabled(true);
}

NamespaceSpec * EditElement::findNsCommand(const QString &prefix, const QString &uri, QList<NamespaceSpec*> namespaces)
{
    foreach(NamespaceSpec * ns, namespaces) {
        if((prefix == ns->prefix) && (uri == ns->uri)) {
            return ns;
        }
    }
    return NULL ;
}


NamespaceManager *EditElement::namespaceManager() const
{
    return _namespaceManager;
}

void EditElement::setNamespaceManager(NamespaceManager *namespaceManager)
{
    _namespaceManager = namespaceManager;
}

void EditElement::on_cmdLoadFileBase64_clicked()
{
    int currentRow = ui.attrTable->currentRow();
    QTableWidgetItem *currentItem = ui.attrTable->currentItem() ;
    if(!((NULL != currentItem) && (currentRow >= 0))) {
        return ;
    }
    QString filePath = QFileDialog::getOpenFileName(this,
                       tr("Choose a File to be Transformed in Base 64 Encoded Text"),
                       QXmlEditData::sysFilePathForOperation(""),
                       Utils::getFileFilterForOpenFile());
    if(!filePath.isEmpty()) {
        bool isError = true ;
        bool isAbort = false ;
        Base64Utils base64Utils;
        QString strBase64 = base64Utils.loadFromBinaryFile(Base64Utils::RFC4648Standard, this, filePath, isError, isAbort);
        if(!(isError || isAbort)) {
            QTableWidgetItem *currentItem = ui.attrTable->item(currentRow, A_COLUMN_TEXT);
            currentItem->setText(strBase64);
            setUpdatedAttr(currentRow);
            ui.attrTable->setCurrentItem(currentItem);
            ui.attrTable->setFocus();
        }
    }
}

void EditElement::on_cmdSaveFileBase64_clicked()
{
    int currentRow = ui.attrTable->currentRow();
    QTableWidgetItem *currentItem = ui.attrTable->currentItem() ;
    if(!((NULL != currentItem) && (currentRow >= 0))) {
        return ;
    }
    QTableWidgetItem *itemValue = ui.attrTable->item(currentRow, A_COLUMN_TEXT);
    QString text = itemValue->text() ;
    Base64Utils base64Utils;
    base64Utils.saveBase64ToBinaryFile(Base64Utils::RFC4648Standard, this, text, QXmlEditData::sysFilePathForOperation(""));
}

