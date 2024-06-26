//
// Created by Rahman on 29/03/2023.
//

#pragma once

#include <QScrollArea>
#include <QLayout>
#include <QList>
#include <QEvent>

#include "RemoteWidget.hpp"
#include <Remote.h>
#include <RoundedButton.hpp>
#include <RoundedLineEdit.hpp>
#include <boost/asio/io_service.hpp>

struct remotes_selected
{
    RemoteWidget *first{};
    RemoteWidget *second{};

    void clear()
    {
        first = nullptr;
        second = nullptr;
    }

    void swap()
    {
        std::swap(first, second);
    }
    void remove(RemoteWidget *remote)
    {
        if (first == remote)
            first = nullptr;
        if (second == remote)
            second = nullptr;
    }
};

class ListRemoteWidget final : public QScrollArea
{
Q_OBJECT

private:
    QVBoxLayout *_layout{}, *_remote_layout{};

    RoundedLineEdit *_recherche{};
    RoundedButton *_add{};

    QList<RemoteWidget *> _remotes{};

    static std::shared_ptr<remotes_selected> _remoteselected;

    QPushButton *_expand{};

    bool _selected{true};


    int _width{};
    bool _is_expand = true;


public:
    explicit ListRemoteWidget(QWidget *parent = nullptr);

    void expand();

    const std::shared_ptr<remotes_selected> &remoteSelected() { return _remoteselected; }

private:
    void getAllRemote();

    void addRemote(RemoteWidget *remote);

    void removeRemote(RemoteWidget *remote);

    void searchRemote(const QString &name);

    void showAnimation(QWidget *widget) const;

    void hideAnimation(QWidget *widget) const;

protected:
    bool event(QEvent *event) override;

signals:

    void remoteClicked(const std::shared_ptr<remotes_selected> &);
};

class RefreshRemoteEvent : public QEvent
{
public:
    static const QEvent::Type refreshRemoteType = static_cast<QEvent::Type>(300);
};
