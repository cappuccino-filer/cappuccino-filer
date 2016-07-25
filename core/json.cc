#include <json.hpp>
#include <unordered_map>
#include "json.h"

using json = nlohmann::basic_json<>;

#define CPCAST(x) (reinterpret_cast<const json*>(x))
#define PCAST(x) (reinterpret_cast<json*>(x))
#define CAST(x) (*(PCAST(x)))
#define PJSON (PCAST(json_.get()))
#define JSON (*PJSON)
#define CPJSON (CPCAST(json_.get()))
#define CJSON (*CPJSON)

ptree::Deleter::Deleter(bool is_reference)
	:is_reference_(is_reference)
{
}

void ptree::Deleter::operator()(void* ptr)
{
	if (!is_reference_)
		delete PCAST(ptr);
}

ptree::ptree()
	:json_(new json, Deleter())
{
}

ptree::ptree(void* ref)
	:json_(PCAST(ref), Deleter(true))
{
}

ptree::~ptree()
{
}

std::string
ptree::get(const std::string& path, const char* defvaule) const
{
	try {
		return get<std::string>(path);
	} catch (...) {
		return defvaule;
	}
}

template<typename T>
T
ptree::get(const std::string& path, const T& defvaule) const
{
	try {
		return get<T>(path);
	} catch (...) {
		return defvaule;
	}
}

namespace {
	json&
	recursive_find_reference(
		json& obj,
		std::string::const_iterator begin,
		std::string::const_iterator end,
		std::function<json&(json&, const std::string&)> select_function
		)
	{
		auto iter = std::find(begin, end, '.');
		std::string selector(begin, iter);
		if (selector.empty())
			return obj;
		json& next = select_function(obj, selector);
		if (iter == end)
			return next;
		return recursive_find_reference(
				next,
				iter + 1,
				end,
				select_function);
	}

	const json&
	recursive_find_reference(
		const json& obj,
		std::string::const_iterator begin,
		std::string::const_iterator end)
	{
		return recursive_find_reference(
				const_cast<json&>(obj),
				begin,
				end,
				[](json& obj, const std::string& selector)->json&
				{
					return obj.at(selector); // use at() since it's read-only.
				}
				);
	}

	json&
	recursive_find_reference(
		json& obj,
		std::string::const_iterator begin,
		std::string::const_iterator end)
	{
		return recursive_find_reference(
				obj,
				begin,
				end,
				[](json& obj, const std::string& selector)->json&
				{
					return obj[selector]; // use [] to create object on-the-fly.
				}
				);
	}

	json&
	recursive_find_reference(
		json& obj,
		const std::string& str)
	{
		return recursive_find_reference(obj, str.cbegin(), str.cend());
	}

	const json&
	recursive_find_reference(
		const json& obj,
		const std::string& str)
	{
		return recursive_find_reference(obj, str.cbegin(), str.cend());
	}
};

template<typename T>
T
ptree::get(const std::string& path) const
{
	try {
		const json& ref = recursive_find_reference(CJSON, path);
		return ref.get<T>();
	} catch (...) {
		throw bad_path(path);
	}
}

ptree
ptree::get_child(const std::string& path)
{
	json& ref = recursive_find_reference(JSON, path);
	return ptree(static_cast<void*>(&ref));
}

const ptree
ptree::get_child(const std::string& path) const
{
	try {
		auto ref = recursive_find_reference(CJSON, path);
		return ptree(const_cast<json*>(&ref));
	} catch (...) {
		throw bad_path(path);
	}
}

const ptree
ptree::get_child(size_t idx) const
{
	try {
		return ptree(const_cast<json*>(&(CJSON.at(idx))));
	} catch (...) {
		throw bad_path("Index " + std::to_string(idx));
	}
}

size_t
ptree::size() const
{
	return CJSON.size();
}

ptree::const_iterator
ptree::begin() const
{
	return const_iterator(this, 0);
}

ptree::const_iterator
ptree::end() const
{
	return const_iterator(this, size());
}

const ptree ptree::const_iterator::operator*()
{
	return parent_->get_child(position_);
}

void
ptree::put(const std::string& path, const char* value)
{
	put<std::string>(path, std::string(value));
}

template<typename T>
void
ptree::put(const std::string& path, const T& value)
{
	json& subobj = recursive_find_reference(JSON, path);
	subobj = value;
}

template<>
void
ptree::put<ptree>(const std::string& path, const ptree& other)
{
	json& subobj = recursive_find_reference(JSON, path);
	subobj = CAST(other.json_.get());
}

template<typename T>
void
ptree::push_back(const T& value)
{
	JSON.push_back(value);
}

template<>
void
ptree::push_back<ptree>(const ptree& other)
{
	JSON.push_back(*CPCAST(other.json_.get()));
}

void
ptree::swap_child_with(const std::string& path, ptree& obj)
{
	json& subobj = recursive_find_reference(JSON, path);
	subobj.swap(CAST(obj.json_.get()));
}

ptree
ptree::create()
{
	return ptree();
}

ptree
ptree::dup()
{
	ptree ret;
	CAST(ret.json_.get()) = JSON;
	return ret;
}

ptree
ptree::mkerror(const std::string& message)
{
	ptree ret;
	ret.put("class",  "error");
	ret.put("message", message);
	return ret;
}

void ptree::load_from(std::string& buf)
{
	JSON = json::parse(buf);
}

void ptree::load_from(std::istream& buf)
{
	JSON = json::parse(buf);
}

void ptree::dump_to(std::string& buf, const int indent) const
{
	buf = JSON.dump(indent);
}

void ptree::dump_to(std::ostream& fout) const
{
	fout << JSON;
}

void ptree::swap(ptree& other)
{
	json_.swap(other.json_);
}

std::ostream& operator<<(std::ostream& fout, ptree pt)
{
	pt.dump_to(fout);
	return fout;
}

#define INSTANT_ALL(INSTMACRO)	INSTMACRO(std::string) \
	INSTMACRO(bool)\
	INSTMACRO(std::int64_t)\
	INSTMACRO(std::uint64_t)\
	INSTMACRO(int)\
	INSTMACRO(long long)\
	INSTMACRO(double)

#define INSTANTIATION(T) template T ptree::get(const std::string& path, const T& defvaule) const;
INSTANT_ALL(INSTANTIATION)
#undef INSTANTIATION

#define INSTANTIATION(T) template T ptree::get(const std::string& path) const;
INSTANT_ALL(INSTANTIATION)
#undef INSTANTIATION

#define INSTANTIATION(T) template void ptree::put(const std::string& path, const T& vaule);
INSTANT_ALL(INSTANTIATION)
#undef INSTANTIATION

#define INSTANTIATION(T) template void ptree::push_back(const T& value);
INSTANT_ALL(INSTANTIATION)
#undef INSTANTIATION
