#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <curl/curl.h>
#include <rapidjson/document.h>

// callback for curl
size_t write_data(void* ptr, size_t size, size_t nmemb, void* userdata) {
    ((std::string*)userdata)->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

// call the API and get the raw JSON text for a node
std::string get_json(const std::string& node) {
    std::string out;
    std::string url = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/" + node;

    CURL* c = curl_easy_init();
    curl_easy_setopt(c, CURLOPT_URL, url.c_str());
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &out);
    curl_easy_perform(c);
    curl_easy_cleanup(c);
    return out;
}

// get the neighbor names from the JSON response
std::vector<std::string> get_neighbors(const std::string& node) {
    std::vector<std::string> n;
    std::string j = get_json(node);

    rapidjson::Document d;
    d.Parse(j.c_str());

    // bail if response is not a JSON object
    if (!d.IsObject()) return n;

    // print error if node not found
    if (d.HasMember("error")) {
        std::cerr << "Error: " << d["error"].GetString() << "\n";
        return n;
    }

    // if JSON has "neighbors" list, copy into vector
    if (d.HasMember("neighbors") && d["neighbors"].IsArray()) {
        for (auto& v : d["neighbors"].GetArray())
            if (v.IsString()) n.push_back(v.GetString());
    }
    return n;
}

// do a breadth-first search to given depth
std::set<std::string> bfs(const std::string& start, int depth) {
    std::set<std::string> seen;                       
    std::queue<std::pair<std::string,int>> q;         

    seen.insert(start);
    q.push({start,0});

    while(!q.empty()) {
        auto p = q.front(); q.pop();
        if (p.second >= depth) continue;             
        for (auto& nb : get_neighbors(p.first)) {
            if (!seen.count(nb)) {                   
                seen.insert(nb);
                q.push({nb,p.second+1});
            }
        }
    }
    return seen;
}

int main(int argc, char* argv[]) {
    // need the starting node name and the depth number
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " \"Node_Name\" depth\n";
        return 1;
    }
    std::string start = argv[1];
    int d = std::stoi(argv[2]);

    // run the search and print results
    auto res = bfs(start, d);
    for (auto& x : res) std::cout << x << "\n";
    return 0;
}