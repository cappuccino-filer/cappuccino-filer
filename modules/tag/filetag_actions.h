// This file should only be included by tag.cc

class AssigTagToFile : public TagAction {
	struct PerFileRequest {
		uint64_t ino;
		std::vector<int> tags;
		std::vector<double> Ps;
	};
public:
	using TagAction::TagAction;

	virtual shared_ptree act() override
	{
		ptree ret;
		std::vector<PerFileRequest> requests;
		int volid;
		try {
			volid = pt_.get<int>("vol");
			ptree req_array = pt_.get_child("requests");
			for (const ptree req : req_array) {
				PerFileRequest pfr;
				std::istringstream iss(req.get<string>());
				iss >> pfr.ino;
				const ptree tags = req.get_child("tags");
				for (const ptree tag : tags)
					pfr.tags.emplace_back(tag.get<int>());
				const ptree Ps = req.get_child("Ps");
				for (const ptree P : Ps)
					pfr.Ps.emplace_back(P.get<double>());
				requests.emplace_back(std::move(pfr));
			}
		} catch (ptree::bad_path& e) {
			render_fail(ret, (string("Missing argument: ") + e.what()).c_str());
			return ret;
		}
		try {
			auto dbc = DatabaseRegistry::get_shared_dbc();
			string sql = DatabaseRegistry::get_sql_provider()->
				query_volume(volid, query::volume::ASSIGN_TAG);
			uint64_t cur_ino;
			int cur_tag;
			double cur_P;
			soci::statement stmt = (dbc->prepare << sql,
					soci::use(cur_ino),
					soci::use(cur_tag),
					soci::use(cur_P));
			soci::transaction tr1(*dbc);
			for(const auto& pfr : requests) {
				cur_ino = pfr.ino;
				size_t nassigns = std::min(
						pfr.tags.size(),
						pfr.Ps.size()
						);
				for (size_t i = 0; i < nassigns; i++) {
					cur_tag = pfr.tags[i];
					cur_P = pfr.Ps[i];
					stmt.execute(true);
				}
			}
			tr1.commit();
		} catch (std::exception& e) {
			render_fail(ret, (string("Throws exception: ") +
						e.what()).c_str());
			return ret;
		}
		render_ok(ret);
		return ret;
	}
};

