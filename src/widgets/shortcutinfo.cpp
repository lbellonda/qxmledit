/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#include "shortcutinfo.h"
#include "ui_shortcutinfo.h"
#include "widgets/qlabelwithsignals.h"
#include "qxmleditconfig.h"
#include "utils.h"

class ActionKeyInfo
{
public:
    QString name;
    QString shortcut;
    QString text;

    inline bool operator<(const ActionKeyInfo *a2)
    {
        return this->shortcut < a2->shortcut ;
    }

    ActionKeyInfo();
    ActionKeyInfo(const QString &theName, const QString &theShortcut, const QString &theText);
    ~ActionKeyInfo();
};

ActionKeyInfo::ActionKeyInfo()
{}

ActionKeyInfo::ActionKeyInfo(const QString &theName, const QString &theShortcut, const QString &theText)
{
    name = theName ;
    shortcut = theShortcut ;
    text = theText ;
}

ActionKeyInfo::~ActionKeyInfo()
{}

//-------------------------------------------------------------------

ShortcutInfo::ShortcutInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShortcutInfo)
{
    _lightTheme = true ;
    ui->setupUi(this);
    loadActions();
    setupData();
}

ShortcutInfo::~ShortcutInfo()
{
    removeWidgets();
    EMPTYPTRLIST(_actions, ActionKeyInfo);
    delete ui;
}

static const QString lightTheme = "border: 1px solid #000040; border-radius:10px; padding:0px;margin:0px;\nbackground-color: qlineargradient(spread:pad, x1:0.469613, y1:1, x2:0.48563, y2:0.477, stop:0 rgba(228, 228, 228, 255), stop:1 rgba(255, 255, 255, 255));";
static const QString colorTextLightTheme = "#5d5d5d";
static const QString colorFKLightTheme = "#000000";

static const QString darkTheme = "border: 1px solid #000040; border-radius:10px; padding:0px;margin:0px;\nbackground-color: qlineargradient(spread:pad, x1:0.469613, y1:1, x2:0.48563, y2:0.477, stop:0 rgba(64,64, 64, 255), stop:1 rgba(128, 128, 128, 255));";
static const QString colorTextDarkTheme = "#FFFFFF";
static const QString colorFKDarkTheme = "#FFFFFF";


static const QString templateData = "<html><head/><body><p><span style='font-weight:600;color:$(FKEYCOLOR);'>$(FKEY)</span><br/><span style='color:$(COLORTEXT);'>$(TEXT)</span></p></body></html>";

void ShortcutInfo::chooseTheme()
{
    _lightTheme = Config::getBool(Config::KEY_INFO_SHORTCUT_LIGHT_THEME, false);
    if(_lightTheme) {
        //_themeCSS = lightTheme ;
        _themeCSS = readCSSData(":/css/buttonLight.css");
        _textColor = colorTextLightTheme;
        _textKeyColor = colorFKLightTheme;
    } else {
        //_themeCSS = darkTheme ;
        _themeCSS = readCSSData(":/css/buttonDark.css");
        _textColor = colorTextDarkTheme;
        _textKeyColor = colorFKDarkTheme;
    }
}

QString ShortcutInfo::readCSSData(const QString &name)
{
    QString result ;
    QFile file(name);
    if(file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream textStream(&file);
        textStream.setCodec(QTextCodec::codecForName("UTF-8"));
        result = textStream.readAll();
        file.close();
    }
    return result ;
}

void ShortcutInfo::setupData()
{
    ui->page->setStyleSheet("background-color: black;border: 1px solid white;");
    ui->stackedWidget->setStyleSheet("background-color: black;border: 1px solid white;");
    chooseTheme();
    QStringList labels ;
    labels << tr("Function Keys") << tr("Ctrl+Shift") << tr("Ctrl+Alt") << tr("Ctrl+Fn") << tr("Ctrl+Num") << tr("Ctrl") << tr("Others");
    QList<int> codes ;
    codes << F << CS << CA << CF << CN << C << O;
    Utils::loadComboCodedArrays(ui->type, F, labels, codes);
    on_type_currentIndexChanged(-1);
}

void ShortcutInfo::on_type_currentIndexChanged(int /*index*/)
{
    // select list
    const int index = ui->type->currentIndex();
    if(-1 == index) {
        return ;
    }
    const int value = ui->type->itemData(index, Qt::UserRole).toInt();
    const EClasses thisClass = static_cast<EClasses>(value);
    switch(thisClass) {
    default:
        break;
    case F:
        refreshButtons(_F);
        break;
    case CS:
        refreshButtons(_ctrShift);
        break;
    case CA:
        refreshButtons(_ctrAlt);
        break;
    case C:
        refreshButtons(_C);
        break;
    case CN:
        refreshButtons(_CN);
        break;
    case CF:
        refreshButtons(_CF);
        break;
    case O:
        refreshButtons(_others);
        break;
    }
}

void ShortcutInfo::removeWidgets()
{
    _mapper.clear();
    while(ui->page->layout()->count() > 0) {
        QLayoutItem * item = ui->page->layout()->takeAt(0);
        disconnect(item->widget(), SIGNAL(clicked()), this, SLOT(onWidgetClicked()));
        delete item->widget();
        delete item;
    }
}

void ShortcutInfo::refreshButtons(const QList<ActionKeyInfo*> &infos)
{
    removeWidgets();
    foreach(ActionKeyInfo *info, infos) {
        QWidget *widget = newKey(info->shortcut, normalizeLabel(info->text));
        ui->page->layout()->addWidget(widget);
        _mapper.insert(widget, info->name);
    }
}

QString ShortcutInfo::normalizeLabel(const QString &label)
{
    const QString &str = tr(label.toLatin1());
    QString t2 = str ;
    t2.replace("...", "");
    t2.replace("&", "");
    return t2 ;
}

QWidget *ShortcutInfo::newKey(const QString &key, const QString &text)
{
    const QString templateInstance = QString(templateData).replace("$(FKEY)", Utils::escapeHTML(key))
                                     .replace("$(TEXT)", Utils::escapeHTML(text))
                                     .replace("$(COLORTEXT)", _textColor)
                                     .replace("$(FKEYCOLOR)", _textKeyColor);
    QWidget* widget = createWidget(templateInstance);
    widget->setStyleSheet(_themeCSS);
    widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(widget, SIGNAL(clicked()), this, SLOT(onWidgetClicked()));
    return widget ;
}

QWidget* ShortcutInfo::createWidget(const QString &text)
{
    return createLabel(text);
}

QWidget* ShortcutInfo::createLabel(const QString &text)
{
    QLabelWithSignals *widget = new QLabelWithSignals();
    widget->setTextFormat(Qt::RichText);
    widget->setText(text);
    return widget;
}

QWidget* ShortcutInfo::createButton(const QString &text)
{
    QPushButton *widget = new QPushButton();
    widget->setText(text);
    return widget;
}

QString ShortcutInfo::findPropertyText(QDomElement &parentElement)
{
    int nodi = parentElement.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = parentElement.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            if(element.tagName() == "string") {
                return element.text();
            }
        }
    }
    return "" ;
}

void ShortcutInfo::collectAction(QDomElement &actionElement)
{
    ActionKeyInfo *info = new ActionKeyInfo();
    info->name = actionElement.attribute("name", "");
    int nodi = actionElement.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = actionElement.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            if(element.tagName() == "property") {
                const QString propertyName = element.attribute("name", "");
                const QString propertyText = findPropertyText(element);
                if("text" == propertyName) {
                    info->text = propertyText ;
                }
                if("shortcut" == propertyName) {
                    info->shortcut = propertyText ;
                }
            }
        }
    }
    _actions.append(info);
}

bool ShortcutInfo::getAllActions(QDomNode &rootNode)
{
    bool isOk = true ;
    int nodi = rootNode.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = rootNode.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            if(element.tagName() == "action") {
                collectAction(element);
            } else {
                getAllActions(element);
            }
        }
    }//for
    return isOk ;
}//getAllActions()

void ShortcutInfo::loadActions()
{
    bool isOk = false;
    QFile file(":/data/MainWindow.xml");
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            isOk = getAllActions(document);
        }
        file.close();
    }
    qSort(_actions);
    foreach(ActionKeyInfo * info, _actions) {
        if(info->shortcut.startsWith("Ctrl+Shift")) {
            _ctrShift.append(info);
        } else if(info->shortcut.startsWith("Ctrl+Alt")) {
            _ctrAlt.append(info);
        } else if(isCtrlFn(info->shortcut)) {
            _CF.append(info);
        } else if(isCtrlNum(info->shortcut)) {
            _CN.append(info);
        } else if(info->shortcut.startsWith("Ctrl+")) {
            _C.append(info);
        } else if(info->shortcut.startsWith("F")) {
            _F.append(info);
        } else if(!info->shortcut.isEmpty()) {
            _others.append(info);
        }
    }
    Utils::TODO_THIS_RELEASE("metti i nomi per gestire i valida");
    addOther(new ActionKeyInfo("actionCut", "Del", "Delete"));
    addOther(new ActionKeyInfo("actionCut", "Backspace", "Delete"));
    addOther(new ActionKeyInfo("actionAddChildElement", "Ins", "Insert"));
    addOther(new ActionKeyInfo("actionAddChildElement", "i", "Insert"));
    addOther(new ActionKeyInfo("actionAppendChildElement", "a", "Append"));
    addOther(new ActionKeyInfo("", "e", "Edit"));
    addOther(new ActionKeyInfo("actionPasteAndSubstituteText", "t", "Text"));
    addOther(new ActionKeyInfo("actionEditInnerXML", "x", "Inner XML"));
    addOther(new ActionKeyInfo("actionEditInnerBase64Text", "y", "Inner XML Base64"));
    if(!isOk) {
        Utils::error(this, tr("Unable to actions data."));
    }
}

void ShortcutInfo::addOther(ActionKeyInfo *info)
{
    _actions.append(info);
    _others.append(info);
}


bool ShortcutInfo::isCtrlFn(const QString &input)
{
    const QString searchData("Ctrl+F");
    if(input.startsWith(searchData)) {
        if(input.length() > searchData.length()) {
            QChar firstCharAfter = input.at(searchData.length());
            return firstCharAfter.isDigit();
        }
    }
    return false ;
}

bool ShortcutInfo::isCtrlNum(const QString &input)
{
    const QString searchData("Ctrl+");
    if(input.startsWith(searchData)) {
        if(input.length() > searchData.length()) {
            QChar firstCharAfter = input.at(searchData.length());
            return firstCharAfter.isDigit();
        }
    }
    return false ;
}

void ShortcutInfo::onWidgetClicked()
{
    QWidget *widget = qobject_cast<QWidget*>(sender());
    if(NULL != widget) {
        const QString &name = _mapper[widget];
        if(!name.isEmpty()) {
            emit actionRequested(name);
        }
    }
}

void ShortcutInfo::setTarget(QWidget *target)
{
    foreach( QWidget *key, _mapper.keys()) {
        QString name = _mapper[key];
        QAction* action = target->findChild<QAction*>(name);
        if( NULL != action ) {
            key->setEnabled(action->isEnabled());
        }
    }
}
