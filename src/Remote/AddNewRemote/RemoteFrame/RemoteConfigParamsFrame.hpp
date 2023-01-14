//
// Created by rahman on 13/01/23.
//

#ifndef IRIDIUM_REMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_REMOTECONFIGPARAMSFRAME_HPP

#include <QFrame>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include "../../../Rclone/Rclone.hpp"

class RemoteConfigParamsFrame : public QFrame
{
Q_OBJECT
protected:
    QPushButton *logInBtn{};
    QVBoxLayout *layout{};
    QLineEdit *remoteName{};
    Rclone *rclone{};
public:
    explicit RemoteConfigParamsFrame(QWidget * parent =  nullptr);

protected:
     virtual void addRemote() =0 ;

     virtual void createUi();

};


#endif //IRIDIUM_REMOTECONFIGPARAMSFRAME_HPP
