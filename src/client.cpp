#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include "../lib/httplib.hpp"
#include "../lib/json.hpp"

using json = nlohmann::json;

//pure virtual - abstract class
class KRLProcessor {
public:
    virtual void sortByDeparture() = 0; 
    virtual void sortByDuration() = 0;  
    virtual std::string getFormattedResult() = 0;
    virtual ~KRLProcessor() {}
};

//inheritance
class Schedule : public KRLProcessor {
public:
    std::vector<std::tuple<std::string, std::string, int>> vec;

    Schedule(json data) {
        if (data.is_array()) {
            for (auto& item : data) {
                std::string id = item.value("trainId", "");
                std::string depart = item.value("departTime", "00:00");
                int duration = item.value("durationMin", 0);
                vec.push_back({id, depart, duration});
            }
        }
    }

//bubble sorting
    void sortByDeparture() override {
        int n = vec.size();
        for (int i = 0; i < n - 1; i++) {
            bool swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                if (std::get<1>(vec[j]) > std::get<1>(vec[j + 1])) { 
                    std::swap(vec[j], vec[j + 1]); 
                    swapped = true;
                }
            }
            if (!swapped) break;
        }
    }

    void sortByDuration() override {
        int n = vec.size();
        for (int i = 0; i < n - 1; i++) {
            bool swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                if (std::get<2>(vec[j]) > std::get<2>(vec[j + 1])) { 
                    std::swap(vec[j], vec[j + 1]); 
                    swapped = true;
                }
            }
            if (!swapped) break;
        }
    }

//searching
    std::string getFormattedResult() override {
        std::stringstream ss;
        if (vec.empty()) {
            return "<div class='no-result'>Tidak ada jadwal kereta yang ditemukan</div>";
        }
        
        ss << "<div class='result-grid'>";
        int count = 1;
        for (const auto& item : vec) {
            std::string id = std::get<0>(item);
            std::string depart = std::get<1>(item);
            int durationMin = std::get<2>(item);

            int h = durationMin / 60;
            int m = durationMin % 60;
            
            ss << "<div class='train-card'>";
            ss << "  <div class='train-id'>🚂 " << id << " (#" << count++ << ")</div>";
            ss << "  <div class='train-info'>";
            ss << "    <div>Berangkat: " << depart << "</div>";
            ss << "    <div>Durasi: ";
            if (h > 0) ss << h << "j " << m << "m";
            else ss << m << "m";
            ss << "    </div>";
            ss << "  </div>";
            ss << "</div>";
        }
        ss << "</div>";
        return ss.str();
    }
};

int main() {
    httplib::Server svr;
    httplib::Client cli("http://0.0.0.0:8080"); 

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        if (std::ifstream i("src/index.html"); i.is_open()) {
            std::stringstream indexHTML;
            i >> indexHTML.rdbuf();
            i.close();

            res.set_content(indexHTML.str(), "text/html");
        } else res.set_content("Not found", "text/html");
    });

    svr.Get("/get-stations", [&](const httplib::Request&, httplib::Response& res) {
        if (auto sRes = cli.Get("/stations")) {
            res.status = sRes->status;
            res.set_content(sRes->body, "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"error\":\"Gagal terhubung\"}", "application/json");
        }
    });

    svr.Get("/get-schedule", [&](const httplib::Request& req, httplib::Response& res) {
        std::string url = "/schedule?from=" + req.get_param_value("from") +
                          "&to=" + req.get_param_value("to") +
                          "&timeFrom=" + req.get_param_value("timefrom") +
                          "&timeTo=" + req.get_param_value("timeto");
        
        auto sRes = cli.Get(url);
        if (sRes && sRes->status == 200) {
            json rawData = json::parse(sRes->body);
            //dynamic binding
            KRLProcessor* processor = new Schedule(rawData);
            
            std::string sort = req.get_param_value("sort");
            if (sort == "departure") processor->sortByDeparture();
            else if (sort == "duration") processor->sortByDuration();

            res.set_content(processor->getFormattedResult(), "text/html");
            delete processor;
        } else {
            res.status = sRes ? sRes->status : 500;
            res.set_content("Gagal mengambil data.", "text/plain");
        }
    });

    std::cout << "Server aktif di http://localhost:3000" << std::endl;
    svr.listen("0.0.0.0", 3000);
    return 0;
}

//buat windows
#ifdef _WIN32
#include <windows.h>
extern "C" int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main();
}
#endif