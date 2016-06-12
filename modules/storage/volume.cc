#include <boost/foreach.hpp>
#include "volume.h"
#include "pref.h"
#include <database.h>
#include <string>
#include <json.h>
#include <QDebug>
#include <QtCore/QProcess>
#include <soci/soci.h>

using soci::use;
using std::cout;

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
				tracking int NOT NULL DEFAULT 0,
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
	json_read_from_stream(ss, pt);

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
	qWarning() << "Commited";
}

shared_ptree Volume::ls_volumes()
{
	shared_ptree ret = create_ptree();
	auto dbc = DatabaseRegistry::get_shared_dbc();
	//scan(dbc);
	try {
		soci::rowset<soci::row> mpoints = (dbc->prepare <<
				"SELECT volumes_table.uuid, mount, CASE WHEN tracking_table.trID IS NOT NULL THEN tracking_table.tracking ELSE 0 END AS tracking FROM volumes_table LEFT JOIN tracking_table ON (volumes_table.uuid = tracking_table.uuid);");
		ptree content;
		for(auto& row : mpoints) {
			ptree vol;
			vol.put("uuid", row.get<string>(0));
			vol.put("mount", row.get<string>(1));
			auto value = row.get<long long>(2);
			vol.put("tracking", value != 0);
			std::string tmp;
			json_write_to_string(vol, tmp);
			content.push_back(std::make_pair("", vol));
		}
		ret->add_child("volumelist", content);
	} catch (std::exception& e) {
		qDebug() << e.what();
	}
#if 0
	std::string tmp;
	json_write_to_string(ret, tmp);
	qDebug() << "Volumes: " << tmp.c_str();
#endif
	return ret;
}

// FIXME: acutal handle something
shared_ptree Volume::handle_request(shared_ptree pt)
{
	return pt;
}
