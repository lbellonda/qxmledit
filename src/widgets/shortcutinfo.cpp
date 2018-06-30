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
#include <QDesktopWidget>
#include "qxmleditconfig.h"
#include "utils.h"

class ActionKeyInfo
{
public:
    QString name;
    QString shortcut;
    QString text;
    QString normalizedLabel;
    QString enabledText;
    QString disabledText;
    bool alwaysEnabled;

    static bool compareOpLessThan(const ActionKeyInfo *a1, const ActionKeyInfo *a2);

    ActionKeyInfo();
    ActionKeyInfo(const QString &theName, const QString &theShortcut, const QString &theText);
    ~ActionKeyInfo();
};

ActionKeyInfo::ActionKeyInfo()
{
    alwaysEnabled = false;
}

ActionKeyInfo::ActionKeyInfo(const QString &theName, const QString &theShortcut, const QString &theText)
{
    name = theName ;
    shortcut = theShortcut ;
    text = theText ;
    alwaysEnabled = false;
}

ActionKeyInfo::~ActionKeyInfo()
{}

bool ActionKeyInfo::compareOpLessThan(const ActionKeyInfo *a1, const ActionKeyInfo *a2)
{
    return QString::compare(a1->normalizedLabel, a2->normalizedLabel, Qt::CaseInsensitive) < 0 ;
}

//-------------------------------------------------------------------

ShortcutInfo::ShortcutInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShortcutInfo)
{
    _signalHidden = true ;
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

void ShortcutInfo::chooseTheme()
{
    _lightTheme = Config::getBool(Config::KEY_INFO_SHORTCUT_LIGHT_THEME, false);
    if(_lightTheme) {
        _themeCSS = readCSSData(":/css/buttonLight.css");
        _templateTextEnabled = readResourceString(":/css/templateLightEnabled.html");
        _templateTextDisabled = readResourceString(":/css/templateLightDisabled.html");
    } else {
        _themeCSS = readCSSData(":/css/buttonDark.css");
        _templateTextEnabled = readResourceString(":/css/templateDarkEnabled.html");
        _templateTextDisabled = readResourceString(":/css/templateDarkDisabled.html");
    }
    _templateClose = readResourceString(":/css/templateClose.html");
    _templateGreen = readResourceString(":/css/templateGreen.html");
    _templateQuit = readResourceString(":/css/templateQuit.html");
    _cssClose = readResourceString(":/css/closeButton.css");
    _cssGreen = readResourceString(":/css/greenButton.css");
    _cssQuit = readResourceString(":/css/quitButton.css");
}

QString ShortcutInfo::readCSSData(const QString &name)
{
    return readResourceString(name);
}

QString ShortcutInfo::readResourceString(const QString &name)
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

void ShortcutInfo::autoHide()
{
    _signalHidden = false ;
    hide();
    _signalHidden = true ;
}

void ShortcutInfo::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    if(_signalHidden) {
        emit hidden();
    }
}

void ShortcutInfo::setupData()
{
    ui->page->setStyleSheet("background-color: #202020;border: 1px solid white;border-radius:10px;");
    ui->stackedWidget->setStyleSheet("background-color: #202020;border: 1px solid white;border-radius:10px;");
    chooseTheme();
    QStringList labels ;
#ifdef ENVIRONMENT_MACOS
    labels << tr("Function Keys") << tr("Command+Shift") << tr("Command+Alt") << tr("Command+Fn") << tr("Command+Num") << tr("Command") << tr("Others");
#else
    labels << tr("Function Keys") << tr("Ctrl+Shift") << tr("Ctrl+Alt") << tr("Ctrl+Fn") << tr("Ctrl+Num") << tr("Ctrl") << tr("Others");
#endif
    QList<int> codes ;
    codes << F << CS << CA << CF << CN << C << O;
    Utils::loadComboCodedArrays(ui->type, C, labels, codes);
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
    emit typeChanged();
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
    QList<ActionKeyInfo*> sortedActions = sortActions(infos);
    foreach(ActionKeyInfo *info, sortedActions) {
        QWidget *widget ;
        if(info->name == "actionQuit") {
            widget = newKey(info, _cssQuit, _templateQuit, _templateQuit, true);
        } else if(info->name == "actionClose") {
            widget = newKey(info, _cssClose, _templateClose, _templateClose, false);
        } else if(info->name == "actionNew") {
            widget = newKey(info, _cssGreen, _templateGreen, _templateGreen, true);
        } else if(info->name == "actionOpen") {
            widget = newKey(info, _cssGreen, _templateGreen, _templateGreen, true);
        } else if(info->name == "actionNewWindow") {
            widget = newKey(info, _cssGreen, _templateGreen, _templateGreen, true);
        } else {
            widget = newKey(info, _themeCSS, _templateTextEnabled, _templateTextDisabled, false);
        }
        ui->page->layout()->addWidget(widget);
        _mapper.insert(widget, info);
    }
    ui->page->layout()->activate();
    layout()->activate();
    // center window
    if(isVisible()) {
        doResize(false);
    }
}

void ShortcutInfo::doResize(const bool isFirstTime)
{
    QRect screenGeometry = QApplication::desktop()->availableGeometry(QApplication::desktop()->screenNumber(this));
    QRect geom = geometry();
    int geomTop = geom.top();
    if(isFirstTime) {
        geomTop = screenGeometry.height() - height();
    }
    setGeometry(10, geomTop, screenGeometry.width() - 20, geom.height());
}

QList<ActionKeyInfo*> ShortcutInfo::sortActions(const QList<ActionKeyInfo*> &infos)
{
    QList<ActionKeyInfo*> result ;
    foreach(ActionKeyInfo *action, infos) {
        result.append(action);
    }
    qSort(result.begin(), result.end(), ActionKeyInfo::compareOpLessThan);
    return result ;
}

QString ShortcutInfo::normalizeLabel(const QString &label)
{
    const QString &str = tr(label.toLatin1());
    QString t2 = str ;
    t2.replace("...", "");
    t2.replace("&", "");
    return t2 ;
}

QWidget *ShortcutInfo::newKey(ActionKeyInfo *info, const QString &css, const QString &tpl, const QString &tplDis, const bool isAlwaysEnabled)
{
    const QString templateInstance = QString(tpl).replace("$(FKEY)", Utils::escapeHTML(info->shortcut))
                                     .replace("$(TEXT)", Utils::escapeHTML(info->normalizedLabel));
    const QString templateDisabledInstance = QString(tplDis).replace("$(FKEY)", Utils::escapeHTML(info->shortcut))
            .replace("$(TEXT)", Utils::escapeHTML(info->normalizedLabel));
    info->disabledText = templateDisabledInstance ;
    info->enabledText = templateInstance ;
    info->alwaysEnabled = isAlwaysEnabled ;
    QWidget* widget = createWidget(templateInstance);
    widget->setStyleSheet(css);
    widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(widget, SIGNAL(clicked()), this, SLOT(onWidgetClicked()));
    return widget ;
}

void ShortcutInfo::setTemplates(ActionKeyInfo *info, const QString &templateString, const QString &key, const QString &text)
{
    const QString templateInstance = QString(templateString).replace("$(FKEY)", Utils::escapeHTML(key))
                                     .replace("$(TEXT)", Utils::escapeHTML(text));
    const QString templateDisabledInstance = QString(templateString).replace("$(FKEY)", Utils::escapeHTML(key))
            .replace("$(TEXT)", Utils::escapeHTML(text));
    info->disabledText = templateDisabledInstance ;
    info->enabledText = templateInstance ;
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

QString ShortcutInfo::filterCmd(const QString &input, const QString &prefix)
{
    QString result ;
    result = input.mid(prefix.length());
    return result;
}

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
    foreach(ActionKeyInfo * info, _actions) {
        if(info->shortcut.startsWith("Ctrl+Shift")) {
            info->shortcut = filterCmd(info->shortcut, "Ctrl+Shift+");
            _ctrShift.append(info);
        } else if(info->shortcut.startsWith("Ctrl+Alt")) {
            info->shortcut = filterCmd(info->shortcut, "Ctrl+Alt+");
            _ctrAlt.append(info);
        } else if(isCtrlFn(info->shortcut)) {
            info->shortcut = filterCmd(info->shortcut, "Ctrl+");
            _CF.append(info);
        } else if(isCtrlNum(info->shortcut)) {
            info->shortcut = filterCmd(info->shortcut, "Ctrl+");
            _CN.append(info);
        } else if(info->shortcut.startsWith("Ctrl+")) {
            info->shortcut = filterCmd(info->shortcut, "Ctrl+");
            _C.append(info);
        } else if(info->shortcut.startsWith("F")) {
            _F.append(info);
        } else if(!info->shortcut.isEmpty()) {
            _others.append(info);
        }
    }
    addOther(new ActionKeyInfo("actionCut", "Del", "Delete"));
    addOther(new ActionKeyInfo("actionCut", "Backspace", "Delete"));
    addOther(new ActionKeyInfo("actionAddChildElement", "Ins", "Insert"));
    addOther(new ActionKeyInfo("actionAddChildElement", "i", "Insert"));
    addOther(new ActionKeyInfo("actionAppendChildElement", "a", "Append"));
    addOther(new ActionKeyInfo("actionEditAsText", "e", tr("Edit Element as Text")));
    addOther(new ActionKeyInfo("actionPasteAndSubstituteText", "t", "Text"));
    addOther(new ActionKeyInfo("actionEditInnerXML", "x", "Inner XML"));
    addOther(new ActionKeyInfo("actionEditInnerBase64Text", "y", "Inner XML Base64"));
    if(!isOk) {
        Utils::error(this, tr("Unable to actions data."));
    }
    foreach(ActionKeyInfo * info, _actions) {
        info->normalizedLabel = normalizeLabel(info->text);
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
        ActionKeyInfo *info = _mapper[widget];
        if(!info->name.isEmpty()) {
            emit actionRequested(info->name);
        }
    }
}

void ShortcutInfo::setTarget(QWidget *target)
{
    foreach(QWidget *key, _mapper.keys()) {
        ActionKeyInfo *info = _mapper[key];
        if(info->alwaysEnabled) {
            QLabelWithSignals *widget = qobject_cast<QLabelWithSignals*>(key);
            key->setEnabled(true);
            if(NULL != widget) {
                widget->setText(info->enabledText);
            }
        } else {
            QAction* action = target->findChild<QAction*>(info->name);
            if(NULL != action) {
                const bool isEnabled = action->isEnabled() ;
                key->setEnabled(isEnabled);
                QLabelWithSignals *widget = qobject_cast<QLabelWithSignals*>(key);
                if(NULL != widget) {
                    widget->setText(isEnabled ? info->enabledText : info->disabledText);
                }
            }
        }
    }
}
