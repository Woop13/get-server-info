#include <boost/asio/io_service.hpp>
#include <boost/asio.hpp>

#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>
#include <stdexcept>
#include <ranges>

constexpr auto array_buffer_size{ 1024uz };

std::string exportData(const std::string& addr, unsigned short port) {
    try {
        boost::asio::io_service service;
        boost::asio::ip::tcp::socket socket{ service };

        boost::asio::ip::tcp::endpoint endpoint{ boost::asio::ip::address::from_string(addr), port };
        socket.connect(endpoint);

        if (socket.is_open())
            std::cout << "Socket Connected" << std::endl;
        else
            std::cout << "Failed" << std::endl;

        // Prepare the packet to send to the server (based on a reverse-engineered packet structure)
        std::string packet{ "\x00\x83\x00\x0d\x00\x00\x00\x00\x00?status\x00", sizeof("\x00\x83\x00\x0d\x00\x00\x00\x00\x00?status\x00") - 1};

        auto packet_length = sizeof("\x00\x83\x00\x0d\x00\x00\x00\x00\x00?status\x00") - 1;
        auto data_sent = 0uz;
        
        try
        {
            while (data_sent < packet_length)
            {
                auto bytes_sent = socket.write_some(boost::asio::buffer(packet.substr(data_sent), packet_length - data_sent));

                std::cout << std::format("Bytes sent: {}\n", bytes_sent);
                data_sent += bytes_sent;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
            std::terminate();
        }

        std::cout << std::format("Packet have been sent\n");

        std::array<char, array_buffer_size> buff{};

        auto bytes_read = socket.read_some(boost::asio::buffer(buff));
        std::cout << std::format("Bytes read: {}\n", bytes_read);
        std::cout << std::format("Packet received\n");

        socket.close();
        std::cout << std::format("Socket closed\n");

        if (buff[0] == '\x00' && buff[1] == '\x83')
        {
            std::cout << "Packet verified, reading data\n";

            /*
            std::array<int, 2> parse_result{};
            parse_result[0] = static_cast<unsigned char>(buff[2]);
            parse_result[1] = static_cast<unsigned char>(buff[3]);

            std::cout << std::format("Parse result 2 and 3: {} {}", parse_result[0], parse_result[1]);

            if (buff[4] == '\x2a')
            {

            }
            */

            std::size_t size = static_cast<unsigned char>(buff[3]) - 1;
            std::string result_string{};
            result_string.reserve(size);

            for (int index{5}; size > 0; index++, size--)
                result_string += buff[index];

            return result_string;
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return "ERROR";
    }
}

int main() {
    std::string addr = "43.248.187.124";
    constexpr unsigned short port = 43319; // Replace with your server's port
    std::string query = "?status";

    auto data_string = exportData(addr, port);

    auto view = data_string
                    | std::views::split('&')
                    | std::views::transform( [](auto word){ return std::string(word.begin(), word.end()); } );

    std::vector<std::string> words(view.begin(), view.end());

    for (auto word : words)
    {
        std::cout << word << " ";
    }

    return 0;
}
