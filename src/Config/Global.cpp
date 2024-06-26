//
// Created by sr-71 on 07/05/2023.
//

#include "Global.hpp"
#include <thread>

using namespace std;
using namespace Iridium;


vector<RcloneFilePtr> Global::copy_files{};
vector<RcloneFilePtr> Global::sync_dirs{};
vector<RemoteInfoPtr> Global::remotes{};
map<RemoteInfoPtr, any> Global::remote_model{};
boost::signals2::signal<void(std::any)> Global::signal_add_info;
boost::signals2::signal<void(std::any)> Global::signal_remove_info;

uint8_t Global::max_process = thread::hardware_concurrency();
string Global::path_rclone;
Load Global::load_type = Load::Dynamic;
uint8_t Global::max_depth = 2;
uint8_t Global::reload_time = 10;
ir::process_pool Global::process_pool{max_process};
void Global::add_process(ir::process_uptr&& process,ir::process_pool::priority priority) { process_pool.add_process(std::move(process),priority); }
