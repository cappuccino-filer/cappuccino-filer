#include "volume.h"
#include "pref.h"
#include <database.h>
#include <boost/foreach.hpp>
#include <string>
#include <json.h>
#include <QDebug>
#include <QtCore/QProcess>

#include "tbl_volumes.h"

Volume* Volume::instance()
{
	static Volume volume;
	return &volume;
}

Volume::Volume()
{
	auto& db = *(DatabaseRegistry::get_db());
	db.execute(R"(CREATE TABLE IF NOT EXISTS tab_volumes(
				id int AUTO_INCREMENT PRIMARY KEY,
				uuid char(40) NOT NULL,
				label char(32),
				mount text,
				last_check datetime
			))");
}

void Volume::scan(DatabasePtr dbc)
{
	qDebug() << "Volume::scan " << dbc.get() ;
	QProcess lsblk;
	lsblk.start("lsblk", QStringList() << "--json" << "--fs" << "--paths" << "--list");

	if (!lsblk.waitForFinished()) {
		    qWarning() << "lsblk failed:" << lsblk.errorString();
			return;
	}
	auto all = lsblk.readAll();
	//qDebug() << "lsblk output:" << all;
	std::stringstream ss(all.toStdString());
	ptree pt;
	boost::property_tree::read_json(ss, pt);

	//auto& db = *dbc;
	const auto tblvol = tab_volumes::tab_volumes{};

	BOOST_FOREACH(ptree::value_type &iter, pt.get_child("blockdevices")) {
		auto& sub = iter.second;
		qDebug() << "lsblk item:" << sub.get<std::string>("uuid", "failed").c_str();
		auto mp = sub.get<std::string>("mountpoint", "");
		if (mp[0] != '/')
			continue;
		try {
			dbc->run(insert_into(tblvol).set(tblvol.uuid = sub.get("uuid", "failed"),
					tblvol.label = sub.get("label", ""),
					tblvol.mount = mp
					));
		} catch (...) {
			qWarning() << "Exception thrown during inserting ";
		}
	}
}
