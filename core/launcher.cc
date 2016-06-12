#include "launcher.h"
#include "pref.h"

shared_ptree Launcher::launch(const std::string& prog, shared_ptree pt, bool superuser)
{
	auto proc = std::make_shared<QProcess>();
	proc->start(QString::fromStdString(prog));
	key_++;
	flying_[key_] = proc;
	std::string tmpstr;
	json_write_to_string(Pref::instance()->get_registry(), tmpstr);
	proc->write(tmpstr.data(), tmpstr.size());
	proc->write("\n", 1);
	json_write_to_string(pt, tmpstr);
	proc->write(tmpstr.data(), tmpstr.size());

	auto ret = std::make_shared<ptree>();
	pt->put("class", "plaunch");
	pt->put("jobkey", key_);
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
