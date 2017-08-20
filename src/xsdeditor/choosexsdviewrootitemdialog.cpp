#include "choosexsdviewrootitemdialog.h"
#include "ui_choosexsdviewrootitemdialog.h"
#include "xsdeditor/xsdwindow.h"
#include "utils.h"

ChooseXSDViewRootItemDialog::ChooseXSDViewRootItemDialog(QWidget *parent, QList<XSchemaElement*> elements) :
    QDialog(parent),
    ui(new Ui::ChooseXSDViewRootItemDialog)
{
    ui->setupUi(this);
    setupData(elements);
    ui->list->setFocus();
    enableOK();
}

ChooseXSDViewRootItemDialog::~ChooseXSDViewRootItemDialog()
{
    delete ui;
}

void ChooseXSDViewRootItemDialog::setupData(QList<XSchemaElement*> elements)
{
    ui->list->setSelectionMode(QAbstractItemView::SingleSelection);
    foreach(XSchemaElement* element, elements) {
        QListWidgetItem * item = new QListWidgetItem(element->name());
        item->setData(Qt::UserRole, qVariantFromValue((void*)element));
        ui->list->addItem(item);
    }
}

void ChooseXSDViewRootItemDialog::on_list_itemDoubleClicked(QListWidgetItem * /*item*/)
{
    accept();
}

void ChooseXSDViewRootItemDialog::on_list_itemSelectionChanged()
{
    enableOK();
}

void ChooseXSDViewRootItemDialog::accept()
{
    const int selRow = ui->list->currentRow();
    if(selRow >= 0) {
        XSchemaElement *sel = (XSchemaElement *)(ui->list->item(selRow)->data(Qt::UserRole).value<void*>());
        if(NULL != sel) {
            _selection = sel->name();
            QDialog::accept();
        }
    }
}

void ChooseXSDViewRootItemDialog::enableOK()
{
    bool enabledOk = false;
    if(ui->list->currentRow() != -1) {
        enabledOk = true ;
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enabledOk);
}

QString ChooseXSDViewRootItemDialog::selection()
{
    return _selection ;
}
