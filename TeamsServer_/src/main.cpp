#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        std::cout << "Boost.Asio server running on port 12345..." << std::endl;

        acceptor.async_accept([](const boost::system::error_code& ec, tcp::socket socket){
            if (!ec) {
                std::cout << "New connection accepted." << std::endl;
            }
        });

        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
