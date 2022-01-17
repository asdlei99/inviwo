/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2021 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/
#pragma once

#include <inviwo/qt/editor/inviwoqteditordefine.h>

#include <warn/push>
#include <warn/ignore/all>
#include <QModelIndex>
#include <QSplitter>
#include <QStringList>
#include <QTextEdit>
#include <warn/pop>

class QTabWidget;
class QToolButton;
class QLineEdit;
class QTextEdit;
class QScrollArea;
class QSortFilterProxyModel;
class QItemSelectionModel;

namespace inviwo {

class WorkspaceTreeModel;
class WorkspaceTreeView;
class InviwoApplication;
class ChangeLog;
class WorkspaceGridView;

class IVW_QTEDITOR_API WelcomeWidget : public QSplitter {
#include <warn/push>
#include <warn/ignore/all>
    Q_OBJECT
#include <warn/pop>
public:
    WelcomeWidget(InviwoApplication* app, QWidget* parent);
    virtual ~WelcomeWidget() = default;

    void updateRecentWorkspaces(const QStringList& list);
    void enableRestoreButton(bool hasRestoreWorkspace);
    void setFilterFocus();

signals:
    void loadWorkspace(const QString& filename, bool isExample);
    void newWorkspace();
    void openWorkspace();
    void restoreWorkspace();

protected:
    virtual void showEvent(QShowEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

private:
    void updateDetails(const QString& filename);
    QModelIndex findFirstLeaf(QModelIndex parent = QModelIndex()) const;

    InviwoApplication* app_;

    WorkspaceTreeModel* workspaceModel_;
    QSortFilterProxyModel* workspaceProxyModel_;
    QItemSelectionModel* workspaceSelectionModel_;
    WorkspaceTreeView* workspaceTreeView_;

    WorkspaceGridView* workspaceGridView_;
    QScrollArea* workspaceGridViewArea_;
    QLineEdit* filterLineEdit_;
    QTextEdit* details_;
    ChangeLog* changelog_;
    QToolButton* loadWorkspaceBtn_;
    QToolButton* restoreButton_;
};

}  // namespace inviwo
