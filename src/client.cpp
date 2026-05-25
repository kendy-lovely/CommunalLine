#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include "httplib.hpp"
#include "json.hpp"

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
            for (int j = 0; j < n - i - 1; j++) {
                if (std::get<1>(vec[j]) > std::get<1>(vec[j + 1])) { 
                    std::swap(vec[j], vec[j + 1]); 
                }
            }
        }
    }

    void sortByDuration() override {
        int n = vec.size();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (std::get<2>(vec[j]) > std::get<2>(vec[j + 1])) { 
                    std::swap(vec[j], vec[j + 1]); 
                }
            }
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

const std::string WEB_LAYOUT = R"html(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>KRL Tracker</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: sans-serif; }
        body { background: #f4f4f4; padding: 20px; color: #333; }
        .container { max-width: 700px; margin: 0 auto; background: white; border: 1px solid #ccc; }
        .header { background: #333; color: white; padding: 20px; text-align: center; }
        .header h1 { font-size: 24px; }
        .header p { font-size: 12px; margin-top: 5px; color: #aaa; }
        .form-section { padding: 20px; }
        .form-group { margin-bottom: 15px; }
        .form-group label { display: block; font-size: 13px; font-weight: bold; margin-bottom: 5px; }
        select, input { width: 100%; padding: 10px; font-size: 14px; border: 1px solid #ccc; background: #fff; }
        .time-inputs { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }
        button { width: 100%; padding: 12px; font-size: 14px; font-weight: bold; cursor: pointer; border: 1px solid #333; }
        .search-btn { background: #333; color: white; }
        .search-btn:hover { background: #444; }
        #output { padding: 20px; border-top: 1px solid #ccc; background: #fafafa; }
        .result-header { font-size: 16px; font-weight: bold; margin-bottom: 15px; padding-bottom: 5px; border-bottom: 1px solid #ccc; }
        .result-grid { display: grid; gap: 10px; }
        .train-card { background: white; border: 1px solid #ccc; padding: 15px; }
        .train-id { font-size: 15px; font-weight: bold; margin-bottom: 5px; }
        .train-info { font-size: 13px; color: #555; display: flex; gap: 20px; }
        .no-result { text-align: center; padding: 30px; color: #666; font-size: 14px; border: 1px solid #ccc; background: white; }
        .sort-section { margin-top: 15px; padding-top: 15px; border-top: 1px dashed #ccc; }
        .sort-header { font-size: 13px; font-weight: bold; margin-bottom: 10px; }
        .sort-buttons { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
        .sort-btn { background: white; color: #333; }
        .sort-btn:hover { background: #eee; }
        .loading { text-align: center; padding: 20px; color: #666; font-size: 14px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>KRL Tracker</h1>
            <p>Sistem Pelacakan Jadwal Commuter Line</p>
        </div>
        
        <div class="form-section">
            <div class="form-group">
                <label>Stasiun Asal</label>
                <select id="from"></select>
            </div>
            <div class="form-group">
                <label>Stasiun Tujuan</label>
                <select id="to"></select>
            </div>
            <div class="time-inputs">
                <div class="form-group">
                    <label>Dari Jam (0-23)</label>
                    <input type="number" id="tf" min="0" max="23" value="7">
                </div>
                <div class="form-group">
                    <label>Hingga Jam (0-23)</label>
                    <input type="number" id="tt" min="0" max="23" value="9">
                </div>
            </div>
            <button class="search-btn" onclick="proses('none')">CARI JADWAL</button>
        </div>
        
        <div id="output" style="display:none;">
            <div class="result-header">Hasil Pencarian</div>
            <div id="resList"></div>
            
            <div class="sort-section">
                <div class="sort-header">Urutkan Berdasarkan:</div>
                <div class="sort-buttons">
                    <button class="sort-btn" onclick="proses('departure')">Waktu Terawal</button>
                    <button class="sort-btn" onclick="proses('duration')">Durasi Tercepat</button>
                </div>
            </div>
        </div>
    </div>

    <script>
        async function load() {
            try {
                let r = await fetch('/get-stations'); 
                let d = await r.json();
                let f = document.getElementById('from'), t = document.getElementById('to');
                f.innerHTML = ""; t.innerHTML = "";
                for (let [id, name] of Object.entries(d)) {
                    f.options.add(new Option(`${name} (${id})`, id)); 
                    t.options.add(new Option(`${name} (${id})`, id));
                }
            } catch (err) {
                alert("Gagal memuat daftar stasiun.");
            }
        }
        
        async function proses(sort) {
            let f = document.getElementById('from').value; 
            let t = document.getElementById('to').value;
            let tf = document.getElementById('tf').value; 
            let tt = document.getElementById('tt').value;
            
            if (!f || !t) {
                alert("Pilih stasiun asal dan tujuan!");
                return;
            }
            
            let timeFrom = tf.padStart(2, '0') + ":00";
            let timeTo = tt.padStart(2, '0') + ":00";
            
            document.getElementById('resList').innerHTML = '<div class="loading">Memuat data...</div>';
            document.getElementById('output').style.display = 'block';
            
            try {
                let r = await fetch(`/get-schedule?from=${f}&to=${t}&timefrom=${timeFrom}&timeto=${timeTo}&sort=${sort}`);
                document.getElementById('resList').innerHTML = await r.text();
            } catch (err) {
                document.getElementById('resList').innerHTML = '<div class="no-result">Gagal mengambil data.</div>';
            }
        }
        window.onload = load;
    </script>
</body>
</html>
)html";

int main() {
    httplib::Server svr;
    httplib::Client cli("http://127.0.0.1:8080"); 

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(WEB_LAYOUT, "text/html");
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