#include "tag.h"
#include "filetag.h"
#include <soci/soci.h>
#include <database.h>
#include <database_query_table.h>
#include <pref.h>
#include <unordered_map>
#include <string>
#include <functional>
#include <set>
#include <QDebug>

using std::string;

TagAction::TagAction(shared_ptree pt)
	:pt_(pt)
{
}

TagAction::~TagAction()
{
}

void TagAction::render_fail(ptree& ret, const char* reason)
{
	ret.put("cat", pt_.get("cat", ""));
	ret.put("result", "Error");
	ret.put("reason", reason);
}

void TagAction::render_ok(ptree& ret)
{
	ret.put("cat", pt_.get("cat", ""));
	ret.put("result", "OK");
}

class DummyTagAction : public TagAction {
public:
	using TagAction::TagAction;

	virtual shared_ptree act() override
	{
		ptree ret;
		render_fail(ret, "Unsupported tag action category");
		string oldjson;
		pt_.dump_to(oldjson);
		ret.put("request", std::move(oldjson));
		return ret;
	}
};

class CreateTag : public TagAction {
public:
	using TagAction::TagAction;

	virtual shared_ptree act() override
	{
		ptree ret;
		string name = pt_.get("name", "");
		if (name.empty()) {
			render_fail(ret, "Tag name is empty");
			return ret;
		}

		auto dbc = DatabaseRegistry::get_shared_dbc();
		int tid;
		*dbc << RETRIVE_SQL_QUERY(query::tag, CREATE),
			soci::into(tid),
			soci::use(name);
		render_ok(ret);
		ret.put("name", name);
		ret.put("tagid", tid);
		return ret;
	}
};

class ListTag : public TagAction {
public:
	using TagAction::TagAction;

	virtual shared_ptree act() override
	{
		ptree ret;
		int offset;
		int limit;
		offset = pt_.get("offset", -1);
		limit = pt_.get("limit", -1);
		int limit_cap = Pref::instance()->get_registry().get("tagger.list.limits", 100);

		if (offset < 0) {
			render_fail(ret, "Invalid offset");
			return ret;
		} else if (limit < 0) {
			render_fail(ret, "Invalid limit");
			return ret;
		}
		limit = std::min(limit, limit_cap);

		auto dbc = DatabaseRegistry::get_shared_dbc();
		soci::rowset<soci::row> taglist = (dbc->prepare <<
				RETRIVE_SQL_QUERY(query::tag, LIST),
				soci::use(offset),
				soci::use(limit));
		render_ok(ret);
		ptree array;
		for (const auto& row: taglist) {
			ptree tag;
			tag.put("id", row.get<int>(0));
			tag.put("name", row.get<string>(1));
			array.push_back(std::move(tag));
		}
		ret.swap_child_with("tags", array);
		return ret;
	}
};

class LocateTag : public TagAction {
public:
	using TagAction::TagAction;

	virtual shared_ptree act() override
	{
		ptree ret;
		bool create_if_not_exists = pt_.get("options.create_if_not_exists", false);

		string name = pt_.get("name", "");
		if (name.empty()) {
			render_fail(ret, "Invalid tag name");
			return ret;
		}
		
		try {
			const ptree cpt = pt_;
			ptree constraints = cpt.get_child("constraints");
			ret = handle(name, create_if_not_exists, &constraints);
		} catch (ptree::bad_path& e) {
			ret = handle(name, create_if_not_exists, nullptr);
		}
		return ret;
	}
protected:

	ptree render_taglist(soci::rowset<soci::row>& taglist)
	{
		ptree ret, array;
		render_ok(ret);
		ret.put("name", pt_.get("name", ""));
		for (const auto& row: taglist) {
			array.push_back(row.get<int>(0));
		}
		if (array.size() > 0) {
			ret.swap_child_with("tagids", array);
		} else {
			ret.put("result", "Not found");
			ret.put("tagids", nullptr);
		}
		return ret;
	}

	shared_ptree handle(const string& name, int create_if_not_exists, const ptree* constraints)
	{
		auto dbc = DatabaseRegistry::get_shared_dbc();
		if (!constraints) {
			//qDebug() << "Handle: " << name.c_str() << "\tcreate: " << create_if_not_exists << "\tconstraints: " << constraints;
			soci::rowset<soci::row> taglist = (dbc->prepare <<
				RETRIVE_SQL_QUERY(query::tag, NAME_TO_ID),
				soci::use(name),
				soci::use(create_if_not_exists));
			return render_taglist(taglist);
		} else {
			// FIXME: use a dedicated function to handle more complicated queries.
			std::set<int> parent_tags;
			for (size_t i = 0; i < constraints->size(); i++) {
				const ptree constraint = constraints->get_child(i);
				if (constraint.get("cat", "") != string("tagged_with"))
					continue;
				const ptree tags = constraint.get_child("tags");
				for (size_t j = 0; j < constraint.size(); j++) {
					parent_tags.insert(tags.get_child(j).get<int>());
				}
			}
			soci::rowset<soci::row> taglist = (dbc->prepare <<
				DatabaseRegistry::get_sql_provider()->query_where_in(query::tag::cat_id, query::tag::NAME_TO_ID_EX, parent_tags),
				soci::use(name),
				soci::use(create_if_not_exists));
			return render_taglist(taglist);
		}
	}
};

class TagAnotherTag : public TagAction {
public:
	using TagAction::TagAction;
	virtual shared_ptree act() override
	{
		ptree ret;
		const ptree pt = pt_;
		try {
			int taggee = pt_.get<int>("taggee");
			int tagger = pt_.get<int>("tagger");
			double p = pt_.get<double>("probability");
			if (p < 0)
				do_find(ret, taggee, tagger);
			else
				do_upsert(ret, taggee, tagger, p);
		} catch (ptree::bad_path& e) {
			TagAction::render_fail(ret, (string("Missing argument: ") + e.what()).c_str());
		}
		return ret;
	}
protected:
	void do_find(ptree& ret, int taggee, int tagger)
	{
		auto dbc = DatabaseRegistry::get_shared_dbc();
		soci::rowset<soci::row> ttrlist  = (dbc->prepare <<
				RETRIVE_SQL_QUERY(query::tag, FIND_TAG_TAG_RELATION),
				soci::use(taggee),
				soci::use(tagger)
				);
		ptree array;
		render_tt_array(array, ttrlist);
		if (array.size() > 0) {
			render_ok(ret);
			ret.swap_child_with("rels", array);
		} else {
			render_fail(ret, "Relationship not found");
		}
	}

	void do_upsert(ptree& ret, int taggee, int tagger, double p)
	{
		auto dbc = DatabaseRegistry::get_shared_dbc();
		soci::rowset<soci::row> ttrlist  = (dbc->prepare <<
				RETRIVE_SQL_QUERY(query::tag, UPSERT_TAG_TAG_RELATION),
				soci::use(taggee),
				soci::use(tagger),
				soci::use(p)
				);
		ptree array;
		render_tt_array(array, ttrlist);
		if (array.size() > 0) {
			render_ok(ret);
			ret.swap_child_with("rels", array);
		} else {
			render_fail(ret, "Fail to upsert tag-tag relation");
		}
	}

	void render_tt_array(ptree& array, soci::rowset<soci::row>& ttrlist)
	{
		for (const auto& row: ttrlist) {
			ptree ttr;
			ttr.put("relid", row.get<int>(0));
			ttr.put("taggee", row.get<int>(1));
			ttr.put("tagger", row.get<int>(2));
			ttr.put("probability", row.get<float>(3));
			array.push_back(std::move(ttr));
		}
	}
};

class DeleteTags : public TagAction {
public:
	using TagAction::TagAction;

	virtual shared_ptree act() override
	{
		ptree ret;
		auto dbc = DatabaseRegistry::get_shared_dbc();
		try {
			ptree todels = pt_.get_child("tagid");
			for (size_t i = 0; i < todels.size(); i++) {
				int tag = todels.get<int>(i);
				(*dbc) << RETRIVE_SQL_QUERY(query::tag, DELETE), soci::use(tag);
			}
		} catch (ptree::bad_path& e) {
			TagAction::render_fail(ret, (string("Missing argument: ") + e.what()).c_str());
		}
		// FIXME: If tag id does not exist.
		render_ok(ret);
		return ret;
	}
};

namespace {
	typedef std::unique_ptr<TagAction> ActionPtr;
	typedef std::function<ActionPtr(shared_ptree)> fab_function_t;

#define NAIVE_FAB(T) [](shared_ptree pt)->ActionPtr { return std::make_unique<T>(pt); }

	std::unordered_map<string, fab_function_t> tagfabmap = {
		{"create", NAIVE_FAB(CreateTag) }
		, {"list", NAIVE_FAB(ListTag) }
		, {"name2id", NAIVE_FAB(LocateTag) }
		, {"tagtag", NAIVE_FAB(TagAnotherTag) }
		, {"delete_tag", NAIVE_FAB(DeleteTags) }
#if 0 // Disable them first.
		, {"tagrel", NAIVE_FAB(TagRelation) }
#endif
	};

	std::unordered_map<string, fab_function_t> filetagfabmap = {
#if 0 // Disable them first.
		{"assign", NAIVE_FAB(AssigTagToFile) }
		, {"tagrel", NAIVE_FAB(TagRelation) }
#endif
	};

#undef NAIVE_FAB

	std::unique_ptr<TagAction>
	find_and_construct(const std::unordered_map<string, fab_function_t>& fabmap,
			   ptree pt)
	{
		string cat = pt.get("cat", "");
		auto iter = fabmap.find(cat);
		if (iter == fabmap.end())
			return std::make_unique<DummyTagAction>(pt);
		else
			return iter->second(pt);
	}
};

std::unique_ptr<TagAction> TagActionFab::fab(shared_ptree pt)
{
	return find_and_construct(tagfabmap, pt);
}

std::unique_ptr<TagAction> FileTagActionFab::fab(shared_ptree pt)
{
	return find_and_construct(filetagfabmap, pt);
}
