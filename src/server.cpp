#define API_TOKEN "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJhdWQiOiIzIiwianRpIjoiYmE0Yzc4MzE4ODNjYTI0N2YzMTBkMTJhYzc3ZjE5ZTdjMTVkNjgxOTk2ODM0MDc0MGM3MzliYmRjNGQ3YTI5MzczYzMyNWM2NDFiZjgxYzciLCJpYXQiOjE3NTQ0NTkxMjYsIm5iZiI6MTc1NDQ1OTEyNiwiZXhwIjoxNzg1OTk1MTI2LCJzdWIiOiI1Iiwic2NvcGVzIjpbXX0.zPA0IDAN3NycMKa6DaOdRmkcFz1oUTX1dkxEp3MLBlhibTQI0L0WB9mY-pUlQW5vQj8ktOdo-rRvrjxiXaHFqLQM6ebONbqTg8V0AjBXwrkBjLZDCE4dop9iZyDXcG2b9XTLCgPgpOBbduW_Dy0-bIkJOOIgIzl9mEEUVQf3T6G_zA796SGJ6rtLqfBK-sMnhOV4eZSqQIXIrxPyCJ8SA893p-29PFxfQfcbXW_6cYBFhDzyiilhJ6xQd6znN2eWOL4MPAxYeS2ZGnaZ7ijUN91MAyPnV0dQU7loVtS1jt2HlM5oMSsE2Zoz6FP31GvG6f7o_MWogEp0ZMOus50bVly3II8Rjjc4IGgswbw0h-RS0Ipo3f2QmXp4GfhRNUoTyqq-7oiCIDPUJcdg39lSIy9Fz7-ECNfbjEiH60V3GyftuiFGrayMoE7XeWaC9wQZo3fLHhI1aPgbXXsP-rqWLFf2km4zdG5Y5CYpUNb_Z11VOU6aaFCdRtoC6e7VcxHxLwCBT22wluNpbfFtEQSYDQE1JlegijvFmnRHTM88n-zp7sWhuCWVX6oE0ULdy51SR4iOqpYOA4B1ZymmYrQz1kBxSA_52lnTBlU9gfWkUiFX8GLSh7wQ8a4dVMYoJj6t1VCJt9-d30jn4S3tXsim_3wpp71RE9SSazV35j8o7do"
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstddef>
#include <algorithm>
#include <memory>
#include <set>
#include <unordered_map>
#include "../lib/httplib.hpp"
#include "../lib/json.hpp"

using namespace std;
using namespace nlohmann;
using namespace httplib;

class Saveable {
    public:
        virtual void saveToJson() = 0;
        virtual json getFromJson() = 0;
        virtual ~Saveable() {};
};

namespace Time {
    int timeToMinutes(string hhmmss) {
        int h = 0, m = 0, s = 0;
        char colon;
        stringstream ss(hhmmss);
        int colons = count(hhmmss.begin(), hhmmss.end(), ':');

        if (colons == 1)
            ss >> h >> colon >> m;
        else if (colons == 2)
            ss >> h >> colon >> m >> colon >> s;
        else return -1;

        if (h > 23 || m > 59 || s > 59 || h < 0 || m < 0 || s < 0) 
            return -1;

        return h * 60 + m + (s >= 30 ? 1 : 0);
    }

    string minutesToTime(int m) {
        stringstream ss;
        m = m < 0 ? 0 : m;

        int h = m / 60;
        m = m % 60;
        ss << std::setfill('0') << std::setw(2) << h << ":" << std::setw(2) << m;
        
        return ss.str();
    }

    string minutesToEnglish(int m) {
        stringstream ss;
        int h = m / 60;
        m = m % 60;

        if (h > 0)
            ss << h << " hours and " << m << " minutes";
        else ss << m << " minutes";
        
        return ss.str();
    }

    int subtractTime(string from, string to) {
        return timeToMinutes(to) - timeToMinutes(from);
    }
}

namespace KRL {
    class Website {
        private:
            Client cli;
            Headers headers;
            Website() : 
                cli("https://api-partner.krl.co.id"), 
                headers({{ "Authorization", API_TOKEN }}) {}
        public:
            static Website &instantiate() {
                static Website inst;
                return inst;
            }

            Result callApi(string path) {
                return cli.Get("/krl-webs/v1" + path, headers);
            }

            ~Website() { cli.stop(); }
    };  

    enum Line {
        BOGOR,
        CIKARANG,
        RANGKASBITUNG,
        TANJUNGPRIUK,
        TANGERANG,
        CGK,
        NONE
    };

    string lineToString(const Line& l) {
        switch (l) {
            case BOGOR:         return "BOGOR";
            case CIKARANG:      return "CIKARANG";
            case RANGKASBITUNG: return "RANGKASBITUNG";
            case TANJUNGPRIUK:  return "TANJUNGPRIUK";
            case TANGERANG:     return "TANGERANG";
            case CGK:           return "CGK";
            default:            return "NONE";
        }
    }

    Line strToLine(std::string str) {
        if (str == "BOGOR")         return BOGOR;
        if (str == "CIKARANG")      return CIKARANG;
        if (str == "RANGKASBITUNG") return RANGKASBITUNG;
        if (str == "TANJUNGPRIUK")  return TANJUNGPRIUK;
        if (str == "TANGERANG")     return TANGERANG;
        if (str == "CGK")           return CGK;
        return NONE;
    }

    Line rgbToLine(std::string str) {
        if (str == "#ff0000")   return BOGOR;
        if (str == "#0000ff")   return CIKARANG;
        if (str == "#008000")   return RANGKASBITUNG;
        if (str == "#ff8095")   return TANJUNGPRIUK;
        if (str == "#d2691e")   return TANGERANG;
        if (str == "#2D2B70")   return CGK;
        return NONE;
    }
    
    // nlohmann json helper functions
    void to_json(json& j, const Line& l) {
        j = json::string_t{lineToString(l)};
    }
    void from_json(const json& j, Line& l) {
        l = strToLine(j.get<string>());
    }

    struct Route : Saveable {
        enum Validation {
            VALID,
            INVALID,
            UNKNOWN
        };
        vector<string> route;
        Line line;

        Route(KRL::Line l) : line(l) {
            string lineStr = lineToString(line);

            if (json routesJson = getFromJson(); !routesJson.is_null() && routesJson.contains(lineStr))
                route = routesJson[lineStr].get<vector<string>>();
        }

        static string validationToString(Validation v) {
            switch (v) {
                case VALID:   return "VALID";
                case INVALID: return "INVALID";
                case UNKNOWN: return "UNKNOWN";
            } 
        }

        inline static pair<string, string> stringToPair(string s, char sep = '-') {
            int sepLoc = s.find(sep);
            return { s.substr(0, sepLoc), s.substr(sepLoc + 1) };
        }

        inline static bool inBetween(int from, int between, int to) {
            return (to < between && between < from) || (from < between && between < to);
        }

        void extend(const vector<string>& queried, bool reverse = false) {
            if (route.size() < 2) { route = queried; saveToJson(); return; }
            int queriedSize = queried.size();
            
            for (int j = 0; j < queriedSize; j++) {
                if (queried[j] == route[0]) {
                    if (!reverse && ((queriedSize > j+1 && queried[j+1] != route[1]) || (j-1 >= 0 && queried[j-1] == route[1]))) {
                        vector<string> reversed(queried.rbegin(), queried.rend());
                        extend(reversed, true);
                        return;
                    }
                    for (int k = j-1; k >= 0; k--)
                        route.insert(route.begin(), queried[k]);
                    saveToJson();
                }
                if (queried[j] == *route.rbegin()) {
                    if (!reverse && ((j-1 >= 0 && queried[j-1] != *next(route.rbegin())) || (queriedSize > j+1 && queried[j+1] == *next(route.rbegin())))) {
                        vector<string> reversed(queried.rbegin(), queried.rend());
                        extend(reversed, true);
                        return;
                    }
                    for (int k = j+1; k < queriedSize; k++)
                        route.push_back(queried[k]);
                    saveToJson();
                }
            }
        }

        Validation check(pair<string, string> sig, string from, string to) {
            if (route.size() < 2) return UNKNOWN;

            int sigFrom = -1, sigTo = -1;
            int fromIdx = -1, toIdx = -1;

            for (int i = 0; i < (int)route.size(); i++) {
                if (route[i] == from) fromIdx = i;
                if (route[i] == to) toIdx = i;
                if (route[i] == sig.first) sigFrom = i;
                if (route[i] == sig.second) sigTo = i;
                if (sigFrom != -1 && sigTo != -1 && fromIdx != -1 && toIdx != -1) break;
            }

            if (sigFrom == -1 && sigTo == -1)
                return UNKNOWN;
            if (fromIdx == -1 || toIdx == -1)
                return UNKNOWN;

            if (fromIdx!=-1 && toIdx!=-1 && sigFrom!=-1 && sigTo!=-1) {
                if (inBetween(fromIdx, sigFrom, toIdx) || inBetween(fromIdx, sigTo, toIdx))
                    return INVALID;
                if (fromIdx - toIdx < 0) {
                    if (sigFrom <= fromIdx && toIdx <= sigTo) 
                        return VALID;
                } else 
                    if (sigTo <= toIdx && fromIdx <= sigFrom)
                        return VALID;
                    
                return INVALID;
            }

            if (fromIdx!=-1 && toIdx!=-1 && sigFrom!=-1) {
                if (inBetween(fromIdx, sigFrom, toIdx))
                    return INVALID;

                if (fromIdx - toIdx < 0) {
                    if (toIdx <= sigFrom)
                        return INVALID;
                    if (sigFrom <= fromIdx)
                        return UNKNOWN;
                } else {
                    if (sigFrom <= toIdx)
                        return INVALID;
                    if (fromIdx <= sigFrom)
                        return UNKNOWN;
                }
            }

            if (fromIdx!=-1 && toIdx!=-1 && sigTo!=-1) {
                if (inBetween(fromIdx, sigTo, toIdx))
                    return INVALID;
                if (fromIdx - toIdx < 0) {
                    if (sigTo <= fromIdx)
                        return INVALID;
                    if (toIdx <= sigTo)
                        return UNKNOWN;
                } else {
                    if (fromIdx <= sigTo)
                        return INVALID;
                    if (sigTo <= toIdx)
                        return UNKNOWN;
                }
            }

            return UNKNOWN;
        }

        Validation check(string sig, string from, string to) {
            return check(stringToPair(sig), from, to);
        }

        json getFromJson() override {
            if (ifstream routesJson("build/routes.json"); routesJson.is_open()) {
                stringstream buff;
                routesJson >> buff.rdbuf();
                routesJson.close();

                json res = json::parse(buff.str());
                return res;
            } else return nullptr;
        }

        void saveToJson() override {
            string lineStr = lineToString(line);
            json routesJson = getFromJson();

            if (routesJson.is_null()) routesJson = json::object();
                routesJson[lineStr] = route;

            if (ofstream j("build/routes.json"); j.is_open()) {
                j << routesJson.dump(2);
                j.close();
            }
        }
    };

    static array<unique_ptr<Route>, 6> routes {
        make_unique<Route>(BOGOR),
        make_unique<Route>(CIKARANG),
        make_unique<Route>(RANGKASBITUNG),
        make_unique<Route>(TANJUNGPRIUK),
        make_unique<Route>(TANGERANG),
        make_unique<Route>(CGK)
    };
}

class Station;
class TransitStation;
class NormalStation;

class Train {
    private:
        KRL::Website& api = KRL::Website::instantiate();
        string id;
        KRL::Line line;
        string route;
        string arriveTime;
    public:
        Train(string i, KRL::Line l, string r, string a) : id(i), line(l), route(r), arriveTime(a) {}
        string getId() { return id; }
        KRL::Line getLine() { return line; }
        string getRoute() { return route; }
        vector<pair<unique_ptr<Station>, int>> getStops(bool onlyTransit = false);
        int timeAt(string id);
        
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Train, id, line, route, arriveTime);
};

class Station : public Saveable {
    protected: 
        KRL::Website& api = KRL::Website::instantiate();
        string id;
        string name;
        Station(string i, string n) : id(i), name(n) {}

        virtual void setLine() = 0;
        void setName() {
            if (json stationJson = getFromJson(); !stationJson.empty()) {
                if (stationJson.contains(id)) {
                    name = stationJson[id]["name"].get<string>();
                    return;
                } 
            }

            if (auto res = api.callApi("/krl-station")) {
                if (res->status != 200) {
                    cerr << "Error " << res->status << ":\n" << res->body << endl; 
                    return;
                }

                json stations = json::parse(res->body)["data"];

                for (auto &station : stations)
                    if (station["sta_id"].get<string>() == id) 
                        name = station["sta_name"].get<string>();
                return;
            } else {
                cerr << "Error: " << to_string(res.error()) << endl;
                return;
            }
        }
        json getFromJson() override {
            ifstream in("build/stations.json");
            if (in.is_open()) {
                stringstream stationsFile;
                in >> stationsFile.rdbuf();
                in.close();

                return json::parse(stationsFile.str());
            } else return json::object();
        }
    public:
        string getName() { return name; }
        string getId() { return id; }

        vector<Train> getTrains(int from, int to) {
            string timeFrom = Time::minutesToTime(from);
            string timeTo = Time::minutesToTime(to);

            if (auto res = api.callApi("/schedules?stationid="+id+"&timefrom="+timeFrom+"&timeto="+timeTo)) {
                if (res->status != 200) { 
                    cerr << "Error " << res->status << " in Station::getTrains():\n" << res->body << endl; 
                    return {};
                }
                
                json trainsJson = json::parse(res->body)["data"];

                vector<Train> trains;
                for (auto& train : trainsJson) {
                    string lineStr = train["ka_name"].get<string>();
                    lineStr = lineStr.substr(lineStr.rfind(' ') + 1);
                    
                    trains.emplace_back(
                        train["train_id"].get<string>(), 
                        KRL::strToLine(lineStr), 
                        train["route_name"].get<string>(),
                        train["time_est"].get<string>()
                    );
                }

                return trains;
            } else {
                cerr << "Error in Station::getTrains()                 : " << to_string(res.error()) << endl;
                return {};
            }
        }

        vector<Train> getTrains(string from, string to) {
            return getTrains(Time::timeToMinutes(from), Time::timeToMinutes(to));
        }

        TransitStation* isTransitStation();

        virtual set<KRL::Line> getLine() = 0;

        virtual ~Station() {}
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Station, id, name);
};

class NormalStation : public Station {
    private:
        KRL::Line line;

        void setLine() override {
            if (json stationJson = getFromJson(); !stationJson.empty()) {
                if (stationJson.contains(id) && stationJson[id].contains("line") && !stationJson[id]["line"].empty()) {
                    line = KRL::strToLine(stationJson[id]["line"].get<string>());
                    return;
                }
            }

            line = getTrains(420, 540)[0].getLine();

            return;
        };

    public:
        NormalStation(string i, string n, KRL::Line l) : Station(i, n), line(l) {
            saveToJson();
        }
        NormalStation(string i, string n) : Station(i, n) { 
            setLine();
            saveToJson();
        }
        NormalStation(string i, KRL::Line l) : Station(i, ""), line(l) {
            setName();
            saveToJson();
        }
        NormalStation(string i) : Station(i, "") {
            setLine();
            setName();
            saveToJson();
        }

        set<KRL::Line> getLine() override { return { line }; }

        void saveToJson() override {
            json stationsJson = getFromJson();
            if (stationsJson.contains(id) && stationsJson[id].size() == 3) return;

            stationsJson[id] = {
                {"name", name},
                {"line", line},
                {"transit", false}
            };

            ofstream out("build/stations.json");
            if (out.is_open()) {
                out << stationsJson.dump(2);
                out.close();
            }
        }

        NLOHMANN_DEFINE_DERIVED_TYPE_INTRUSIVE(NormalStation, Station, line);
};

class TransitStation : public Station {
    private:
        set<KRL::Line> lines;

        void setLine() override {
            if (json stationJson = getFromJson(); !stationJson.empty()) {
                if (stationJson.contains(id) && stationJson[id].contains("lines") && !stationJson[id]["lines"].empty()) {
                    for (auto& l : stationJson[id]["lines"])
                        lines.insert(KRL::strToLine(l.get<string>()));
                    return;
                }
            }

            for (auto& train : getTrains(420, 540))
                lines.insert(train.getLine());

            return;
        };
    public:
        TransitStation(string i, string n) : Station(i, n) { 
            setLine(); 
            saveToJson(); 
        }
        TransitStation(string i, string n, set<KRL::Line> l) : Station(i, n), lines(l) { 
            saveToJson(); 
        }
        TransitStation(string i) : Station(i, "") {
            setLine();
            setName();
            saveToJson();
        }
        TransitStation(string i, set<KRL::Line> l) : Station(i, ""), lines(l) {
            setName();
            saveToJson();
        }

        set<KRL::Line> getLine() override { return lines; }


        void saveToJson() override {
            json stationsJson = getFromJson();
            if (stationsJson.contains(id) && stationsJson[id].size() == 3) return;

            stationsJson[id] = {
                {"name", name},
                {"lines", lines},
                {"transit", true}
            };

            ofstream out("build/stations.json");
            if (out.is_open()) {
                out << stationsJson.dump(2);
                out.close();
            }
        }

        NLOHMANN_DEFINE_DERIVED_TYPE_INTRUSIVE(TransitStation, Station, lines);
};

TransitStation* Station::isTransitStation() {
    if (auto transit = dynamic_cast<TransitStation*>(this))
        return transit;
    else return nullptr;
}

vector<pair<unique_ptr<Station>, int>> Train::getStops(bool onlyTransit) {
    if (auto res = api.callApi("/schedules-train?trainid="+id)) {
        if (res->status != 200) { 
            cerr << "Error " << res->status << " in Train::getStops():\n" << res->body << endl; 
            return {};
        }
        json body = json::parse(res->body)["data"];

        vector<pair<unique_ptr<Station>, int>> stops;
        for (auto& stop : body) {
            string stopID = stop["station_id"].get<string>();
            string name = stop["station_name"].get<string>();

            if (stop["transit"].is_string() && !onlyTransit) {
                string lineStr = stop["ka_name"].get<string>();
                lineStr = lineStr.substr(lineStr.rfind(" ") + 1);
                KRL::Line line = KRL::strToLine(lineStr);

                stops.emplace_back(std::make_unique<NormalStation>(stopID, name, line), Time::timeToMinutes(stop["time_est"].get<string>()));
            } else {
                vector<string> transitLineStr = stop["transit"].get<vector<string>>();

                set<KRL::Line> transitLines;
                for (string& l : transitLineStr)
                    transitLines.insert(KRL::rgbToLine(l));

                stops.emplace_back(std::make_unique<TransitStation>(stopID, name, transitLines), Time::timeToMinutes(stop["time_est"].get<string>()));
            }
        }

        return stops;
    } else {
        cerr << "Error in Train::getStops(): " << to_string(res.error()) << endl;
        return {};
    }
}

unique_ptr<Station> determineStationType(string id) {
    if (ifstream stationsJson("build/stations.json"); stationsJson.is_open()) {
        stringstream buff;
        stationsJson >> buff.rdbuf();
        json stations = json::parse(buff.str());
        if (stations.contains(id) && stations[id].contains("transit") && (stations[id].contains("line") || stations[id].contains("lines")) && stations[id].contains("name")) {
            if (stations[id]["transit"].get<bool>())
                return make_unique<TransitStation>(id, stations[id]["name"].get<string>(), stations[id]["lines"].get<set<KRL::Line>>());
            else 
                return make_unique<NormalStation>(id, stations[id]["name"].get<string>(), stations[id]["line"].get<KRL::Line>());
        }
    }

    KRL::Website& api = KRL::Website::instantiate();
    if (auto res = api.callApi("/schedules?stationid="+id+"&timefrom=7&timeto=12")) {
        if (res->status != 200) { 
            cerr << "Error " << res->status << " in determineStationType():\n" << res->body << endl; 
            return nullptr;
        }

        set<KRL::Line> lines;
        for (auto& train : json::parse(res->body)["data"]) {
            string lineStr = train["ka_name"].get<string>();
            KRL::Line line = KRL::strToLine(lineStr.substr(lineStr.rfind(" ") + 1));

            lines.insert(line);
            if (lines.size() > 1) break;
        }

        if (lines.size() == 1) 
            return make_unique<NormalStation>(id, *lines.begin());
        else
            return make_unique<TransitStation>(id, lines);
    } else {
        cerr << "Error in determineStationType(): " << to_string(res.error()) << endl;
        return nullptr;
    }
}

int Train::timeAt(string id) {
    for (auto &[stop, time] : getStops())
        if (stop->getId() == id) return time;
    return -1;
}

vector<tuple<string, int, int>> schedule(string fromID, string toID, int from, int to) {
    vector<tuple<string, int, int>> schedule;
    auto src = determineStationType(fromID);
    if (!src) { cerr << "Error: could not determine station type for " << fromID << endl; return {}; }
    cerr << "src: " << src->getName() << endl;

    auto dest = determineStationType(toID);
    if (!dest) { cerr << "Error: could not determine station type for " << toID << endl; return {}; }
    cerr << "dest: " << dest->getName() << endl;

    auto trains = src->getTrains(from, to);
    cerr << "trains count: " << trains.size() << endl;

    for (auto& train : trains) {
        cerr << "processing train: " << train.getId() << " line: " << KRL::lineToString(train.getLine()) << " route: " << train.getRoute() << endl;

        if (train.getLine() >= KRL::routes.size()) {
            cerr << "Warning: line out of bounds for train " << train.getId() << endl;
            continue;
        }

        cerr << "checking route..." << endl;
        KRL::Route::Validation valid = KRL::routes[train.getLine()]->check(train.getRoute(), src->getName(), dest->getName());
        cerr << "valid: " << KRL::Route::validationToString(valid) << endl;

        if (valid == KRL::Route::INVALID) continue;

        cerr << "getting stops..." << endl;
        vector<string> route;
        int timeFrom = -1, timeTo = -1;

        for (auto& stop : train.getStops()) {
            cerr << "stop: " << stop.first->getName() << endl;
            route.push_back(stop.first->getName());
            if (stop.first->getId() == src->getId()) timeFrom = stop.second;
            if (stop.first->getId() == dest->getId()) timeTo = stop.second;
        }

        cerr << "extending route..." << endl;
        KRL::routes[train.getLine()]->extend(route);
        cerr << "done extending" << endl;

        if (timeFrom != -1 && timeTo != -1 && timeFrom < timeTo) 
            schedule.emplace_back(train.getId(), timeFrom, timeTo);
    }

    return schedule;
}

vector<tuple<string, int, int>> schedule(string fromID, string toID, string from, string to) {
    return schedule(fromID, toID, Time::timeToMinutes(from), Time::timeToMinutes(to));
}

unordered_map<string, string> getStations() {
    if (ifstream i("build/stations.json"); i.is_open()) {
        stringstream buff;
        i >> buff.rdbuf();
        i.close();
        json stationsJson = json::parse(buff.str());
        unordered_map<string, string> names;
        for (auto& [id, data]: stationsJson.items()) {
            names[id] = data["name"].get<string>();
        }
        return names;
    }


    KRL::Website& api = KRL::Website::instantiate();
    if (auto res = api.callApi("/krl-station")) {
        if (res->status != 200) {
            cerr << "Error " << res->status << ":\n" << res->body << endl; 
            return {};
        }

        json stations = json::parse(res->body)["data"];

        unordered_map<string, string> names;
        for (auto &station : stations)
            if (station["fg_enable"].get<int>())
                names[station["sta_id"].get<string>()] = station["sta_name"].get<string>();

        if (ifstream i("build/stations.json"); !i.is_open()) {
            ofstream o("build/stations.json");
            json stations;
            for (auto& [id, name] : names)
                stations[id] = { {"name", name} };
            o << stations.dump(2);
            o.close();
        } 

        return names;
    } else {
        cerr << "Error: " << to_string(res.error()) << endl;
        return {};
    }
}
 
void sendJson(Response& res, const json& body, int status = 200) {
    res.set_header("Content-Type", "application/json");
    res.set_header("Access-Control-Allow-Origin", "*");
    res.status = status;
    res.body   = body.dump(2) ;
}
 
void sendError(Response& res, const string& msg, int status = 400) {
    sendJson(res, { {"error", msg} }, status);
}
 
int main() {
    Server svr;
 
    // GET /stations
    // returns { "DPB": "DEPOK BARU", ... }
    svr.Get("/stations", [](const Request&, Response& res) {
        auto names = getStations();
        json j;
        for (auto& [id, name] : names)
            j[id] = name;
        sendJson(res, j);
    });
 
    // GET /stops/:trainId?onlyTransit=true
    // returns array of stops with id, name, time, lines
    svr.Get("/stops/:trainId", [](const Request& req, Response& res) {
        string trainId = req.path_params.at("trainId");
        bool onlyTransit = req.has_param("onlyTransit") && req.get_param_value("onlyTransit") == "true";

        cout << trainId << " " << (onlyTransit ? "true" : "false") << endl;

        Train train(trainId, KRL::NONE, "", "");
        auto stops = train.getStops(onlyTransit);
 
        if (stops.empty()) {
            sendError(res, "No stops found for train " + trainId, 404);
            return;
        }

        json stopsJson = json::array();
        for (auto &[stop, time] : stops) {
            json obj = {
                { "stationID", stop->getId() },
                { "stationName", stop->getName() },
                { "lines", stop->getLine() },
                { "arriveTime", time }
            };

            stopsJson.push_back(obj);
        }
 
        sendJson(res, stopsJson);
    });
 
    // GET /trains/:stationId?from=07:00&to=09:00
    // returns array of trains at that station in the time window
    svr.Get("/trains/:stationId", [](const Request& req, Response& res) {
        string stationId = req.path_params.at("stationId");
 
        if (!req.has_param("from") || !req.has_param("to")) {
            sendError(res, "Missing 'from' and 'to' query params (e.g. ?from=07:00&to=09:00)");
            return;
        }
 
        string from = req.get_param_value("from");
        string to   = req.get_param_value("to");
 
        cout << stationId << " " << from << " " << to << endl;

        auto station = determineStationType(stationId);
        if (!station) {
            sendError(res, "Station not found: " + stationId, 404);
            return;
        }
 
        auto trains = station->getTrains(from, to);
        json trainsJson(trains);

        sendJson(res, trainsJson);
    });
 
    // GET /schedule?from=DPB&to=JAKK&timeFrom=07:00&timeTo=09:00
    // returns array of { trainId, departTime, arriveTime }
    svr.Get("/schedule", [](const Request& req, Response& res) {
        if (!req.has_param("from") || !req.has_param("to") ||
            !req.has_param("timeFrom") || !req.has_param("timeTo")) {
            sendError(res, "Required params: from, to, timeFrom, timeTo");
            return;
        }
 
        string fromId    = req.get_param_value("from");
        string toId      = req.get_param_value("to");
        string timeFrom  = req.get_param_value("timeFrom");
        string timeTo    = req.get_param_value("timeTo");
 
        cout << fromId << " " << toId << " " << timeFrom << " " << timeTo << endl;

        auto results = schedule(fromId, toId, timeFrom, timeTo);
 
        if (results.empty()) {
            sendJson(res, json::array());
            return;
        }
 
        json arr = json::array();
        for (auto& [id, from, to] : results) {
            arr.push_back({
                {"trainId",     id},
                {"departTime",  Time::minutesToTime(from)},
                {"arriveTime",  Time::minutesToTime(to)},
                {"durationMin", to - from},
                {"duration",    Time::minutesToEnglish(to - from)}
            });
        }
 
        sendJson(res, arr);
    });
 
    // 404 fallback
    svr.set_error_handler([](const Request& req, Response& res) {
        // Only set the error message if a payload wasn't already configured
        if (res.body.empty()) {
            sendError(res, "Not found: " + req.path, 404);
        }
    });
 
    cout << "KRL API server running on http://0.0.0.0:8080" << endl
         << "Endpoints:" << endl
         << "  GET /stations" << endl
         << "  GET /stops/:trainId?onlyTransit=true" << endl
         << "  GET /trains/:stationId?from=HH:MM&to=HH:MM" << endl
         << "  GET /schedule?from=ID&to=ID&timeFrom=HH:MM&timeTo=HH:MM" << endl;
 
    svr.listen("0.0.0.0", 8080);
    return 0;
}