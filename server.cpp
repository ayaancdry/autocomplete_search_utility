#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <chrono>
#include "trie.h"

using namespace std;

const int PORT = 8080;
const string WEB_DIR = "./web/";

Trie trie;

void respond(int client_socket, const string& response) {
    send(client_socket, response.c_str(), response.size(), 0);
}

string get_file_content(const string& file_path) {
    ifstream file(WEB_DIR + file_path);
    if (!file) {
        cerr << "Error: File not found - " << WEB_DIR + file_path << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

string get_query_param(const string& request, const string& param) {
    size_t start = request.find(param + "=");
    if (start == string::npos) return "";
    start += param.length() + 1;
    size_t end = request.find("&", start);
    if (end == string::npos) end = request.find(" ", start);
    return request.substr(start, end - start);
}

bool add_word_to_dictionary(const string& word) {
    if (word.empty()) return false;
    
    ofstream file(WEB_DIR + "dictionary.txt", ios::app);
    if (!file) return false;
    
    file << word << "\n";
    file.close();
    return true;
}

void handle_request(int client_socket) {
    char buffer[1024];
    recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    buffer[sizeof(buffer) - 1] = '\0';

    string request(buffer);
    cout << "Received request: " << request << endl;

    if (request.find("POST /add-word") == 0) {
        string word = get_query_param(request, "word");
        if (!word.empty()) {
            if (add_word_to_dictionary(word)) {
                trie.insert(word);
                string response = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: application/json\r\n"
                                "Access-Control-Allow-Origin: *\r\n"
                                "Content-Length: 27\r\n\r\n"
                                "{\"status\": \"word_added\"}";
                respond(client_socket, response);
            } else {
                string response = "HTTP/1.1 500 Internal Server Error\r\n"
                                "Content-Type: application/json\r\n"
                                "Access-Control-Allow-Origin: *\r\n"
                                "Content-Length: 29\r\n\r\n"
                                "{\"status\": \"add_failed\"}";
                respond(client_socket, response);
            }
        }
    }
    else if (request.find("GET ") == 0) {
        size_t pos = request.find("GET ") + 4;
        size_t end_pos = request.find(" HTTP/1.1");
        if (end_pos == string::npos) return;
        string path = request.substr(pos, end_pos - pos);

        if (path.find("/suggestions") == 0) {
            string query = get_query_param(request, "query");
            if (query.empty()) {
                string not_found_response = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
                respond(client_socket, not_found_response);
                close(client_socket);
                return;
            }

            // Start timing
            auto start = chrono::high_resolution_clock::now();

            vector<pair<string, int> > results = trie.autocomplete(query);

            // End timing
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

            vector<string> suggestions;
            for (const auto& p : results) suggestions.push_back(p.first);

            // Build JSON with suggestions and time
            string json_response = "{";
            json_response += "\"suggestions\":[";
            for (size_t i = 0; i < suggestions.size(); ++i) {
                json_response += "\"" + suggestions[i] + "\"";
                if (i < suggestions.size() - 1) json_response += ",";
            }
            json_response += "],";
            json_response += "\"search_time_us\":" + to_string(duration);
            json_response += "}";

            string response = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: application/json\r\n"
                            "Access-Control-Allow-Origin: *\r\n"
                            "Content-Length: " + to_string(json_response.size()) + "\r\n\r\n" + json_response;
            respond(client_socket, response);
        } else {
            if (path == "/") path = "index.html";
            string content = get_file_content(path);
            if (content.empty()) {
                string not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
                respond(client_socket, not_found_response);
            } else {
                string response = "HTTP/1.1 200 OK\r\n"
                                "Content-Length: " + to_string(content.size()) + "\r\n\r\n" + content;
                respond(client_socket, response);
            }
        }
    }
    close(client_socket);
}

int main() {
    loadWordsFromFile(trie, WEB_DIR + "dictionary.txt");

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "Error opening socket" << endl;
        return 1;
    }

    // Only use SO_REUSEADDR
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "Error setting socket options" << endl;
        return 1;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (::bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error on binding" << endl;
        return 1;
    }

    listen(server_socket, 5);
    cout << "Listening on port " << PORT << endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            cerr << "Error on accept" << endl;
            continue;
        }
        handle_request(client_socket);
    }

    close(server_socket);
    return 0;
}