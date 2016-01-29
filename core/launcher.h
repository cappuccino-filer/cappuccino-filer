#ifndef CORE_LAUNCHER_H
#define CORE_LAUNCHER_H

#include <string>
#include <vector>
#include <memory>
#include <QtCore/QProcess>
#include <json.h>

class Launcher {
public:
	shared_ptree launch(const std::string&, shared_ptree args, bool superuser = false);
	std::shared_ptr<QProcess> get_job(int key);
	shared_ptree list_all() const;

	static Launcher* instance();
private:
	std::map<int, std::shared_ptr<QProcess>> flying_;
	int key_ = 0;
};

#endif
