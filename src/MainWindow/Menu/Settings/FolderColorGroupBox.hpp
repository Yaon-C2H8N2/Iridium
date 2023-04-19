//
// Created by sr-71 on 19/04/2023.
//

#ifndef IRIDIUM_FOLDERCOLORGROUPBOX_HPP
#define IRIDIUM_FOLDERCOLORGROUPBOX_HPP

#include <QGroupBox>
#include <QLayout>
#include <QPushButton>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

class FolderColorButton;

class FolderColorGroupBox : public QGroupBox
{
Q_OBJECT

	QHBoxLayout *m_layout;
	QList<FolderColorButton *> m_buttons;

public:

	explicit FolderColorGroupBox(QWidget *parent = nullptr);

};


class FolderColorButton : public QPushButton
{
	QColor m_color, m_colorDarker, m_colorNormal;
	bool m_checked = false;

public:
	explicit FolderColorButton(const QColor &color, QWidget *parent = nullptr);

	void check(bool checked)
	{
		m_checked = checked;
		update();
	}

protected:
	void paintEvent(QPaintEvent *event) override;

	void mousePressEvent(QMouseEvent *event) override;

	void mouseReleaseEvent(QMouseEvent *event) override;
};


#endif //IRIDIUM_FOLDERCOLORGROUPBOX_HPP