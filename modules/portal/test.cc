#include "afhttp.hpp"
#include "afhttp_util.h"
#include "afio.h"
#include <stdlib.h>

caf::behavior file_fetcher_beh(event_based_actor* self)
{
	return {
		[self](AfHttpd::Frame frame) {
			auto path = AfHttpd::jail_path(frame->path());

			self->sync_send(AfIO::io_actor(), AfIO::read, path).then(
				[self](IOActor::FileBlob blob) {
					// Done, 200 OK
					return AfHttpd::Response(200, blob);
				}
				[self](IOActor::Execption e) {
					// File Not found or similar error
					auto path = AfHttpd::jail_path("/404.html");
					self->sync_send(AfIO::io_actor(), AfIO::read, path).then(
						[self](IOActor::FileBlob blob) {
							return AfHttpd::Response(404, blob);
						},
						[self](IOActor::Execption) {
							return AfHttpd::Response(404, e.what());
						}
					}
				}
			);
		}
	};
}

int main()
{
	// Server
	AfHttpd::Httpd server;
	auto reactor = caf::spawn(file_fetcher_beh);
	server.default_actor["GET"]  = [reactor]() { return reactor; }
	server.active(8080);

	system("read"); // Wait for pressing enter

	server.shutdown();
	AfIO::instance()->shutdown();
	caf::await_all_actors_done();
	caf::shutdown();

	return 0;
}
