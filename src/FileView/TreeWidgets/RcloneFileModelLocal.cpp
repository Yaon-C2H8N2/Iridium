//
// Created by sr-71 on 30/03/2023.
//

#include "RcloneFileModelLocal.hpp"
#include "TreeFileItemLocal.hpp"
#include <Settings.hpp>
#include <Utility/Utility.hpp>


RcloneFileModelLocal::RcloneFileModelLocal(const RemoteInfoPtr &remoteInfo, QObject *parent) : RcloneFileModel(
	remoteInfo, parent)
{
	RcloneFileModelLocal::init();
}

void RcloneFileModelLocal::addItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
	if (m_thread not_eq nullptr)
	{
		m_thread->detach();
		Iridium::Utility::KillThread(m_thread->native_handle());
	}
	auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
	m_thread = boost::make_shared<boost::thread>(

//	m_thread = std::make_shared<std::thread>(
		[this, tree_item, file]()
		{
			if (tree_item->rowCount() == 1)
			{
				auto list_file = QDir(file->getPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
				for (const QFileInfo &info: list_file)
				{
					auto *item = new TreeFileItemLocal(info.filePath(), m_remoteInfo);
					tree_item->appendRow(getItemList(item));
					if (info.isDir())
						item->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
				}
				delete tree_item->child(0, 0);
				tree_item->removeRow(0);
			}
		});
}

void RcloneFileModelLocal::init()
{
	auto *drive = new TreeFileItem(QString::fromStdString(m_remoteInfo->m_path), m_remoteInfo);
	drive->setIcon(Settings::HARDDRIVE_ICON);
	m_root_index = drive->index();
	drive->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
	appendRow({
				  drive,
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem(tr("Disque"), drive->getFile(), drive)
			  });
}
