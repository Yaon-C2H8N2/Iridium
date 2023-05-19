//
// Created by rahman on 05/02/23.
//

#include <QProgressBar>
#include "RcloneFileModelDistant.hpp"
#include "TreeFileItemDistant.hpp"

uint8_t RcloneFileModelDistant::m_max_depth = 2;
RcloneFileModelDistant::Load RcloneFileModelDistant::m_load = Dynamic;

RcloneFileModelDistant::RcloneFileModelDistant(const RemoteInfoPtr &remoteInfo, QTreeView *View)
        : RcloneFileModel(remoteInfo, View)
{
    RcloneFileModelDistant::init();
}

void RcloneFileModelDistant::init()
{
    auto *drive = new TreeFileItem(m_remote_info->m_path.c_str(), m_remote_info);
    drive->getFile()->setSize(0);
    drive->setIcon(QIcon(m_remote_info->m_icon.c_str()));
    m_root_index = drive->index();
    drive->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
    appendRow({
                      drive,
                      new TreeFileItem(1, drive->getFile(), drive),
                      new TreeFileItem(2, drive->getFile(), drive),
                      new TreeFileItem(3, drive->getFile(), drive),
              });
}

void RcloneFileModelDistant::addItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
    RcloneFileModel::addItem(file, parent);
    if (m_load == Dynamic)
        addItemDynamic(file->getPath(), parent);
    else
        addItemStatic(file->getPath(), parent);
}

void RcloneFileModelDistant::addItemDynamic(const QString &path, TreeFileItem *parent)
{
    auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
    auto rclone = RcloneManager::get();
    if (tree_item->state() == TreeFileItem::NotLoaded)
    {
        addProgressBar(tree_item->child(0, 0)->index());
        tree_item->setState(TreeFileItem::Loading);
        connect(rclone.get(), &Rclone::lsJsonFinished, this, [tree_item, path, this](const boost::json::array &array)
        {
            tree_item->removeRow(0);
            for (const auto &file: array)
            {
                auto *item = new TreeFileItemDistant(path, m_remote_info, file.as_object());
                tree_item->appendRow(getItemList(item));
            }
            tree_item->setState(TreeFileItem::Loaded);
        });
        rclone->lsJson(path.toStdString());
    }
}

void RcloneFileModelDistant::addItemStatic(const QString &path, TreeFileItem *parent, uint8_t depth)
{
    if (depth == 0)
        return;
    if (depth == m_max_depth)
    {
        for (auto &rclone: m_locked_rclone) { rclone->cancel(); }
        m_locked_rclone.clear();
    }

    auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
    if (tree_item->state() == TreeFileItem::NotLoaded)
    {
        auto rclone = RcloneManager::get(true);
        addProgressBar(tree_item->child(0, 0)->index());
        connect(rclone.get(), &Rclone::lsJsonFinished, this,
                [depth, tree_item, rclone, path, this](const boost::json::array &array)
                {
                    tree_item->removeRow(0);
                    for (const auto &file: array)
                    {
                        auto *item = new TreeFileItemDistant(path, m_remote_info, file.as_object());
                        tree_item->appendRow(getItemList(item));
                        if (item->getFile()->isDir() and not rclone->isCanceled())
                            addItemStatic(item->getFile()->getPath(), item, depth - 1);
                    }
                    tree_item->setState(TreeFileItem::Loaded);
                });
        connect(rclone.get(), &Rclone::started, this,
                [tree_item] { tree_item->setState(TreeFileItem::Loading); });

        connect(rclone.get(), &Rclone::finished, this, [rclone, this]
        {
//            remove rclone from m_locked_rclone
            m_locked_rclone.erase(std::remove(m_locked_rclone.begin(), m_locked_rclone.end(), rclone),
                                  m_locked_rclone.end());
            rclone->disconnect();
        });
        rclone->lsJson(path.toStdString());
        m_locked_rclone.push_back(rclone);
        RcloneManager::addLockable(rclone);
    }else{
        for (int i = 0; i < tree_item->rowCount(); ++i) {
            auto *item = dynamic_cast<TreeFileItemDistant*>(tree_item->child(i, 0));
            if (item->getFile()->isDir())
                addItemStatic(item->getFile()->getPath(), item, depth - 1);
        }
    }
}
