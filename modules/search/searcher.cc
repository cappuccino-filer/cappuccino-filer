#include "searcher.h"
#include <database.h>
#include <soci/soci.h>
#include <database_query_table.h>
#include <QDebug>

using std::string;
using std::make_unique;


Searcher::Searcher(ptree r)
	: req_(r)
{
	ans_.put("cat", "search result");
}

Searcher::~Searcher()
{
}

class NoGoSearcher : public Searcher {
public:
	NoGoSearcher(const char* reason)
		:Searcher(ptree()), reason_(reason)
	{
	}

	ptree do_search() override
	{
		ans_.put("status", reason_);
		return ans_;
	}
private:
	const char* reason_;
};

struct FileResult;
class RegexSearcher : public Searcher {
public:
	using Searcher::Searcher;

	ptree do_search() override;
private:
	void search_vol(long long volid,
			const string& mount,
			const string& pattern,
			ptree result_array);
	std::vector<FileResult> file_results_; // FIXME: batch query
};

std::unique_ptr<Searcher> SearcherFab::fab(ptree pt)
{
	string tmp;
	pt.dump_to(tmp);
	qDebug() << "Incoming search request: " << tmp.c_str();

	qDebug() << "Incoming search class : " << pt.get<string>("class", "").c_str();
	if (pt.get<string>("cat", "") != "byname")
		return make_unique<NoGoSearcher>("Unsupported search class.");

	if (pt.get("matcher", "") != "regex")
		return make_unique<NoGoSearcher>("Only 'regex' is supported.");

	if (pt.get("pattern", "").empty())
		return make_unique<NoGoSearcher>("'pattern' cannot be empty.");

	return make_unique<RegexSearcher>(pt);
}

struct FileResult {
	// Required
	string name;
	uint64_t size;
	string path;
	long long mtime_sec;
	long long mtime_nsec;

	// Cached
	uint64_t inode;
};

ptree
RegexSearcher::do_search()
{
	string pattern = get<string>("pattern", "");
	auto dbc = DatabaseRegistry::get_shared_dbc();

	soci::rowset<soci::row> indexed = (dbc->prepare <<
R"zzz(
SELECT trID, volumes_table.uuid, mount 
FROM tracking_table LEFT JOIN volumes_table ON (tracking_table.uuid = volumes_table.uuid);
)zzz"
);
	ans_.put("result", "OK");
	try {
		ptree array;
		for (auto& row : indexed) {
			auto trID = row.get<int>(0);
			auto mount = row.get<string>(2);
			search_vol(trID, mount, pattern, array);
		}
		ans_.swap_child_with("items", array);
	} catch(std::exception& e) {
		ans_.put("result", "Error");
		ans_.put("reason", e.what());
	}
	return ans_;
}


void
RegexSearcher::search_vol(long long volid,
		const string& mount,
		const string& pattern,
		ptree result_array)
{
	if (volid < 0)
		return ;
	auto dbc = DatabaseRegistry::get_shared_dbc();
	auto sqlprovider = DatabaseRegistry::get_sql_provider();
	
	soci::rowset<soci::row> indexed = (dbc->prepare <<
			sqlprovider->query_volume(volid, query::volume::REGEX_NAME_MATCH),
			soci::use(pattern)
		);
	std::string volstr = std::to_string(volid);
	for(const auto& row: indexed) {
		ptree item;
		item.put("name", row.get<string>(0));
		if (mount.empty()) 
			item.put("path", volstr + row.get<string>(1));
		else
			item.put("path", mount + row.get<string>(1));
		result_array.push_back(std::move(item));
	}
}
