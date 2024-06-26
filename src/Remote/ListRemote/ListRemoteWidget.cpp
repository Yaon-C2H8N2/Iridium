//
// Created by Rahman on 29/03/2023.
//

#include <QPropertyAnimation>
#include <QMessageBox>
#include "ListRemoteWidget.hpp"
#include <AddNewRemoteDialog.hpp>
#include <Settings.hpp>
#include <QPalette>
#include <QThread>
#include <algorithm>
#include <QGraphicsOpacityEffect>

std::shared_ptr<remotes_selected> ListRemoteWidget::_remoteselected = std::make_shared<remotes_selected>();

/**
 * @brief constructeur
 * @param parent
 */
ListRemoteWidget::ListRemoteWidget(QWidget * parent) : QScrollArea(parent)
{
	// background transparent
	auto pal = this->palette();
	pal.setColor(QPalette::Window, Qt::transparent);
	setPalette(pal);
	setWidgetResizable(true);
	auto * widget = new QWidget(this);
	setWidget(widget);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	_layout = new QVBoxLayout(widget);
	_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	_layout->setSpacing(10);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

	_layout->setContentsMargins(10, 10, 15, 10);

	_expand = new QPushButton(this);
	_expand->setCheckable(true);
	_expand->setChecked(true);
	_expand->setIcon(Settings::hardDriveIcon());
	_expand->setFixedWidth(35);
	// rounded button
	_layout->addWidget(_expand);

	connect(_expand, &QPushButton::clicked, this, [this]() { expand(); });

	auto * toplayout = new QHBoxLayout;

	_add = new RoundedButton("＋");
	_add->setFixedSize(35, 35);
	_add->setCircular(true);
	connect(_add, &QPushButton::clicked, this, [this]()
	{
		auto * addRemote = new AddNewRemoteDialog(this);
		connect(addRemote, &AddNewRemoteDialog::newRemoteAdded, this, [this]()
		{
			Settings::refreshRemotesList();
			emit remoteClicked(_remoteselected);
		});
		addRemote->setModal(true);
		addRemote->show();
	});
	toplayout->addWidget(_add);

	_recherche = new RoundedLineEdit(this);
	_recherche->setPlaceholderText(tr("Rechercher"));
	connect(_recherche, &QLineEdit::textChanged, this, &ListRemoteWidget::searchRemote);
	toplayout->addWidget(_recherche);

	_layout->addLayout(toplayout);


	_remote_layout = new QVBoxLayout;
	_layout->addLayout(_remote_layout);

	_remoteselected = std::make_shared<remotes_selected>();

	Settings::list_remote_changed.connect(
		[this] { qApp->postEvent(this, new QEvent(RefreshRemoteEvent::refreshRemoteType), Qt::HighEventPriority); });

	// no border
	setFrameShape(QFrame::NoFrame);

	_width = QScrollArea::sizeHint().width();

	getAllRemote();
}

/**
 * @brief Récupère tous les remotes
 */
void ListRemoteWidget::getAllRemote()
{
	//     creation des remoteWidget
	const auto remotes = &Iridium::Global::remotes;

	// add new remote widget if not exist in _remotes but exist in remotes
	for (const auto &remote: *remotes)
	{
		if (std::ranges::none_of(_remotes.begin(), _remotes.end(), [&remote](const RemoteWidget * remoteWidget)
		{
			return *remoteWidget->remoteInfo() == *remote;
		})) { addRemote(new RemoteWidget(remote)); }
	}

	// remove widget if remote not exist in remotes
	for (auto *remote: _remotes)
	{
		if (std::ranges::none_of(remotes->begin(), remotes->end(), [remote](const RemoteInfoPtr& remoteInfo)
		{
			return *remoteInfo == *remote->remoteInfo();
		})) { removeRemote(remote); }
	}
	emit remoteClicked(_remoteselected);
}

void ListRemoteWidget::addRemote(RemoteWidget * remote)
{
	_remotes.push_back(remote);
	connect(remote, &RemoteWidget::clicked, this, [this](RemoteWidget * remoteWidget)
	{
		for (auto * remote: _remotes)
			remote->setSelection(RemoteWidget::None);
		if (_selected)
			_remoteselected->first = remoteWidget;
		else
			_remoteselected->second = remoteWidget;
		if (_remoteselected->first == _remoteselected->second)
			_remoteselected->first->setSelection(RemoteWidget::FirstSecond);
		else
		{
			if (_remoteselected->first not_eq nullptr)
				_remoteselected->first->setSelection(RemoteWidget::First);
			if (_remoteselected->second not_eq nullptr)
				_remoteselected->second->setSelection(RemoteWidget::Second);
		}
		_selected = !_selected;
		emit remoteClicked(_remoteselected);
	});

	connect(remote, &RemoteWidget::deleted, this, [this](auto * remoteWidget) { Settings::refreshRemotesList(); });

	_remote_layout->addWidget(remote);
}

void ListRemoteWidget::removeRemote(RemoteWidget * remote)
{
	_remoteselected->remove(remote);
	_remotes.erase(std::ranges::find(_remotes, remote));
	_remote_layout->removeWidget(remote);
	delete remote;
}

/**
 * @brief Recherche un remote en fonction de son nom
 * @param name
 */
void ListRemoteWidget::searchRemote(const QString& name)
{
	for (auto * remote: _remotes)
	{
		if (QString::fromStdString(remote->remoteInfo()->name()).contains(name, Qt::CaseInsensitive))
			showAnimation(remote);
		else
			hideAnimation(remote);
	}
}

/**
 * @brief expand animation with expand button clicked
 */
void ListRemoteWidget::expand()
{
	auto animation = new QPropertyAnimation(this, "maximumWidth");
	animation->setDuration(500);
	animation->setStartValue(width());
	animation->setEndValue(_is_expand ? 45 : _width);
	animation->setEasingCurve(QEasingCurve::InOutQuad);
	animation->start(QAbstractAnimation::DeleteWhenStopped);
	_is_expand = !_is_expand;

	if (_is_expand)
	{
		_add->show();
		for (auto wid: _remotes)
			showAnimation(wid);
		showAnimation(_recherche);
	}
	else
	{
		for (auto wid: _remotes)
			hideAnimation(wid);
		hideAnimation(_recherche);
	}

	auto show_effect = new QGraphicsOpacityEffect(_add);
	animation = new QPropertyAnimation(show_effect, "opacity");
	connect(animation, &QPropertyAnimation::destroyed, this, [this]
	{
		if (!_is_expand)
			_add->hide();
	});
	_add->setGraphicsEffect(show_effect);
	animation->setStartValue(_is_expand ? 0 : 1);
	animation->setEndValue(_is_expand ? 1 : 0);
	animation->setDuration(500);
	animation->setEasingCurve(QEasingCurve::BezierSpline);
	animation->start(QAbstractAnimation::DeleteWhenStopped);
}

/**
 * @brief set animation for show widget in parameter
 * @param widget
 */
void ListRemoteWidget::showAnimation(QWidget * widget) const
{
	auto animation = new QPropertyAnimation(widget, "maximumWidth");
	animation->setDuration(300);
	animation->setStartValue(0);
	animation->setEndValue(_width);
	animation->setEasingCurve(QEasingCurve::InOutQuad);
	animation->start(QAbstractAnimation::DeleteWhenStopped);
	connect(animation, &QPropertyAnimation::finished, widget, [widget]() { widget->show(); });
}

/**
 * @brief set animation for hide widget in parameter
 * @param widget
 */
void ListRemoteWidget::hideAnimation(QWidget * widget) const
{
	auto animation = new QPropertyAnimation(widget, "maximumWidth");
	animation->setDuration(300);
	animation->setStartValue(widget->width());
	animation->setEndValue(0);
	animation->setEasingCurve(QEasingCurve::InOutQuad);
	animation->start(QAbstractAnimation::DeleteWhenStopped);
	connect(animation, &QPropertyAnimation::finished, widget, [widget]() { widget->hide(); });
}

bool ListRemoteWidget::event(QEvent * event)
{
	if (event->type() == RefreshRemoteEvent::refreshRemoteType) { getAllRemote(); }

	return QScrollArea::event(event);
}
