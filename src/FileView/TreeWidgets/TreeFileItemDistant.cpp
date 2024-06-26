//
// Created by Rahman on 31/03/2023.
//

#include "TreeFileItemDistant.hpp"

TreeFileItemDistant::TreeFileItemDistant(const RcloneFile& file, TreeFileItem * parent)
{
	_file = std::make_shared<RcloneFile>(file);
	setText(_file->getName());
	QStandardItem::setData(_file->getName(), SORT_ROLE);
	initIcon();
	if (_file->isDir())
	{}
	else
		setFlags(flags() & ~Qt::ItemIsDropEnabled);
}
