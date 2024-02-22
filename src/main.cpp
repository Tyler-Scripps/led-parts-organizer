#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>


#include <FS.h>        // File System for Web Server Files
#include <LittleFS.h>  // This file system is used.

#include <Grid.h>
#include <Vial.h>

const char* ssid = "Ethyl Ethernet";
const char* password = "spoof-emote-bamboo-rerun";

WebServer server(80);

Grid grid;

bool ledState = false;
unsigned long lastLed = 0;

bool writeDataFile(){
    Serial.printf("Writing file: /data.csv\r\n");

    File file = LittleFS.open("/data.csv", FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return false;
    }
    if(file.print(grid.toCSV())){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
        return false;
    }
    file.close();
    return true;
}

bool readDataFile() {
    Serial.printf("Reading file: /data.csv\r\n");

    File file = LittleFS.open("/data.csv");
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return false;
    }

    String rawStr = file.readString();
    file.close();

    int startIndex = 0;
    int endIndex = 0;
    int lineCount = 0;
  
    while (endIndex != -1) {
        // Find the index of the next newline character
        endIndex = rawStr.indexOf('\n', startIndex);

        // Extract the substring between the current and next newline characters
        String substring;
        if (endIndex != -1) {
            substring = rawStr.substring(startIndex, endIndex);
        } else {
            substring = rawStr.substring(startIndex);
        }

        if (lineCount > 0 && substring.length() > 0)
        {
            Serial.print("Adding: ");
            Serial.println(substring);

            char line[substring.length()+1];
            substring.toCharArray(line, substring.length()+1);
            // Serial.println(line);

            char *values[4]; // an array of pointers to the pieces of the above array after strtok()
            char *ptr = NULL;

            byte index = 0;
            ptr = strtok(line, ",");  // delimiters space and comma
            while (ptr != NULL)
            {
                values[index] = ptr;
                index++;
                ptr = strtok(NULL, ",");
            }

            int cid = atoi(values[0]);
            String name = values[1];
            int qty = atoi(values[2]);
            int ledNum = atoi(values[3]);

            // Serial.print("cid: ");
            // Serial.print(cid);
            // Serial.print(", name:");
            // Serial.print(name);
            // Serial.print(", qty:");
            // Serial.print(qty);
            // Serial.print(", ledNum:");
            // Serial.println(ledNum);

            //add part with values retrieved from line
            bool success = grid.addNewPart(ledNum, name, qty, cid);
            Serial.print("Successfully added: ");
            Serial.println(success);
        }

        // Update the start index for the next iteration
        startIndex = endIndex + 1;
        lineCount++;
    }
    return true;
}

// This function is called when the WebServer was requested to list all existing files in the filesystem.
// a JSON array with file information is returned.
void handleListFiles() {
    File dir = LittleFS.open("/", "r");
    String result;

    result += "[\n";
    while (File entry = dir.openNextFile()) {
        if (result.length() > 4) { result += ",\n"; }
        result += "  {";
        result += "\"type\": \"file\", ";
        result += "\"name\": \"" + String(entry.name()) + "\", ";
        result += "\"size\": " + String(entry.size()) + ", ";
        result += "\"time\": " + String(entry.getLastWrite());
        result += "}";
    }  // while

    result += "\n]";
    server.sendHeader("Cache-Control", "no-cache");
    server.send(200, "text/javascript; charset=utf-8", result);
}  // handleListFiles()


// This function is called when the sysInfo service was requested.
void handleSysInfo() {
    String result;

    result += "{\n";
    result += "  \"Chip Model\": " + String(ESP.getChipModel()) + ",\n";
    result += "  \"Chip Cores\": " + String(ESP.getChipCores()) + ",\n";
    result += "  \"Chip Revision\": " + String(ESP.getChipRevision()) + ",\n";
    result += "  \"flashSize\": " + String(ESP.getFlashChipSize()) + ",\n";
    result += "  \"freeHeap\": " + String(ESP.getFreeHeap()) + ",\n";
    result += "  \"fsTotalBytes\": " + String(LittleFS.totalBytes()) + ",\n";
    result += "  \"fsUsedBytes\": " + String(LittleFS.usedBytes()) + ",\n";
    result += "}";

    server.sendHeader("Cache-Control", "no-cache");
    server.send(200, "text/javascript; charset=utf-8", result);
}  // handleSysInfo()

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

//should rewrite using String WebServer::arg(String name) to retrieve desired search parameters
void handleSearch() {
    String searchStr = server.arg("searchString");
    Serial.print("Searching for: ");
    Serial.println(searchStr);
    if (searchStr.length() == 0)
    {
        server.send(400, "text/plain", "no search string found");
        return;
    }

    Vial foundVials[256];
    int numVialsFound = grid.search(searchStr, foundVials, 256);
    Serial.print(numVialsFound);
    Serial.println(" matches found");
    String returnJSON = "{\"results\": [";
    for (uint16_t i = 0; i < numVialsFound; i++)
    {
        returnJSON += foundVials[i].toJson();
        returnJSON += ",";
    }
    returnJSON.remove(returnJSON.length() - 1);
    returnJSON += "]}";
    server.send(200, "application/json", returnJSON);
    return;
}

void handleAdd() {
    uint16_t newContainerId = -1;
    uint16_t newQty = -1;
    String newName = "";
    uint16_t newX;
    uint16_t newY;

    String tempStr = "";

    tempStr = server.arg("newQty");
    if (tempStr.length() > 0)
    {
        newQty = tempStr.toInt();
    }
    
    tempStr = server.arg("name");
    if (tempStr.length() == 0)
    {
        return;
    } else {
        newName = tempStr;
    }

    tempStr = server.arg("x");
    if (tempStr.length() == 0)
    {
        return;
    } else {
        newX = tempStr.toInt();
    }

    tempStr = server.arg("y");
    if (tempStr.length() == 0)
    {
        return;
    } else {
        newY = tempStr.toInt();
    }    
    
    grid.addNewPart(newX, newY, newName, newQty);
    server.send(200, "text/plain", "Added part: " + newName);
    Serial.print("added: ");
    Serial.println(newName);
    bool writeSuccess = writeDataFile();
}

void handleList(){
    Vial foundVials[256];
    int numFound = grid.list(foundVials, 256);
    Serial.print("Listing ");
    Serial.println(numFound);
    String returnJSON = "{\"results\": [";
    for (uint16_t i = 0; i < numFound; i++)
    {
        returnJSON += foundVials[i].toJson();
        returnJSON += ",";
    }
    returnJSON.remove(returnJSON.length() - 1);
    returnJSON += "]}";
    server.send(200, "application/json", returnJSON);
    return;
}

void handleIlluminate() {
    Serial.print("Got request to illuminate: ");
    Serial.println(server.arg("cid"));
    String containerId = server.arg("cid");
    if (containerId.length() == 0)
    {
        return;
    }
    grid.illuminate(containerId.toInt());
    server.send(200, "text/plain", "Illuminated: " + containerId);
}

void handleAllOff() {
    grid.allOff();
    Serial.println("Turned off all leds");
    server.send(200, "text/plain", "Turned off leds");
}

void handleDelete() {
    String partString = server.arg("cid");
    if (partString.length() == 0)
    {
        server.send(400, "text/plain", "no cid found");
        return;
    }
    
    Serial.print("deleting: ");
    Serial.println(partString);

    bool partDeleted = grid.deletePart(partString.toInt());
    if (partDeleted)
    {
        server.send(200, "text/plain", "deleted part");
        writeDataFile();
    } else {
        server.send(200, "text/plain", "part not deleted");
    }
    
}

void setup(void) {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    Serial.begin(115200);
    Serial.println("wifi test");
    delay(1000);

    if (!LittleFS.begin()) {
        Serial.println("could not mount the filesystem...\n");
        delay(2000);
        Serial.println("formatting...\n");
        LittleFS.format();
        delay(2000);
        Serial.println("restart.\n");
        delay(2000);
        ESP.restart();
    } else {
        Serial.println("LittleFS good");
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        Serial.print(".");
        
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

    grid.begin(16, 100);
    Serial.println("Grid begun");
    readDataFile();
    Serial.println("loaded data from file");


    // register some REST services
    server.on("/api/list", HTTP_GET, handleListFiles);
    server.on("/api/sysinfo", HTTP_GET, handleSysInfo);

    server.onNotFound(handleNotFound);

    server.on("/search", HTTP_GET, handleSearch);
    server.on("/add", HTTP_POST, handleAdd);
    server.on("/list", HTTP_GET, handleList);
    server.on("/illuminate", HTTP_POST, handleIlluminate);
    server.on("/alloff", HTTP_POST, handleAllOff);
    server.on("/delete", HTTP_POST, handleDelete);

    server.serveStatic("/", LittleFS, "/");

    server.begin();
    Serial.println("HTTP server started");

}

void loop(void) {
    server.handleClient();
    delay(2);//allow the cpu to switch to other tasks
    if (millis() - lastLed > 500)
    {
        lastLed = millis();
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState);
    }
    
}

