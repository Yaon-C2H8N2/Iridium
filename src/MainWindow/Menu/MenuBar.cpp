//
// Created by sr-71 on 14/04/2023.
//

#include "MenuBar.hpp"
#include <QMessageBox>
#include <QFormLayout>
#include <QLabel>
#include <Rclone.hpp>
#include <Config.h>
#include <Settings.hpp>
#include <boost/version.hpp>

MenuBar::MenuBar(QWidget *parent) : QMenuBar(parent)
{
	m_aboutMenu = addMenu("About");
	m_aboutAction = m_aboutMenu->addAction("About");

	m_settingsMenu = addMenu("Settings");
	m_settingsAction = m_settingsMenu->addAction("Settings");

	connectSignals();
}

void MenuBar::connectSignals()
{
	connect(m_aboutAction, &QAction::triggered, this, [this]()
	{
		auto dialog = QDialog(this);
		dialog.setWindowTitle(tr("A propos d'Iridium"));
		dialog.setModal(true);

		auto formLayout = QFormLayout(&dialog);
		formLayout.setContentsMargins(30, 30, 30, 30);
		formLayout.setSizeConstraint(QLayout::SetFixedSize);
		formLayout.setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		formLayout.setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);

		// add icon
		auto icon = QLabel();
		icon.setPixmap(QPixmap(":/ressources/Iridium.png").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		icon.setAlignment(Qt::AlignCenter);
		formLayout.addRow(&icon);

		auto version = QLabel(
			"<a href=\"https://github.com/Sudo-Rahman/Iridium/releases/tag/v" + QString::fromStdString(IRIDIUM_VERSION) +
			"\">" + QString::fromStdString(IRIDIUM_VERSION) + "</a>");
		version.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Iridium version : ")), &version);

		auto qtVersion = QLabel("<a href=\"https://www.qt.io/\">" + QString::fromStdString(QT_VERSION_STR) + "</a>");
		qtVersion.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Qt Version : ")), &qtVersion);

		auto boostVersion = QLabel("<a href=\"https://www.boostVersion.org/\">" + QString::fromStdString(BOOST_LIB_VERSION) + "</a>");
		boostVersion.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Boost Version : ")), &boostVersion);

		auto rcloneVersion = QLabel("<a href=\"https://rclone.org/\">" + QString::fromStdString(Rclone().version()) + "</a>");
		rcloneVersion.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Rclone Version : ")), &rcloneVersion);


		auto auteur = QLabel("<a href=\"https://github.com/Sudo-Rahman\">" + QString::fromStdString(AUTHOR) + "</a>");
		auteur.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Auteur : ")), &auteur);

		dialog.exec();
	});

	connect(m_settingsAction, &QAction::triggered, this, [this]()
	{});
}