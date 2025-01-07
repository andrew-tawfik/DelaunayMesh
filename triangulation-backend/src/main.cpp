#include "mesh.h"
#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <memory>
#include <map>

typedef websocketpp::server<websocketpp::config::asio> server;

// Map to associate each client with their own mesh
std::map<websocketpp::connection_hdl, Mesh, std::owner_less<websocketpp::connection_hdl>> client_meshes;

void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg)
{

    try
    {
        auto received_message = nlohmann::json::parse(msg->get_payload());
        std::string action = received_message["action"];

        auto& client_mesh = client_meshes[hdl];
        if (action == "add_point")
        {
            float x = received_message["data"]["x"];
            float y = received_message["data"]["y"];
            client_mesh.triangulatePoint(x, y);

            nlohmann::json updated_mesh_json = client_mesh;
            s->send(hdl, updated_mesh_json.dump(), websocketpp::frame::opcode::text);
        }
    }
    catch (websocketpp::exception const & e)
    {
        std::cout << "Send failed: " << e.what() << std::endl;
    }
}

// Handle when a client connects
void on_open(server* s, websocketpp::connection_hdl hdl) {
    std::cout << "Client connected: " << hdl.lock().get() << std::endl;
    // Initialize a new mesh for the client
    client_meshes[hdl] = Mesh();
}

// Handle when a client disconnects
void on_close(server* s, websocketpp::connection_hdl hdl) {
    std::cout << "Client disconnected: " << hdl.lock().get() << std::endl;
    // Remove the client's mesh
    client_meshes.erase(hdl);
}

int main(int argc, char *argv[])
{
    server mesh_server;
    try
    {
        mesh_server.init_asio();
        mesh_server.set_message_handler(std::bind(&on_message, &mesh_server, std::placeholders::_1, std::placeholders::_2));
        mesh_server.set_open_handler(std::bind(&on_open, &mesh_server, std::placeholders::_1));
        mesh_server.set_close_handler(std::bind(&on_close, &mesh_server, std::placeholders::_1));
        mesh_server.listen(9002);
        mesh_server.start_accept();
        mesh_server.run();
    }
    catch (websocketpp::exception const & e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
