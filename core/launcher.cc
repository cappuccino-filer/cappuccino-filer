#include "launcher.h"
#include "pref.h"
#include <QDebug>

shared_ptree Launcher::launch(const std::string& prog, shared_ptree pt, bool superuser)
{
	auto proc = std::make_shared<QProcess>();
#if 1
	proc->setInputChannelMode(QProcess::ManagedInputChannel);
	proc->setProcessChannelMode(QProcess::ForwardedErrorChannel);
#endif
	proc->start(QString::fromStdString(prog));
	if (!proc->waitForStarted()) {
		auto ret = create_ptree();
		ret->put("class", "plaunch");
		ret->put("status", "timeout");
		return ret;
	}
	key_++;
	flying_[key_] = proc;
	std::string tmpstr1, tmpstr2;
	json_write_to_string(Pref::instance()->get_registry(), tmpstr1);
	json_write_to_string(pt, tmpstr2);
	uint32_t size1 = uint32_t(tmpstr1.size());
	uint32_t size2 = uint32_t(tmpstr2.size());
	qDebug() << proc->write((const char*)&size1, sizeof(uint32_t));
	qDebug() << proc->write((const char*)&size2, sizeof(uint32_t));
	qDebug() << proc->write(tmpstr1.data(), tmpstr1.size());
	qDebug() << proc->write(tmpstr2.data(), tmpstr2.size());
	qDebug() << proc->waitForBytesWritten();

	auto ret = create_ptree();
	ret->put("class", "plaunch");
	ret->put("status", "running");
	ret->put("jobkey", key_);
	return ret;
}

std::shared_ptr<QProcess> Launcher::get_job(int key)
{
	auto iter = flying_.find(key);
	if (iter != flying_.end())
		return iter->second;
	return std::shared_ptr<QProcess>(nullptr);
}

shared_ptree Launcher::list_all() const
{
	auto ret = std::make_shared<ptree>();
	for(const auto& item : flying_) {
		ptree process;
		process.put("PID", std::to_string(item.second->processId()));
		process.put("program", item.second->program().toStdString());
		// FIXME: add program arguments
		
		ret->add_child(std::to_string(item.first), process);
	}
	return ret;
}

Launcher* Launcher::instance()
{
	static Launcher l;
	return &l;
}
