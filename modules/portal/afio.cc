#include "afio.h"
#include <string>

namespace AfIO {
	void io_engine(blocking_actor* self)
	{
		self->receive_loop(
			[](read_atom, const std::string& path, size_t off, ssize_t len) {
				try { 
					return FileBlob(path, off, len);
				} catch(...) {
					return Execption();
				}
			}
		);
	}

	caf::actor io_actor()
	{
		static auto actor = caf::spawn(io_engine);
		return actor;
	}

	FileBlob::FileBlob(const std::string& path, size_t off, ssize_t len)
		:data(make_shared<std::string>())
	{
		std::ifstream fin(path, std::ios::in | std::ios::binary);
		if (fin.is_open()) {
			fin.seekg(0, std::ios::end);
			size_t fsize = fin.tellg();
			if (off >= fsize)
				throw Execption("File read out of range");
			if (len < 0)
				data->resize(fsize);
			else
				data->resize(len);
			fin.seekg(off, std::ios::beg);
			fin.read(data->data(), data->size());
			fin.close();
		} else {
			throw Execption("File open error");
		}
	}
}
