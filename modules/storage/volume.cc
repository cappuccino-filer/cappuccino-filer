#include "volume.h"
#include "pref.h"
#include <database.h>
#include <boost/foreach.hpp>
#include <string>
#include <json.h>
#include <QDebug>
#include <QtCore/QProcess>
#include <soci/soci.h>

using soci::use;

Volume* Volume::instance()
{
	static Volume volume;
	return &volume;
}

Volume::Volume()
{
	auto& db = *(DatabaseRegistry::get_shared_dbc());
	soci::transaction tr(db);
	db << R"(CREATE TABLE IF NOT EXISTS volumes_table(
				uuid char(40) PRIMARY KEY,
				label char(32),
				mount text
			);)";
	db << R"(CREATE TABLE IF NOT EXISTS tracking_table(
				trID int PRIMARY KEY AUTO_INCREMENT,
				uuid char(40),
				FOREIGN KEY(uuid) REFERENCES volumes_table(uuid)
			);)";
	tr.commit();
}

void Volume::scan(DbConnection dbc)
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

	soci::transaction tr(*dbc);
	BOOST_FOREACH(ptree::value_type &iter, pt.get_child("blockdevices")) {
		auto& sub = iter.second;
		qDebug() << "lsblk item:" << sub.get<std::string>("uuid", "failed").c_str();
		auto mp = sub.get<std::string>("mountpoint", "");
		if (mp[0] != '/')
			continue;
		try {
			*dbc << "INSERT INTO volumes_table(uuid, label, mount) VALUES(:1, :2, :3) ON DUPLICATE KEY UPDATE mount=VALUES(mount)",
				use(sub.get("uuid", "failed")),
				use(sub.get("label", "")),
				use(mp);
		} catch (...) {
			qWarning() << "Exception thrown during inserting ";
		}
	}
	tr.commit();
}
