#ifndef CORE_JSON_H
#define CORE_JSON_H

#include <string>

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>

inline std::ostream& operator<<(std::ostream& fout, decltype(nullptr))
{
	fout << "<null>";
	return fout;
}

typedef boost::variant<void*, std::string, int, uintmax_t, double, bool> property;
typedef boost::property_tree::basic_ptree<std::string, property> ptree;
typedef std::shared_ptr<ptree> shared_ptree;

template<typename InputType>
struct VariantPropertyWriter {
	static property put_value(InputType v)
	{
		return property(v);
	}
};

template<>
struct VariantPropertyWriter<const char*> {
	static property put_value(const char* v)
	{
		std::string str(v);
		return property(str);
	}
};

struct VariantReadStringVisitor : public boost::static_visitor<> {
private:
	std::string& value;
public:
	VariantReadStringVisitor(std::string& buf)
		:value(buf)
	{
	}
	void operator()(void* const &i) const
	{
		std::ostringstream oss;
		oss << (void*)i;
		value += oss.str();
		// Nothing to do here
	}
	void operator()(const std::string & str) const
	{
		value += str;
	}
	template <typename T>
	void operator()(const T& v) const
	{
		value += std::to_string(v);
	}
};

template<typename ExternalType>
struct VariantPropertyTranslator {
	typedef ExternalType external_type;
	typedef property internal_type;
	boost::optional<ExternalType> get_value(const property& var)
	{
		std::string val;
		boost::apply_visitor(VariantReadStringVisitor(val), var);
		return boost::get<ExternalType>(var);
	}

	template<typename T>
	boost::optional<property> put_value(T v)
	{
		return VariantPropertyWriter<T>::put_value(v);
	}
};

template<>
struct VariantPropertyTranslator<std::string> {
	typedef std::string external_type;
	typedef property internal_type;

	boost::optional<std::string> get_value(const property& var)
	{
		std::string ret;
		boost::apply_visitor(VariantReadStringVisitor(ret), var);
		return ret;
	}

	template<typename T>
	boost::optional<property> put_value(T v)
	{
		return VariantPropertyWriter<T>::put_value(v);
	}
};

namespace boost { namespace property_tree {
template<typename T>
struct translator_between<property, T> {
	typedef VariantPropertyTranslator<T> type;
};
}}

shared_ptree json_mkerror(const std::string&);
shared_ptree create_ptree();
void json_write_to_string(const shared_ptree pt, std::string&);
void json_write_to_string(const ptree& pt, std::string&);
void json_read_from_stream(std::istream&, ptree& pt);
void vp_write_json_nocheck(std::ostream &stream, const ptree &pt, int indent = 0, bool pretty = false);

#endif
