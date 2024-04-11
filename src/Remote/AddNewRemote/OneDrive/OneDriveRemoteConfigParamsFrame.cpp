//
// Created by Rahman on 14/04/2023.
//

#include "OneDriveRemoteConfigParamsFrame.hpp"

OneDriveRemoteConfigParamsFrame::OneDriveRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
    RemoteConfigParamsFrame::createUi();
}

void OneDriveRemoteConfigParamsFrame::addRemote()
{

    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;

    using iridium::rclone::entity;
    iridium::rclone::process().config_create().name(_remote_name->text().toStdString())
            .type(ire::remote::remote_type_to_string(ire::remote::onedrive))
            .execute();

}


void OneDriveRemoteConfigParamsFrame::createUi()
{
    RemoteConfigParamsFrame::createUi();
}

bool OneDriveRemoteConfigParamsFrame::checkFields()
{
    return RemoteConfigParamsFrame::checkFields();
}
