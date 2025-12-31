#include "mesh.h"
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// Session represents a single WebSocket connection
class session : public std::enable_shared_from_this<session>
{
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;
    Mesh mesh_;
    
public:
    explicit session(tcp::socket&& socket)
        : ws_(std::move(socket))
        , mesh_()
    {
    }
    
    void run()
    {
        net::dispatch(ws_.get_executor(),
            beast::bind_front_handler(&session::on_run, shared_from_this()));
    }
    
    void on_run()
    {
        // Set suggested timeout settings for the websocket
        ws_.set_option(websocket::stream_base::timeout::suggested(
            beast::role_type::server));
        
        // Set a decorator to change the Server of the handshake
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(beast::http::field::server, "DelaunayMesh");
            }));
        
        // Accept the websocket handshake
        ws_.async_accept(
            beast::bind_front_handler(&session::on_accept, shared_from_this()));
    }
    
    void on_accept(beast::error_code ec)
    {
        if (ec)
        {
            std::cerr << "Accept error: " << ec.message() << std::endl;
            return;
        }
        
        // Read a message
        do_read();
    }
    
    void do_read()
    {
        ws_.async_read(
            buffer_,
            beast::bind_front_handler(&session::on_read, shared_from_this()));
    }
    
    void on_read(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);
        
        // This indicates that the session was closed
        if (ec == websocket::error::closed)
            return;
        
        if (ec)
        {
            std::cerr << "Read error: " << ec.message() << std::endl;
            return;
        }
        
        try
        {
            // Convert buffer to string
            std::string message = beast::buffers_to_string(buffer_.data());
            auto received_message = nlohmann::json::parse(message);
            std::string action = received_message["action"];
            
            if (action == "add_point")
            {
                float x = received_message["data"]["x"];
                float y = received_message["data"]["y"];
                mesh_.triangulatePoint(x, y);
                
                // Serialize mesh to JSON
                nlohmann::json updated_mesh_json = mesh_;
                std::string response = updated_mesh_json.dump();
                
                // Set text mode for the message
                ws_.text(true);
                
                // Clear buffer and write response
                buffer_.consume(buffer_.size());
                beast::ostream(buffer_) << response;
                
                // Send the response
                ws_.async_write(
                    buffer_.data(),
                    beast::bind_front_handler(&session::on_write, shared_from_this()));
                return;
            }
        }
        catch (std::exception const& e)
        {
            std::cerr << "Message processing error: " << e.what() << std::endl;
        }
        
        // Clear the buffer
        buffer_.consume(buffer_.size());
        
        // Read another message
        do_read();
    }
    
    void on_write(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);
        
        if (ec)
        {
            std::cerr << "Write error: " << ec.message() << std::endl;
            return;
        }
        
        // Clear the buffer
        buffer_.consume(buffer_.size());
        
        // Read another message
        do_read();
    }
};

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    
public:
    listener(net::io_context& ioc, tcp::endpoint endpoint)
        : ioc_(ioc)
        , acceptor_(net::make_strand(ioc))
    {
        beast::error_code ec;
        
        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if (ec)
        {
            std::cerr << "Open error: " << ec.message() << std::endl;
            return;
        }
        
        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec)
        {
            std::cerr << "Set option error: " << ec.message() << std::endl;
            return;
        }
        
        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec)
        {
            std::cerr << "Bind error: " << ec.message() << std::endl;
            return;
        }
        
        // Start listening for connections
        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if (ec)
        {
            std::cerr << "Listen error: " << ec.message() << std::endl;
            return;
        }
    }
    
    // Start accepting incoming connections
    void run()
    {
        do_accept();
    }
    
private:
    void do_accept()
    {
        // The new connection gets its own strand
        acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(&listener::on_accept, shared_from_this()));
    }
    
    void on_accept(beast::error_code ec, tcp::socket socket)
    {
        if (ec)
        {
            std::cerr << "Accept error: " << ec.message() << std::endl;
        }
        else
        {
            // Create the session and run it
            std::make_shared<session>(std::move(socket))->run();
        }
        
        // Accept another connection
        do_accept();
    }
};

int main(int argc, char* argv[])
{
    try
    {
        auto const address = net::ip::make_address("0.0.0.0");
        auto const port = static_cast<unsigned short>(9002);
        
        // The io_context is required for all I/O
        net::io_context ioc{1};
        
        // Create and launch a listening port
        std::make_shared<listener>(ioc, tcp::endpoint{address, port})->run();
        
        std::cout << "WebSocket server running on port " << port << std::endl;
        
        // Run the I/O service on the main thread
        ioc.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}