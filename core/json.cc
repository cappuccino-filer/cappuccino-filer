#include "json.h"
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <memory>

namespace {
	// Note: this templace function is simplified from boost::property_tree::json_parser::create_escapes
	// We asserts UTF-8 json text, therefore escaping UTF-8 characters are
	// unnecessary.
	template<typename Ch>
	std::basic_string<Ch> create_escapes_utf8(const std::basic_string<Ch> &s)
	{
		std::basic_string<Ch> result;
		typename std::basic_string<Ch>::const_iterator b = s.begin();
		typename std::basic_string<Ch>::const_iterator e = s.end();
		while (b != e) {
			Ch c(*b);
			if (*b == Ch('\b')) result += Ch('\\'), result += Ch('b');
			else if (*b == Ch('\f')) result += Ch('\\'), result += Ch('f');
			else if (*b == Ch('\n')) result += Ch('\\'), result += Ch('n');
			else if (*b == Ch('\r')) result += Ch('\\'), result += Ch('r');
			else if (*b == Ch('\t')) result += Ch('\\'), result += Ch('t');
			else if (*b == Ch('/')) result += Ch('\\'), result += Ch('/'); // FIXME: check if this is necessary
			else if (*b == Ch('"'))  result += Ch('\\'), result += Ch('"');
			else if (*b == Ch('\\')) result += Ch('\\'), result += Ch('\\');
			else result += *b;
			++b;
		}
		return result;
	}

	class json_value_visitor
		: public boost::static_visitor<>
	{
	public:
		json_value_visitor(std::string& buf)
			:value(buf)
		{
		}

		void operator()(void* const &i) const
		{
			value += "null"; // Serializing pointer is meaningless.
		}

		void operator()(const std::string & str) const
		{
			value += "\"";
			value += create_escapes_utf8(str);
			value += "\"";
		}

		// Json doesn't support 64bit integers for now
		// Anyway it's text in json.
		void operator()(const uintmax_t& v) const
		{
			value += "\"";
			value += std::to_string(v);
			value += "\"";
		}

		template <typename T>
		void operator()(const T& v) const
		{
			value += std::to_string(v);
		}
	private:
		std::string &value;
	};

	struct JsonValueTranslator
	{
		typedef property    internal_type;
		typedef std::string	external_type;

		// Converts boost::variant to string
		boost::optional<external_type> get_value(const internal_type& var)
		{
			std::string ret;
			boost::apply_visitor(json_value_visitor(ret), var);
			return ret;
		}
	};
}

shared_ptree json_mkerror(const std::string& message)
{
	auto pt = std::make_shared<ptree>();
	pt->put("class", "error");
	pt->put("message", message);
	return pt;
}

void json_write_to_string(const shared_ptree pt, std::string& str)
{
	json_write_to_string(*pt, str);
}

void json_write_to_string(const ptree& pt, std::string& str)
{
	std::stringstream jsonstream;
	vp_write_json_nocheck(jsonstream, pt);
	str = std::move(jsonstream.str());
}

shared_ptree create_ptree()
{
	return std::make_shared<ptree>();
}

// Note: this function is modified from boost::property_tree::json_parser::write_json_helper
void vp_write_json_nocheck(std::ostream &stream, const ptree &pt, int indent, bool pretty)
{
	typedef char Ch;
	typedef std::string Str;

	// Value or object or array
	if (indent > 0 && pt.empty()) {
		// Write value, JsonValueTranslator is responsible for
		// all possible escapes.
		stream << pt.template get_value<Str>(JsonValueTranslator());
	} else if (indent > 0 && pt.count(Str()) == pt.size()) {
		// Write array
		stream << Ch('[');
		if (pretty)
			stream << Ch('\n');
		typename ptree::const_iterator it = pt.begin();
		for (; it != pt.end(); ++it) {
			if (pretty)
				stream << Str(4 * (indent + 1), Ch(' '));
			vp_write_json_nocheck(stream, it->second, indent + 1, pretty);
			if (boost::next(it) != pt.end())
				stream << Ch(',');
			if (pretty)
				stream << Ch('\n');
		}
		if (pretty)
			stream << Str(4 * indent, Ch(' '));
		stream << Ch(']');
	} else {
		// Write object
		stream << Ch('{');
		if (pretty)
			stream << Ch('\n');
		typename ptree::const_iterator it = pt.begin();
		for (; it != pt.end(); ++it) {
			if (pretty)
				stream << Str(4 * (indent + 1), Ch(' '));
			stream << Ch('"') << create_escapes_utf8(it->first) << Ch('"') << Ch(':');
			if (pretty)
				stream << Ch(' ');
			vp_write_json_nocheck(stream, it->second, indent + 1, pretty);
			if (boost::next(it) != pt.end())
				stream << Ch(',');
			if (pretty)
				stream << Ch('\n');
		}
		if (pretty)
			stream << Str(4 * indent, Ch(' '));
		stream << Ch('}');
	}
}

void json_read_from_stream(std::istream& fin, ptree& pt)
{
	boost::property_tree::read_json(fin, pt);
}
