//
// Created by rahman on 13/01/23.
//

#pragma once

#include <QFrame>
#include <QPushButton>
#include <QLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <iridium/process/process.hpp>
#include <iridium/process/config_create.hpp>

#include <RoundedLineEdit.hpp>

#include "Remote.h"

class RemoteConfigParamsFrame : public QFrame
{
Q_OBJECT

protected:
    QPushButton *_login{}, *_cancel{};
    QLabel *_mess_label{};
    QVBoxLayout *_layout{};
    QFormLayout *_form_layout{};
    RoundedLineEdit *_remote_name{};
    std::unique_ptr<iridium::rclone::process> _process{};
    std::vector<RemoteInfoPtr> _remotes{};

public:
    explicit RemoteConfigParamsFrame(QWidget *parent = nullptr);

    virtual void reset();

    void focusLineEdit() { if (_remote_name)_remote_name->setFocus(); }

    const char *noCheck = "noCheck";

protected:
    virtual void addRemote();

    virtual void createUi();

    virtual bool checkFields();

    void clearAllFields();


signals:

    void remoteAdded();

};