#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <curl/curl.h>
#include <rapidjson/document.h>

using namespace std;

size_t write_data(void* ptr, size_t size, size_t nmemb, void* userdata) {
    string* s = (string*)userdata;
    s->append((char*)ptr, size*nmemb);
    return size*nmemb;
}

string encode(string s) {
    for (char &c : s) if (c==' ') c='_';
    return s;
}

string get_json(string node) {
    string out;
    string url = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/" + encode(node);
    CURL* c = curl_easy_init();
    curl_easy_setopt(c, CURLOPT_URL, url.c_str());
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &out);
    curl_easy_perform(c);
    curl_easy_cleanup(c);
    return out;
}

vector<string> get_neighbors(string node) {
    vector<string> n;
    string j = get_json(node);
    rapidjson::Document d;
    d.Parse(j.c_str());
    if (d.HasMember("neighbors")) {
        for (auto& v : d["neighbors"].GetArray()) n.push_back(v.GetString());
    }
    return n;
}

set<string> bfs(string start, int depth) {
    set<string> seen;
    queue<pair<string,int>> q;
    seen.insert(start);
    q.push({start,0});
    while (!q.empty()) {
        auto p = q.front(); q.pop();
        if (p.second>=depth) continue;
        for (string nb : get_neighbors(p.first))
            if (!seen.count(nb)) {
                seen.insert(nb);
                q.push({nb,p.second+1});
            }
    }
    return seen;
}

int main(int argc, char* argv[]) {
    if (argc<3) { cerr<<"Usage: "<<argv[0]<<" Node_Name depth\n"; return 1; }
    string start = argv[1];
    int d = stoi(argv[2]);
    auto res = bfs(start,d);
    for (string x : res) cout << x << "\n";
}