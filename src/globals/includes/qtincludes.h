/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2016 by Luca Bellonda and individual contributors  *
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


#ifndef QTINCLUDES_H
#define QTINCLUDES_H

// this file is for include qt globals

//---core
#include <QtCore/QVariant>
#include <QSet>
#include <QList>
//---------- ui

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSpacerItem>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QProgressDialog>
#include <QScrollBar>
#include <QMimeData>
#include <QDialog>
#include <QLabel>
#include <QFileDialog>
#include <QLineEdit>
#include <QCloseEvent>
#include <QTimer>
#include <QThreadPool>
#include <QFontDialog>
#include <QToolButton>
#include <QInputDialog>
#include <QAction>
#include <QFont>
#include <QFontMetrics>
#include <QUndoGroup>
#include <QTableWidget>
#include <QUndoStack>
#include <QToolTip>
#include <QSpinBox>
#include <QRadioButton>
#include <QGroupBox>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtConcurrent/QtConcurrentRun>
#include <QtWidgets/QApplication>
#include <QtPrintSupport/QPrinter>
#else
#include <QtConcurrentRun>
#include <QApplication>
#include <QMenuItem>
#include <QPrinter>
#endif

#include <QXmlStreamReader>
#include <QDomDocument>
#include <QFile>
#include <QtGui>
#include <QClipboard>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

//------------------------

#endif // QTINCLUDES_H
