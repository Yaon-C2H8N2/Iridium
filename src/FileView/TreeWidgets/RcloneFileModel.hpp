//
// Created by Rahman on 02/02/2023.
//

#pragma once

#include <QStandardItemModel>
#include "TreeFileItem.hpp"
#include <QTreeView>
#include <QMimeData>

class TreeMimeData : public QMimeData
{
	Q_OBJECT

	QList<TreeFileItem *> _items{};

public:
	explicit TreeMimeData(const QList<TreeFileItem *> &items) : _items(items) {}

	[[nodiscard]] QList<TreeFileItem *> items() const { return _items; }

	[[nodiscard]] bool hasFormat(const QString &mimeType) const override
	{
		return mimeType == "application/x-qabstractitemmodeldatalist";
	}
};

class RcloneFileModel : public QStandardItemModel
{
	Q_OBJECT

public:
	[[nodiscard]] QMimeData *mimeData(const QModelIndexList &indexes) const override;

	[[nodiscard]] QStringList mimeTypes() const override;

	bool
	dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

protected:
	RemoteInfoPtr _remote_info{};
	QModelIndex _root_index{};

	QTreeView *_view{};

	bool _expand_or_double_click = false;

	bool _check_is_valid = false;

	virtual void init() = 0;

	explicit RcloneFileModel();

	explicit RcloneFileModel(const RemoteInfoPtr &remoteInfo, QTreeView *View);

	void addProgressBar(const QModelIndex &index);

public:
	virtual void addItem(const RcloneFilePtr &file, TreeFileItem *parent);

	virtual void reload(TreeFileItem *item = nullptr) = 0;

	[[nodiscard]] const QModelIndex& getRootIndex() const;

	static QList<QStandardItem *> getItemList(TreeFileItem *item);

	void setExpandOrDoubleClick(bool expandOrDoubleClick) { _expand_or_double_click = expandOrDoubleClick; }

	static bool fileInFolder(const RcloneFilePtr &, const TreeFileItem *folder);

	static bool fileInFolder(const QString &, const TreeFileItem *folder);

	static QList<RcloneFilePtr> filesInFolder(const TreeFileItem *folder);

	TreeFileItem *getTreeFileItem(const RcloneFilePtr &file, TreeFileItem *parent);

	virtual void stop() = 0;

	RemoteInfoPtr getRemoteInfo() const { return _remote_info; }
};
