#ifndef AFIO_H
#define AFIO_H

#include "caf/all.hpp"
#include "caf/io/all.hpp"

namespace AfIO {
	caf::actor io_actor();
	using read_atom = caf::atom_constant<atom("afio_read")>;
	constexpr auto read = read_atom::value;

	struct FileBlob {
		std::shared_ptr<std::string> data;

		FileBlob(const std::string& path, size_t off = 0, ssize_t len = -1);
	};

	class Exception : public std::runtime_error {
		Exception(const char* reason)
			:runtime_error(reason)
		{
		}
	};
}

#endif
