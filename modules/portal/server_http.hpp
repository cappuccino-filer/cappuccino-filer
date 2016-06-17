#ifndef SERVER_HTTP_HPP
#define	SERVER_HTTP_HPP

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/thread/future.hpp>

#include <regex>
#include <memory>
#include <unordered_map>
#include <thread>
#include <functional>
#include <iostream>

namespace SimpleWeb {
	using std::shared_ptr;
	using std::make_shared;
	using boost::system::error_code;
	namespace asio = boost::asio;

	template <class socket_type>
	class ServerBase;

	template <class socket_type>
	class RequestTempl {
		friend class ServerBase<socket_type>;
	public:
		std::string method, path, http_version;
		std::istream content;
		std::unordered_multimap<std::string, std::string> header;
		std::smatch path_match;
		std::string remote_endpoint_address;
		unsigned short remote_endpoint_port;
		
	private:
		RequestTempl(): content(&streambuf) {}
		
		asio::streambuf streambuf;
		
		void read_remote_endpoint_data(socket_type& socket) {
			try {
				remote_endpoint_address = socket.lowest_layer().remote_endpoint().address().to_string();
				remote_endpoint_port = socket.lowest_layer().remote_endpoint().port();
			}
			catch(const std::exception& e) {
				std::cerr << e.what() << std::endl;
			}
		}

		static auto create()
		{
			return shared_ptr<RequestTempl>(new RequestTempl);
		}
	};

	template <class socket_type>
	class ResponseTempl {
		friend class ServerBase<socket_type>;
		typedef shared_ptr<socket_type> shared_socket;
	private:
		shared_ptr<asio::strand> strand;
		asio::yield_context& yield;
		asio::streambuf streambuf;
		shared_socket socket;
		
		ResponseTempl(
			asio::io_service& io_service,
			shared_socket socket,
			shared_ptr<asio::strand> strand, 
			asio::yield_context& yield) :
				strand(strand),
				yield(yield),
				socket(socket),
				stream(&streambuf)
		{}

		ResponseTempl() = delete;

		typedef std::function<void(const error_code&)> error_report_function;

		void async_flush(error_report_function callback = nullptr)
		{
			if(!socket->lowest_layer().is_open()) {
				throw std::runtime_error("Broken pipe.");
			}
			
			auto write_buffer = make_shared<asio::streambuf>();
			std::ostream response(write_buffer.get());
			response << stream.rdbuf();
												
			//*async_writing=true;
			
			auto socket_=this->socket;
			//auto async_writing_=this->async_writing;
			
			asio::async_write(*socket,
				*write_buffer, 
				strand->wrap(
					[socket_, write_buffer, callback]
						(const error_code& ec,
						 size_t bytes_transferred)
					{
						if(callback)
							callback(ec);
					}
				)
			);
		}
		
		void flush() {
			asio::streambuf write_buffer;
			std::ostream response(&write_buffer);
			response << stream.rdbuf();

			asio::async_write(*socket, write_buffer, yield);
		}

	public:
		std::ostream stream;
		
		template <class T>
		ResponseTempl& operator<<(const T& t) {
			stream << t;
			return *this;
		}

		ResponseTempl& operator<<(std::ostream& (*manip)(std::ostream&)) {
			stream << manip;
			return *this;
		}
		
		ResponseTempl& operator<<(ResponseTempl& (*manip)(ResponseTempl&)) {
			return manip(*this);
		}

		boost::future<void> future;
		boost::promise<int> promise;
	};


	template <class socket_type>
	class Exchange {
	public:
		shared_ptr<RequestTempl<socket_type>> request;
		shared_ptr<ResponseTempl<socket_type>> response;
	};

	template <class socket_type>
	class ServerBase {
	public:
		typedef ResponseTempl<socket_type> Response;
		typedef RequestTempl<socket_type> Request;
		typedef shared_ptr<Response> ResponsePtr;
		typedef shared_ptr<Request> RequestPtr;
		typedef shared_ptr<socket_type> shared_socket;
		
		static Response& async_flush(Response& r) {
			r.async_flush();
			return r;
		}
		
		static Response& flush(Response& r) {
			r.flush();
			return r;
		}
		
	
		typedef std::function<boost::future<int>(ResponsePtr, RequestPtr)> ResourceFunction;
		std::unordered_map<std::string, std::unordered_map<std::string, ResourceFunction>>  resource;
		std::unordered_map<std::string, ResourceFunction> default_resource;

	private:
		std::vector<
			std::pair<std::string,
				std::vector<
					std::pair<std::regex, ResourceFunction>
					   >
				 >
			   > opt_resource;
		
	public:
		void start() {
			//Copy the resources to opt_resource for more efficient request processing
			opt_resource.clear();
			for(auto& res: resource) {
				for(auto& res_method: res.second) {
					auto it=opt_resource.end();
					for(auto opt_it=opt_resource.begin();opt_it!=opt_resource.end();opt_it++) {
						if(res_method.first==opt_it->first) {
							it=opt_it;
							break;
						}
					}
					if(it==opt_resource.end()) {
						opt_resource.emplace_back();
						it=opt_resource.begin()+(opt_resource.size()-1);
						it->first=res_method.first;
					}
					it->second.emplace_back(std::regex(res.first), res_method.second);
				}
			}
						
			accept(); 
			
			//If num_threads>1, start m_io_service.run() in (num_threads-1) threads for thread-pooling
			threads.clear();
			for(size_t c=1;c<num_threads;c++) {
				threads.emplace_back([this](){
					io_service.run();
				});
			}

			//Main thread
			io_service.run();

			//Wait for the rest of the threads, if any, to finish as well
			for(auto& t: threads) {
				t.join();
			}
		}
		
		void stop() {
			io_service.stop();
		}

	protected:
		asio::io_service io_service;
		asio::ip::tcp::endpoint endpoint;
		asio::ip::tcp::acceptor acceptor;
		size_t num_threads;
		std::vector<std::thread> threads;
		
		size_t timeout_request;
		size_t timeout_content;
		
		ServerBase(
			unsigned short port,
			size_t num_threads,
			size_t timeout_request,
			size_t timeout_send_or_receive
			) :
			endpoint(asio::ip::tcp::v4(), port),
			acceptor(io_service, endpoint),
			num_threads(num_threads), 
			timeout_request(timeout_request),
			timeout_content(timeout_send_or_receive)
		{
		}
		
		virtual void accept()=0;

		shared_ptr<asio::deadline_timer> make_timer(shared_socket socket)
		{
			if (timeout_content > 0)
				return set_timeout_on_socket(socket, timeout_content);
			else
				return nullptr;
		}
		
		shared_ptr<asio::deadline_timer>
		set_timeout_on_socket(
				shared_socket socket,
				size_t seconds,
				shared_ptr<asio::strand> strand = nullptr
				)
		{
			shared_ptr<asio::deadline_timer> timer(new asio::deadline_timer(io_service));
			timer->expires_from_now(boost::posix_time::seconds(seconds));
			auto lambda = [socket](const error_code& ec){
				if(!ec) {
					error_code ec;
					socket->lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
					socket->lowest_layer().close();
				}
			};
			if (!strand)
				timer->async_wait(lambda);
			else 
				timer->async_wait(strand->wrap(lambda));
			return timer;
		}  
		
		void read_request_and_content(shared_socket socket)
		{
			// Create new streambuf (Request::streambuf) for async_read_until()
			// shared_ptr is used to pass temporary objects to the asynchronous functions
			auto request = Request::create();
			request->read_remote_endpoint_data(*socket);

			//Set timeout on the following asio::async-read or write function
			auto timer = make_timer(socket);

			auto lambda = [this, socket, request, timer]
				(const error_code& ec,
				 size_t bytes_transferred)
			{
				if (timeout_request > 0)
					timer->cancel();

				if(ec)
					return ;
				// request->streambuf.size() is not necessarily the same as bytes_transferred, from Boost-docs:
				// "After a successful async_read_until
				// operation, the streambuf may contain
				// additional data beyond the delimiter"
				//
				// The chosen solution is to extract lines from the
				// stream directly when parsing the header.
				// What is left of the streambuf (maybe some
				// bytes of the content) is appended to in the
				// async_read-function below (for retrieving
				// content).

				size_t num_additional_bytes = request->streambuf.size() - bytes_transferred;
				
				parse_request(request, request->content);
				
				//If content, read that as well
				const auto it=request->header.find("Content-Length");
				if (it != request->header.end()) {
					//Set timeout on the following asio::async-read or write function
					auto timer = make_timer(socket);

					try {
						auto lambda = [this, socket, request, timer]
							(const error_code& ec, size_t)
						{
							if (timeout_content > 0)
								timer->cancel();
							if (!ec)
								deliver_request(socket, request);
						};

						/*
						 * FIXME: Crafted Content-Length
						 */
						asio::async_read(*socket,
							request->streambuf, 
							asio::transfer_exactly(stoull(it->second)-num_additional_bytes),
							lambda
						);
					}
					catch(const std::exception& e) {
						std::cerr << e.what() << std::endl;
					}
				}
				else {
					deliver_request(socket, request);
				}
			};
						
			asio::async_read_until(*socket,
				request->streambuf,
				"\r\n\r\n",
				lambda
			);
		}

		void parse_request(RequestPtr request, std::istream& stream) const
		{
			std::string line;
			getline(stream, line);
			size_t method_end = line.find(' ');
			size_t path_end = line.find(' ', method_end + 1);
			if (method_end != std::string::npos && path_end != std::string::npos) {
				request->method = line.substr(0, method_end);
				request->path = line.substr(method_end + 1, path_end - method_end - 1);
				request->http_version = line.substr(path_end + 6, line.size() - path_end - 7);

				while (true) {
					getline(stream, line);
					size_t param_end = line.find(':');
					if (param_end == std::string::npos)
						break;

					size_t value_start = param_end+1;
					if (line[value_start] == ' ')
						value_start++;

					std::string key = line.substr(0, param_end);
					request->header.emplace(key, line.substr(value_start, line.size()-value_start-1));
				}
			}
		}

		void deliver_request(shared_socket socket, shared_ptr<Request> request) {
			//Find path- and method-match, and call write_response
			for(auto& res: opt_resource) {
				if(request->method==res.first) {
					for(auto& res_path: res.second) {
						std::smatch sm_res;
						if(std::regex_match(request->path, sm_res, res_path.first)) {
							request->path_match=std::move(sm_res);
							write_response(socket, request, res_path.second);
							return;
						}
					}
				}
			}
			auto it_method=default_resource.find(request->method);
			if(it_method!=default_resource.end()) {
				write_response(socket, request, it_method->second);
			}
		}
		
		void write_response(shared_socket socket, shared_ptr<Request> request, ResourceFunction& resource_function)
		{
			shared_ptr<asio::strand> strand(new asio::strand(io_service));

			//Set timeout on the following asio::async-read or write function
			auto timer = make_timer(socket);

			asio::spawn(*strand,
				[this, strand, &resource_function, socket, request, timer](asio::yield_context yield)
				{
					ResponsePtr response(new Response(io_service, socket, strand, yield));
					boost::future<int> p;

					try {
						p = resource_function(response, request);
					}
					catch(std::exception& e) {
						return;
					}

					auto flush_lambda = [this, socket, request, timer](const error_code& ec)
					{
						if(timeout_content>0)
							timer->cancel();

						if(!ec && stof(request->http_version)>1.05)
							read_request_and_content(socket);
					};

					fprintf(stderr, "Resp %p\n", &*response);
					response->future = p.then(
							[response, flush_lambda](boost::future<int> f)
							{
								fprintf(stderr, "Then %d\n", f.get());
								response->async_flush(flush_lambda);
							}
						);
				});
		}
	};
	
	template<class socket_type>
	class Server : public ServerBase<socket_type> {};
	
	typedef asio::ip::tcp::socket HTTP;
	
	template<>
	class Server<HTTP> : public ServerBase<HTTP> {
	public:
		Server(unsigned short port,
		       size_t num_threads=1,
		       size_t timeout_request=5,
		       size_t timeout_content=300
		       ) : 
			ServerBase<HTTP>::ServerBase(port,
					             num_threads,
						     timeout_request,
						     timeout_content)
		{}
		
	private:
		void accept() {
			//Create new socket for this connection
			//Shared_ptr is used to pass temporary objects to the asynchronous functions
			auto socket = make_shared<HTTP>(io_service);
						
			acceptor.async_accept(*socket, [=](const error_code& ec){
				//Immediately start accepting a new connection
				accept();

				if(!ec) {
					asio::ip::tcp::no_delay option(true);
					socket->set_option(option);
					read_request_and_content(socket);
				}
			});
		}
	};
}
#endif	/* SERVER_HTTP_HPP */
