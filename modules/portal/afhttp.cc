#include "afhttp.hpp"

namespace AfHttpd {

Response::Response(int code, IOActor::FileBlob blob)
	:ptr_(std::make_shared<ResponseData>())
{
	ptr_->status_code = code;
	ptr_->content = std::move(*blob.data);
}

Response(int code, const std::string& content)
	:ptr_(std::make_shared<ResponseData>())
{
	ptr_->status_code = code;
	ptr_->content = content;
}

void Response::offload(caf::io::connection_handle hdl, caf::io::broker* broker)
{
	// TODO
	// API void broker::write(connection_handle hdl, size_t num_bytes, const void* buf)
}

}
