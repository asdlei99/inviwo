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
#include <inviwo/qt/editor/workspacetreemodel.h>

#include <warn/push>
#include <warn/ignore/all>

#include <QtGlobal>
#include <QTreeView>
#include <QIcon>

#include <warn/pop>

class QSortFilterProxyModel;

namespace inviwo {

class InviwoApplication;
class TreeItem;
class WorkspaceTreeModel;

class IVW_QTEDITOR_API WorkspaceTreeView : public QTreeView {
#include <warn/push>
#include <warn/ignore/all>
    Q_OBJECT
#include <warn/pop>
public:
    explicit WorkspaceTreeView(WorkspaceTreeModel* model,
                               QSortFilterProxyModel* workspaceProxyModel,
                               QItemSelectionModel* selectionModel, QWidget* parent = nullptr);
    virtual ~WorkspaceTreeView() = default;

    bool selectRecentWorkspace(int index);
    /**
     * \brief expand recent workspaces and examples, but not test workspaces
     */
    void defaultExpand();
    /**
     * \brief expand all items depending on whether there is a filter enabled. If not, this will
     * call defaultExpand().
     *
     * \see defaultExpand
     */
    void expandItems();

signals:
    void selectedFileChanged(const QString& filename, bool isExample);
    void loadFile(const QString& filename, bool isExample);

protected:
#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
    // QTreeView::expandRecursively() was introduced in Qt 5.13
    // see https://doc.qt.io/qt-5/qtreeview.html#expandRecursively
    void expandRecursively(const QModelIndex& index);
#endif
private:
    void updateWorkspaces(TreeItem* currentWorkspaceItem, TreeItem* newWorkspaceItem);

    WorkspaceTreeModel* model_;
    QSortFilterProxyModel* proxyModel_;

    TreeItem* recentWorkspaceItem_ = nullptr;
    TreeItem* examplesItem_ = nullptr;
    TreeItem* regressionTestsItem_ = nullptr;
};

}  // namespace inviwo
