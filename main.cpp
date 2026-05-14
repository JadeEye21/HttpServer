// #include "hdelibc.hpp"
#include <stdio.h>
// #include "Servers/TestServer.hpp"
// #include "Servers/MultiClientServer.hpp"
#include "SingleFileHttpServer.hpp"
#include "Redis/RedisMainServer.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>


int main(){
    // std::cout<<"Starting socket...."<<std::endl;
    // std::cout<<"Binding socket...."<<std::endl;
    // HDE::BindingSocket bs = HDE::BindingSocket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY);
    // std::cout<<"Listening socket...."<<std::endl;
    // HDE::ListeningSocket ls = HDE::ListeningSocket(AF_INET, SOCK_STREAM, 0, 81, INADDR_ANY, 10);
    // std::cout<<"Success \n";
    // HDE::MultiClientServer mcs;
    // hdcpp::SingleFileHttpServer sfhs(4444);

    hdcpp::RedisMainServer rms;

    const std::vector<std::pair<const char*, std::string>> cases = {
        // --- Basic Types ---
        {"simple string", "+OK\r\n"},
        {"error string", "-Error message\r\n"},
        {"integer: zero", ":0\r\n"},
        {"integer: positive", ":1000\r\n"},
        {"integer: negative", ":-42\r\n"},
        
        // --- Bulk Strings ---
        {"bulk: empty", "$0\r\n\r\n"},
        {"bulk: null", "$-1\r\n"},
        {"bulk: with interior CRLF", "$4\r\n\r\n\r\n\r\n"}, // A string containing just \r\n
        {"bulk: long string", "$13\r\nHello\r\nWorld!\r\n"},

        // --- Arrays ---
        {"array: empty", "*0\r\n"},
        {"array: null", "*-1\r\n"},
        {"array: mixed types", "*3\r\n:1\r\n:2\r\n:3\r\n"},
        {"array: nested", "*2\r\n*2\r\n:1\r\n:2\r\n+OK\r\n"}, // [ [1, 2], "OK" ]

        // --- RESP3 (Optional but recommended) ---
        {"resp3: null", "_\r\n"},
        {"resp3: boolean true", "#t\r\n"},
        {"resp3: boolean false", "#f\r\n"},
        {"resp3: double", ",3.14\r\n"},

        // --- Truncated Data ---
        {"truncated: type only", "*"},
        {"truncated: length only", "$5\r\nHel"},
        {"truncated: missing final CRLF", "+OK\r"},

        // --- Malformed Lengths ---
        {"malformed: non-numeric length", "$five\r\nhello\r\n"},
        {"malformed: negative length (not -1)", "$-5\r\nhello\r\n"},
        {"malformed: array length mismatch", "*2\r\n:1\r\n"}, // Claims 2, gives 1

        // --- Protocol Violations ---
        {"violation: invalid type prefix", "!Hello\r\n"},
        {"violation: bulk string length mismatch", "$3\r\nHello\r\n"}, // Length 3 but data is 5
        {"violation: extra data after CRLF", ":100\r\nextra"},
        
        // --- Integer Overflows ---
        {"overflow: larger than int64", ":9223372036854775808\r\n"}
    };

    for (const auto& [name, input] : cases) {
        std::cout << "\n=== " << name << " ===\n";
        std::cout << "bytes=" << input.size() << "\n";
        rms.read_input(input);
    }

    // std::ifstream file("test.txt");
    // std::string line;
    // if (file.is_open()) {
    //     // Use a loop to process the entire file
    //     while (std::getline(file, line)) {
    //         // Note: std::getline removes the \n, but keeps the \r if present
    //         if (!line.empty()) {
    //             rms.read_input(line);
    //         }
    //     }
    // } else {
    //     printf("Failed to open file.\n");
    // }

    return 0;

}