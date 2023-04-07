//
// Created by sr-71 on 30/03/2023.
//

#include "FileViewWidget.hpp"

FileViewWidget::FileViewWidget(const RemoteInfo &remoteInfo, QWidget *parent) : QWidget(parent)
{
	m_layout = new QHBoxLayout(this);
	setContentsMargins(0, 0, 0, 0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_treeFileView1 = new TreeFileViewContainer(std::make_shared<RemoteInfo>(remoteInfo), this);
	m_layout->addWidget(m_treeFileView1);

	m_treeFileView2 = new TreeFileViewContainer(std::make_shared<RemoteInfo>(RemoteInfo()), this);
	m_layout->addWidget(m_treeFileView2);

	changeRemote(std::make_shared<RemoteInfo>(RemoteInfo("/", RemoteType::LocalHardDrive)));

	connect(m_treeFileView1->treeFileView(), &TreeFileView::fileCopied, this, [this](const QList<TreeFileItem *> &lst)
	{ m_currentFileList = lst; });
	connect(m_treeFileView2->treeFileView(), &TreeFileView::fileCopied, this, [this](const QList<TreeFileItem *> &lst)
	{ m_currentFileList = lst; });
	connect(m_treeFileView1->treeFileView(), &TreeFileView::pasted, this, [this](const RcloneFilePtr &file)
	{
		if (m_currentFileList.empty())
			return;
		QList<RcloneFilePtr> lst;
		for (auto item: m_currentFileList)
			lst << item->getFile();
		copyTo({lst, file}, m_treeFileView1);
	});
	connect(m_treeFileView2->treeFileView(), &TreeFileView::pasted, this, [this](const RcloneFilePtr &file)
	{
		if (m_currentFileList.empty())
			return;
		QList<RcloneFilePtr> lst;
		for (auto item: m_currentFileList)
			lst << item->getFile();
		copyTo({lst, file}, m_treeFileView2);
	});
}

void FileViewWidget::changeRemote(const RemoteInfoPtr &remoteInfo)
{
	if (m_currentView == 0)
	{
		m_treeFileView2->changeRemote(remoteInfo);
		m_currentView = 1;
	} else
	{
		m_treeFileView1->changeRemote(remoteInfo);
		m_currentView = 0;
	}
}

void FileViewWidget::copyTo(const QPair<QList<RcloneFilePtr>, RcloneFilePtr> &pair, TreeFileViewContainer *view)
{
	for (const auto &file: pair.first)
	{
		auto progress = new QProgressBar;
		progress->setRange(0, 100);
		progress->setValue(0);
		auto rclone = m_rclonesManager.get();
		connect(rclone.get(), &Rclone::copyProgress, this, [this, rclone, view, progress](const int p)
		{
			progress->setValue(p);
			if (p == 100)
			{
				view->treeFileView()->paste(m_currentFileList);
				m_rclonesManager.release(rclone);
			}
		});
		rclone->copyTo(file.operator*(), pair.second.operator*());
		emit progressBarCreated(progress);
	}
}
