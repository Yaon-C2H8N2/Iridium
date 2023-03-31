//
// Created by rahman on 05/02/23.
//

#ifndef IRIDIUM_RCLONEFILEMODELDISTANT_HPP
#define IRIDIUM_RCLONEFILEMODELDISTANT_HPP

#include "RcloneFileModel.hpp"
#include <QTimer>

class RcloneFileModelDistant : public RcloneFileModel
{
Q_OBJECT

	RclonesManager manager{std::thread::hardware_concurrency() * 10};

public:
	enum Load
	{
		Static, Dynamic
	};
private:
	Load load{};

	QTimer m_timer{};
	int cpt{};
	QStandardItem *m_itemLoading{};

public:
	explicit RcloneFileModelDistant(const RemoteInfo &remoteInfo, Load = Static, QObject *parent = nullptr);

protected:
	void init() override;

	void addItem(const RcloneFilePtr &file, TreeFileItem *parent) override;

private:
	void addItemDynamic(const QString &path, TreeFileItem *parent);


	void initStatic(const QString &path, TreeFileItem *parent);

	void loading();
};


#endif //IRIDIUM_RCLONEFILEMODELDISTANT_HPP
