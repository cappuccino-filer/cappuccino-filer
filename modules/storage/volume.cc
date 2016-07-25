#include "volume.h"
#include "pref.h"
#include <database.h>
#include <database_query_table.h>
#include <string>
#include <json.h>
#include <launcher.h>
#include <QDebug>
#include <QtCore/QProcess>
#include <soci/soci.h>
#include <boost/lexical_cast.hpp>

using soci::use;
using soci::into;
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
	db << RETRIVE_SQL_QUERY(query::meta, CREATE_VOLUME_RECORD_TABLE);
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
	//qDebug() << "lsblk output:" << all.toStdString().c_str();
	std::stringstream ss(all.toStdString());
	ptree pt;
	pt.load_from(ss);
	ptree bsub = pt.get_child("blockdevices");
	qDebug() << "lsblk blockdevices size:" << pt.get_child("blockdevices").size();
	//std::cerr << "lsblk blockdevices from ptree:" << pt;
	//std::cerr << "lsblk blockdevices from bsub[0]:" << bsub.get_child(1);

	soci::transaction tr(*dbc);
	for(const ptree& sub: pt.get_child("blockdevices")) {
#if 0
		string tmp;
		sub.dump_to(tmp);
		qDebug() << "lsblk json item" << tmp.c_str();
#endif
		string uuid = sub.get<std::string>("uuid", "");
		if (uuid.size() < 30) {
			qDebug() << "Skip device "
				 << sub.get<std::string>("name", "").c_str()
				 << " for non-unique UUID";
			continue;
		}
		qDebug() << "lsblk item:" << uuid.c_str();
		auto mp = sub.get("mountpoint", "");
		if (mp[0] != '/')
			continue;
		try {
			*dbc << RETRIVE_SQL_QUERY(query::meta, UPSERT_SEEN_VOLUME),
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
	shared_ptree ret;
	auto dbc = DatabaseRegistry::get_shared_dbc();
	//scan(dbc);
	try {
		soci::rowset<soci::row> mpoints = (dbc->prepare <<
			RETRIVE_SQL_QUERY(query::meta, LIST_SEEN_VOLUMES));
		ptree content;
		for(auto& row : mpoints) {
			ptree vol;
			vol.put("uuid", row.get<string>(0));
			vol.put("mount", row.get<string>(1));
			auto value = row.get<long long>(2);
			vol.put("tracking", value != 0);
#if 0
			std::string tmp;
			json_write_to_string(vol, tmp);
#endif
			content.push_back(vol);
		}
		ret.put("volumelist", content);
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
	std::vector<std::string> path_list;
	try {
		auto dbc = DatabaseRegistry::get_shared_dbc();
		soci::transaction tr1(*dbc);
		for(const ptree vol : pt.get_child("volumelist")) {
			// Retrive volume from ptree
			auto uuid = vol.get<std::string>("uuid", "");
			auto mp = vol.get<std::string>("mount", "");
			auto tracking_str = vol.get<std::string>("tracking", "");
			qDebug() << "request volume item:" << uuid.c_str() << "\t" << mp.c_str() << "\t" << tracking_str.c_str();
			bool tracking;
			std::istringstream(tracking_str) >> std::boolalpha >> tracking;

			// Check the old state
			int current_tracking = -1;
			(*dbc) << "SELECT tracking FROM tracking_table WHERE uuid = :uuid",
				into(current_tracking), use(uuid);
			if (current_tracking == -1)
				current_tracking = 0; // Non-existing equals to false
			if (!!current_tracking == tracking)
				continue ; // State not changed, continue to the next
			qDebug() << "Changing volume :" << uuid.c_str() << "\t" << mp.c_str();

			if (!tracking) {
				// Sync the state
				// Note: setting non-tracking -> tracking should be
				// done by updatedb
				*dbc << RETRIVE_SQL_QUERY(query::meta, UPSERT_TRACKING_RECORD),
					use(uuid),
					use((int)tracking),
					use((int)tracking);
				continue; 
			}

			if (mp.empty()) {
				qDebug() << "Cannot initiating tracking on Volume "
					<< uuid.c_str()
					<< ": volume was not mounted.";
				continue;
			}
			path_list.emplace_back(mp);
		}
		tr1.commit();
	} catch (soci::soci_error& e) {
		qDebug() << e.what();
	} catch (std::exception& e) {
		qDebug() << e.what();
	} catch (...) {
		std::string buf;
		pt.dump_to(buf);
		qDebug() << "Error in parsing json " << buf.c_str();
	}
	if (!path_list.empty()) {
		ptree paths;
		for(const auto& mp : path_list) {
			ptree path;
			path.put("", mp);
			paths.push_back(path);
		}
		// Launch the updatedb process
		shared_ptree req;
		req.put("paths", paths);
		std::string buf;
		req.dump_to(buf);
		qDebug() << "Paths " << buf.c_str();
		Launcher::instance()->launch(Pref::instance()->get_pref("core.libexecpath")+"updatedb", req);
	}
	return ls_volumes();
}
