#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <DNSServer.h>
#include <map>

// Constants for the HTML pages and config file
static const String beginHtml = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>SmartCube Configure</title><style>body{font-family:Arial,sans-serif;background-color:#f4f4f9;color:#333;display:flex;align-items:center;justify-content:center;height:100vh;margin:0}.container{width:100%;max-width:300px;padding:20px;background:#fff;border-radius:10px;box-shadow:0 4px 8px rgba(0,0,0,0.2);box-sizing:border-box}h2{margin-top:0;text-align:center;color:#0073e6}label{display:block;margin-bottom:5px;font-weight:bold}input[type='text'],input[type='password']{width:100%;padding:8px;margin-bottom:15px;border:1px solid #ccc;border-radius:4px;box-sizing:border-box}button{width:100%;padding:10px;background-color:#0073e6;color:white;border:none;border-radius:4px;cursor:pointer;font-size:16px}button:hover{background-color:#005bb5}table{width:100%;margin-top:20px}td{padding:5px;text-align:left}</style></head><body><div class='container'><h2>Configure AP</h2><table><tbody><tr><td><label for='ssid'>SSID</label></td><td><input id='ssid' type='text' placeholder='Enter SSID'/></td></tr><tr><td><label for='pass'>Password</label></td><td><input id='pass' type='password' placeholder='Enter Password'/></td></tr><tr><td colspan='2'><button onclick=\"location.href = '/add?ssid=' + encodeURIComponent(document.getElementById('ssid').value) + '&pass=' + encodeURIComponent(document.getElementById('pass').value);\">Add Network</button></td></tr></tbody></table><br/><table><tbody>";
static const String endHtml = "</tbody></table></body></html>";
static const String configFile = "/cubeWifiManager";

// Timeout for WiFi connection attempts
static const unsigned long connectionTimeout = 15000; // 15 seconds

struct cubeWifiManager {
public:
    cubeWifiManager(Adafruit_SSD1306& display);
    cubeWifiManager(String ssid, String pass, bool hidden, Adafruit_SSD1306& display);
    bool start();
    void reset();
    void addSsid(String ssid, String password);
    void removeSsid(String ssid, String password);
    
private:
    Adafruit_SSD1306& display;
    std::unique_ptr<ESP8266WebServer> server;
    std::map<String, String> _ssids;
    String _ssid, _pass;
    bool _hidden;
    
    void init(String ssid, String pass, bool hidden);
    bool tryConnectToSsid(const char* ssid, const char* pass);
    bool tryConnect();
    void createAP();
    bool redirectToIp();
    void readConfig();
    void writeConfig();
    void handleRoot();
    void handleAdd();
    void handleRemove();
    void handleSelect();
};

// Initialization
void cubeWifiManager::init(String ssid, String pass, bool hidden) {
    // Ensure password meets minimum length
    if (pass != "" && pass.length() < 8) {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("Password too short");
            display.display(); 
            pass = "8characters";
    }
    
    // Get the last 4 characters of the MAC address
    String macAddress = WiFi.macAddress();
    String macSuffix = macAddress.substring(macAddress.length() - 5); 
    macSuffix.replace(":", "");

    // Set default SSID if none provided
    _ssid = ssid.isEmpty() ? "SmartCube_" + macSuffix : ssid;
    _pass = pass;
    _hidden = hidden;


    // Initialize LittleFS with error handling
    if (!LittleFS.begin()) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("FS init failed");
        display.display(); 
    }
}

// Constructors
cubeWifiManager::cubeWifiManager(Adafruit_SSD1306& display) : display(display) {
    init("", "", false);
}

cubeWifiManager::cubeWifiManager(String ssid, String pass, bool hidden, Adafruit_SSD1306& display) : display(display) {
    init(ssid, pass, hidden);
}

// Attempt to start and connect or create AP
bool cubeWifiManager::start() {
    if (_pass == "8characters") {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Using default pass:");
        display.println("8characters");
        display.display(); 
    }
    return tryConnect() || (createAP(), false);
}

// Attempt connection to each saved SSID in order
bool cubeWifiManager::tryConnect() {
    readConfig();
    for (auto const& item : _ssids) {
        if (tryConnectToSsid(item.first.c_str(), item.second.c_str())) {
            return true;
        }
    }
    return false;
}

// Attempt to connect to a specific SSID with timeout, dot animation, and IP display
bool cubeWifiManager::tryConnectToSsid(const char* ssid, const char* pass) {
    WiFi.begin(ssid, pass);
    unsigned long start = millis();

    // Clear display and set initial message
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Connecting to WiFi:");
    display.setCursor(0, 11);
    display.println(String(ssid));
    display.display();

    int dotCount = 0;
    while (millis() - start < connectionTimeout) {
        delay(500);

        // Check WiFi connection status
        if (WiFi.status() == WL_CONNECTED) {
            // Success message with IP address
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("Connected!");
            display.setCursor(0, 12);
            display.print("IP: ");
            display.println(WiFi.localIP());
            display.display();
            delay(500);
            return true;
        }

        // Animate by adding dots
        display.setCursor(0, 20);
        for (int i = 0; i < dotCount; i++) {
            display.print(".");
        }
        display.display();
        dotCount = (dotCount + 1);
    }

    // Connection failed
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Connection failed.");
    display.display();
    WiFi.disconnect();
    return false;
}

// Setup Access Point with DNS and HTTP server
void cubeWifiManager::createAP() {
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_ssid.c_str(), _pass.c_str(), 1, _hidden);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("AccessPoint created");
    display.setCursor(0, 14);
    display.println("SSID:");
    display.setCursor(0, 24);
    display.setTextSize(1);
    display.println(_ssid.c_str());
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println("Config portal:");
    display.setCursor(0, 50);
    display.print("http://");
    display.println(WiFi.softAPIP().toString());
    display.display(); 

    server.reset(new ESP8266WebServer(80));
    DNSServer dnsServer;
    dnsServer.start(53, "*", WiFi.softAPIP());

    server->on("/", std::bind(&cubeWifiManager::handleRoot, this));
    server->on("/add", std::bind(&cubeWifiManager::handleAdd, this));
    server->on("/remove", std::bind(&cubeWifiManager::handleRemove, this));
    server->on("/select", std::bind(&cubeWifiManager::handleSelect, this));

    server->begin();

    while (true) {
        dnsServer.processNextRequest();
        server->handleClient();
        delay(10);
    }
}

// Redirect to AP IP if not accessed directly
bool cubeWifiManager::redirectToIp() {
    if (server->hostHeader() == WiFi.softAPIP().toString()) {
        return false;
    }
    server->sendHeader("Location", "http://" + WiFi.softAPIP().toString(), true);
    server->send(302, "text/plain", "");
    server->client().stop();
    return true;
}

// Modify the addSsid function to take parameters from the `select` page
void cubeWifiManager::addSsid(String ssid, String password) {
    _ssids[ssid] = password;
    writeConfig();

    // Attempt to connect to the selected network
    if (tryConnectToSsid(ssid.c_str(), password.c_str())) {
        // Redirect to the main page on success
        server->sendHeader("Location", "/", true);
        server->send(302, "text/plain", "");
    } else {
        // Show error message if connection failed
        server->send(200, "text/html", "<html><body><h2>Connection failed. Please try again.</h2></body></html>");
    }
}

// Remove SSID from config
void cubeWifiManager::removeSsid(String ssid, String password) {
    if (_ssids.count(ssid) && _ssids[ssid] == password) {
        _ssids.erase(ssid);
        writeConfig();
    }
}

// Handle file-based config loading
void cubeWifiManager::readConfig() {
    _ssids.clear();
    File file = LittleFS.open(configFile, "r");
    if (!file) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Config not found");
        display.display(); 
        return;
    }
    
    while (file.available()) {
        String ssid = file.readStringUntil('\n');
        ssid.trim();
        String pass = file.readStringUntil('\n');
        pass.trim();
        _ssids[ssid] = pass;
    }
    file.close();
}

// Handle file-based config saving
void cubeWifiManager::writeConfig() {
    File file = LittleFS.open(configFile, "w");
    for (const auto& item : _ssids) {
        file.println(item.first);
        file.println(item.second);
    }
    file.close();
}

// Reset configuration
void cubeWifiManager::reset() {
    LittleFS.remove(configFile);
    _ssids.clear();
}

String urlEncode(const String &str) {
    String encoded = "";
    char c;
    for (size_t i = 0; i < str.length(); i++) {
        c = str.charAt(i);
        if (isalnum(c)) {
            encoded += c;
        } else {
            encoded += '%';
            char buf[3];
            snprintf(buf, sizeof(buf), "%02X", c);
            encoded += buf;
        }
    }
    return encoded;
}

void cubeWifiManager::handleRoot() {
    if (redirectToIp()) return;

    // Scan for available networks
    int n = WiFi.scanNetworks();
    String result = beginHtml;

    // Add stored SSIDs to the page
    result += "<h3>Saved Networks</h3>";
    for (const auto& item : _ssids) {
        result += "<tr><td><button onclick=\"location.href='/remove?ssid=' + escape('" + item.first + "') + '&pass=' + escape('" + item.second + "') \">&times;</button></td><td>" + item.first + "</td><td>-</td><td>" + item.second + "</td></tr>";
    }

    // Display available WiFi networks
    result += "</tbody></table><h3>Available Networks</h3><table><tbody>";
    for (int i = 0; i < n; i++) {
        // Get SSID and signal strength
        String ssid = WiFi.SSID(i);
        int rssi = WiFi.RSSI(i);
        bool openNetwork = (WiFi.encryptionType(i) == ENC_TYPE_NONE);

        // Show network with button to select
        result += "<tr><td><button onclick=\"location.href='/select?ssid=" + urlEncode(ssid) + "'\">" + ssid + "</button></td><td>" + (openNetwork ? "(Open)" : "(Secured)") + "</td><td>" + String(rssi) + " dBm</td></tr>";
    }
    result += endHtml;
    server->send(200, "text/html", result);
}

void cubeWifiManager::handleAdd() {
    server->send(200, "text/html", "The ESP will now reboot.");
    addSsid(server->arg("ssid"), server->arg("pass"));
    delay(500);
    ESP.restart();
}

void cubeWifiManager::handleRemove() {
    removeSsid(server->arg("ssid"), server->arg("pass"));
    handleRoot();
}

// Add SSID to config and save
void cubeWifiManager::handleSelect() {
    String ssid = server->arg("ssid");
    String selectPage = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Connect to " + ssid + "</title><style>body{font-family:Arial,sans-serif;background-color:#f4f4f9;color:#333;display:flex;align-items:center;justify-content:center;height:100vh;margin:0}.container{width:100%;max-width:300px;padding:20px;background:#fff;border-radius:10px;box-shadow:0 4px 8px rgba(0,0,0,0.2);box-sizing:border-box}h2{margin-top:0;text-align:center;color:#0073e6}label{display:block;margin-bottom:5px;font-weight:bold}input[type='password']{width:100%;padding:8px;margin-bottom:15px;border:1px solid #ccc;border-radius:4px;box-sizing:border-box}button{width:100%;padding:10px;background-color:#0073e6;color:white;border:none;border-radius:4px;cursor:pointer;font-size:16px}button:hover{background-color:#005bb5}</style></head><body><div class='container'><h2>Connect to " + ssid + "</h2><form action='/add' method='get'><input type='hidden' name='ssid' value='" + ssid + "'><label for='pass'>Password:</label><input id='pass' type='password' name='pass' placeholder='Enter Password'><button type='submit'>Connect</button></form></div></body></html>";
    server->send(200, "text/html", selectPage);
}