#ifndef CORE_JSON_H
#define CORE_JSON_H

#include <string>
#include <istream>
#include <ostream>
#include <memory>
#include <stdexcept>
#include <iterator>

class ptree {
public:
	ptree();
	explicit ptree(void*);
	~ptree();

	std::string get(const std::string& path, const char* defvalue) const;
	template<typename T> T get(const std::string& path, const T& defvalue) const;
	template<typename T> T get(const std::string& path = std::string()) const;
	template<typename T> T get(size_t) const;
	ptree get_child(const std::string& path);
	const ptree get_child(const std::string& path) const;
	const ptree get_child(size_t) const;

	size_t size() const; // return the number of elements
	// We do not provide non-const iterators, use put and push_back
	// instead.
	class const_iterator : public std::iterator<
			       std::random_access_iterator_tag,
			       ptree>
	{
	private:
		friend class ptree;
		const ptree* parent_;
		size_t position_;
	public:
		const_iterator(const ptree* parent, size_t pos)
			: parent_(parent), position_(pos)
		{
		}
		const ptree operator*();
		const_iterator operator++() { auto ret = *this; position_++; return ret; };
		const_iterator& operator++(int) { ++position_; return *this; };
		const_iterator operator--() { position_--; return *this; };
		bool operator!=(const ptree::const_iterator& other)
		{
			return parent_ != other.parent_ || position_ != other.position_;
		}
		bool operator==(const ptree::const_iterator& other)
		{
			return !(*this != other);
		}
		const_iterator& operator+=(ssize_t i)
		{
			position_ += i;
			return *this;
		}
		const_iterator& operator-=(ssize_t i)
		{
			return operator+=(-i);
		}
	};
	const_iterator begin() const;
	const_iterator end() const;

	void put(const std::string& path, const char* value);
	void put(const std::string& path, char* value) { put(path, (const char*)value); }
	void put(const std::string& path, std::string&& value);
	template<typename T> void put(const std::string& path, const T& value);
	template<typename T> void push_back(const T& value);
	void push_back(ptree&& other);
	void swap_child_with(const std::string& path, ptree& obj); // Cheap way to attach an json object to another one.
	ptree dup();

	static ptree create();
	static ptree mkerror(const std::string& message);

	void load_from(std::string&);
	void load_from(std::istream&);
	void dump_to(std::string&, const int indent = -1) const;
	void dump_to(std::ostream&) const;

	class bad_path : public std::runtime_error {
		using runtime_error::runtime_error;
	};

	void swap(ptree& other);
private:
	class Deleter {
	public:
		Deleter(bool is_reference = false);
		void operator()(void*);
	private:
		bool is_reference_;
	};

	std::shared_ptr<void> json_;
};

using shared_ptree = ptree;

std::ostream& operator<<(std::ostream&, ptree);

#endif
