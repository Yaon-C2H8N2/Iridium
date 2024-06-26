//
// Created by Rahman on 05/04/2023.
//

#pragma once

#include <QSettings>
#include <iridium/options.hpp>
#include <QApplication>
#include <RcloneFileModelDistant.hpp>
#include <Remote.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <Global.hpp>


class Settings
{

public:

    enum ThemeColor
    {
        Default,
        Green,
        Gray,
        Orange,
        Pink,
        Purple,
        Red,
        Yellow,
    };

    enum Node
    {
        All,
        General,
        Rclone,
        RclonePath,
        Theme,
        MaxProcess,
        LoadType,
        DirIconColor,
        Remotes,
        MaxDepth,
        Flags,
        Language,
        Width,
        Height,
        ReloadTime,
        TempFiles
    };

    enum ProcessOptions
    {
        Stats,
        Transfers,
    };

    enum Os
    {
        Windows,
        Linux,
        MacOs,
        UnknownSystem,
    };

    enum Arch
    {
        X86,
        x86_64,
        Arm64,
        UnknownArch,
    };
    struct System
    {
        Os os;
        Arch arch;
    };

    static const std::map<std::string, QLocale::Language> languages;

private:
    static boost::property_tree::ptree _settings;

    static const std::map<Node, std::string> _nodes;

    static const std::map<ProcessOptions, std::string> _nodes_options;

    static std::map<ProcessOptions,iro::basic_opt_uptr> _options_process;

    static boost::property_tree::ptree _default;

    static void loadSettings();

    static boost::filesystem::path getPathSettings();

    static void initSettings();
    static QIcon DIR_ICON;

    static QIcon HARDDRIVE_ICON;

    static std::vector<RemoteInfoPtr> getLocalRemotes();

public:

    static boost::signals2::signal<void()> list_remote_changed;

    static void init();

    static void initRlclone(std::function<void(bool)> && rclone_init_ok = nullptr);

    auto static rcloneBaseName() -> QString;

    static void changeDirIcon(const ThemeColor &color);

    static void saveSettings();

    static QIcon dirIcon() { return DIR_ICON; }

    static QIcon hardDriveIcon() { return HARDDRIVE_ICON; }

    static void refreshRemotesList();

    static void addLocalRemote(const RemoteInfo &remoteInfo);

    static void deleteRemote(const RemoteInfoPtr &remoteInfo);

    static void resetSettings(const Node &);

    static void loadValues();

    static void setProcessOptions(const ProcessOptions &option, iro::basic_opt_uptr &&value);

    static void setProcessOptions(const ProcessOptions &option, const iro::basic_option &value);

    static QFile saveFileToTemp(const QByteArray &data, const QString &name);

    static void deleteAllTempFiles();

    static iro::basic_option getProcessOptions(const ProcessOptions &option);

    /**
     * @brief set the value of a node and save the settings
     * @tparam Args
     */
    template<class ... Args>
    requires (... && std::is_same_v<std::pair<Node, typename Args::second_type>, Args>)
    static void setValue(Args &&... args)
    {
        for (auto &&arg : {args...})
        {
            try
            {
                _settings.put(_nodes.at(arg.first), arg.second);
            } catch (boost::exception &e)
            {
                std::cout << "eror set Value" << diagnostic_information_what(e, true) << std::endl;
            }
        }
        saveSettings();
    }

    /**
     * @brief get the value of a node
     * @tparam Type
     * @param node
     * @return
     */
    template<class Type>
    static Type getValue(const Node &node)
    {
        return _settings.get_child(_nodes.at(node)).get_value<Type>();
    }

    static void setLanguage(const QLocale::Language &);

    static System getSystem();

};