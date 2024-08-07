//
// Created by Rahman on 12/04/2023.
//

#include "TaskRowChild.hpp"
#include <Utility/Utility.hpp>

using namespace std::chrono;
using namespace boost::variant2;

/**
 * @brief TaskRow::TaskRow
 * @param src
 * @param dest
 * @param data
 */
TaskRowChild::TaskRowChild(const RcloneFile &src, const RcloneFile &dest, const ire::json_log::stats::transfer &data)
{
	_src = src;
	_dest = dest;
	_size = data.size;
	_data = data;
	TaskRowChild::init();
}

void TaskRowChild::updateDataChild()
{
	_size = _data.size;
	at(2)->setText(Iridium::Utility::sizeToString(_size).c_str());

	_progressBar->setProgress(_data.percentage / 100.0);

	if (_data.speed == 0)
		return;
	auto remainingTime = double64_t(_size - _data.bytes) / _data.speed;

	// remainingTime to hh:mm:ss
	auto seconds = (uint64_t)remainingTime % 60;
	auto minutes = ((uint64_t)remainingTime / 60) % 60;
	auto hours = ((uint64_t)remainingTime / 3600) % 24;

	this->at(6)->setText(
		QString("%1:%2:%3")
		.arg(hours, 2, 10, QChar('0'))
		.arg(minutes, 2, 10, QChar('0'))
		.arg(seconds, 2, 10, QChar('0')));

	// elapsed time
	auto now = duration_cast<std::chrono::seconds>(system_clock::now().time_since_epoch()).count();
	auto elapsed = now - _elapsed_time;
	seconds = elapsed % 60;
	minutes = (elapsed / 60) % 60;
	hours = (elapsed / 3600) % 24;
	this->at(7)->setText(QString("%1:%2:%3")
		.arg(hours, 2, 10, QChar('0'))
		.arg(minutes, 2, 10, QChar('0'))
		.arg(seconds, 2, 10, QChar('0')));

	this->at(8)->setText((Iridium::Utility::sizeToString(_data.speed) + "/s").c_str());
	this->at(9)->setText(Iridium::Utility::sizeToString(_data.speed_avg).c_str());
}

void TaskRowChild::updateData(const variant<ire::json_log, ire::json_log::stats::transfer> &variant)
{
	try
	{
		_data = get<ire::json_log::stats::transfer>(variant);
		if (_elapsed_time == 0)
			_elapsed_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
		updateDataChild();
	}
	catch (const bad_variant_access &e) { throw std::runtime_error("TaskRowParent::updateData: bad_variant_access"); }
}

/**
 * @brief if the task is finished
 */
void TaskRowChild::finished()
{
	_size = _data.size;

	at(2)->setText(Iridium::Utility::sizeToString(_size).c_str());

	this->at(8)->setText((Iridium::Utility::sizeToString(_data.speed) + "/s").c_str());

	terminate();
}

void TaskRowChild::init()
{
	TaskRow::init();

	at(0)->setText(_src.getName());
	at(0)->setToolTip(_src.getName());

	at(1)->setText(_dest.getName());
	at(1)->setToolTip(_dest.getName());
}
