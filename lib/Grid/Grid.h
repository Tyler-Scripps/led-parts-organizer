#ifndef GRID_H
#define GRID_H
#include <Arduino.h>
#include <Vial.h>
#include <Adafruit_NeoPixel.h>

// Helper function to get the next substring from a string
bool getNextSubstring(const String& str, int& index, String& substring) {
  int spaceIndex = str.indexOf(' ', index);
  if (spaceIndex == -1) {
    // No more spaces found, get the last substring
    substring = str.substring(index);
    index = str.length(); // Set index to the end
    return !substring.isEmpty();
  } else {
    // Get the substring up to the next space
    substring = str.substring(index, spaceIndex);
    index = spaceIndex + 1; // Move index to the next character after the space
    return true;
  }
}

bool checkAllSubstrings(const String& str1, const String& str2) {
  int index = 0;
  String substring;

  // Iterate through each substring in str1
  while (getNextSubstring(str1, index, substring)) {
    // Check if the substring is present in str2
    if (str2.indexOf(substring) == -1) {
      return false; // Substring not found in str2
    }
  }

  // All substrings found in str2
  return true;
}

class Grid
{
private:
    Adafruit_NeoPixel* pixels;
    int maxVials = -1;
    Vial* vials;
    int width = 10;
    int height = 10;

public:
    Grid();
    ~Grid();

    void begin(int pin, int numPixels);

    bool addNewPart(int x, int y, String name, int qty);
    bool addNewPart(int ledNum, String name, int qty, int containerId);
    int XYtoLED(int x, int y);
    Vial search(int id);
    int search(String name, Vial foundVials[], int maxSearchSize);
    uint16_t list(Vial vialsArr[], uint16_t arrSize);
    void illuminate(uint16_t cid);
    void allOff();
    String toCSV();
    bool deletePart(int containerId);
    void testPixels();
    bool setQuantity(int cid, int newQty);
};

Grid::Grid()
{
}

Grid::~Grid()
{
}

void Grid::begin(int pin, int numVials)
{
    Serial.println("Begining grid");
    pixels = new Adafruit_NeoPixel(numVials, pin, NEO_GRB + NEO_KHZ800);
    Serial.println("Started neopixels");
    maxVials = numVials;
    vials = new Vial[maxVials];
    Vial defaultVial;
    defaultVial.containerId = -1;
    defaultVial.qty = -1;
    defaultVial.name = "";
    for (uint16_t i = 0; i < maxVials; i++)
    {
        vials[i] = defaultVial;
    }
    
    Serial.println("Clearing pixels");
    pixels->begin();
    pixels->clear();
    Serial.println("Cleared pixels");
}

/**
 * @brief will add new part to the grid
 * 
 * @param x x coord on grid
 * @param y y coord on grid
 * @param name searchable string
 * @param qty 
 * @return true 
 * @return false bad inputs found or no free container id
 */
bool Grid::addNewPart(int x, int y, String name, int qty) {
    if (x > 15 || y > 15 || name.length() == 0)
    {
        return false;
    }

    int containerId = -1;
    for (uint16_t i = 0; i < maxVials; i++)
    {
        if (search(i).containerId<0)
        {
            containerId = i;
            break;
        }
    }
    
    if (containerId == -1)
    {
        return false;
    }
    

    Vial newVial;
    newVial.containerId = containerId;
    newVial.qty = qty;
    newVial.name = name;
    vials[XYtoLED(x, y)] = newVial;
    return true;
}

bool Grid::addNewPart(int ledNum, String name, int qty, int containerId) {
    if (ledNum > maxVials - 1)
    {
        return false;
    }

    if (containerId < 0)
    {
        return false;
    }
    
    if (search(containerId).name != "not found")
    {
        return false;
    }
    
    Vial newVial;
    newVial.containerId = containerId;
    newVial.qty = qty;
    newVial.name = name;
    vials[ledNum] = newVial;
    return true;
}

/**
 * @brief converts an xy coord to the raw led number
 * 
 * @param x 
 * @param y 
 * @return int 
 */
int Grid::XYtoLED(int x, int y) {
    int ledNum = -1;

    //even or odd row
    if (y % 2 == 0) { //even
        ledNum = (width * (y + 1)) - x - 1;
    } else {    //odd
        ledNum = (width * y) + x;
    }

    return ledNum;
}

/**
 * @brief find vial by it's containerId
 * 
 * @param id 
 * @return Vial 
 */
Vial Grid::search(int id) {
    for (uint16_t i = 0; i < maxVials; i++)
    {
        if (vials[i].containerId == id)
        {
            return vials[i];
        }
    }
    Vial retVial;
    retVial.containerId = -1;
    retVial.qty = -1;
    retVial.name = "not found";
    return retVial;
}

int Grid::search(String name, Vial foundVials[], int maxSearchSize) {
    uint16_t numFound = 0;
    for (uint16_t i = 0; i < maxVials; i++)
    {
        // Serial.print("Checking: ");
        // Serial.println(vials[i].name);
        if (checkAllSubstrings(name, vials[i].name))
        {
            Serial.print("found: ");
            Serial.println(vials[i].name);
            foundVials[numFound] = vials[i];
            numFound++;
            if (numFound == maxSearchSize - 1)
            {
                return numFound;
            }
        }
    }
    return numFound;
}

/**
 * @brief lists all vials in the grid
 * 
 * @param vialsArr 
 * @param arrSize 
 * @return uint16_t number of vials found
 */
uint16_t Grid::list(Vial vialsArr[], uint16_t arrSize) {
    uint16_t numFound = 0;
    for (uint16_t i = 0; i < maxVials; i++)
    {
        if (vials[i].containerId >= 0)
        {
            numFound++;
            if (numFound <= arrSize)
            {
                vialsArr[numFound-1] = vials[i];
            }
        }
    }
    return numFound;
}

void Grid::illuminate(uint16_t cid) {
    Serial.print("illuminating container: ");
    Serial.println(cid);
    pixels->clear();
    for (uint16_t i = 0; i < maxVials; i++)
    {
        if (vials[i].containerId == cid)
        {
            pixels->setPixelColor(i,pixels->Color(40,40,40));
            Serial.print("Illuminating pixel: ");
            Serial.println(i);
        }
    }
    pixels->show();
}

void Grid::allOff() {
    pixels->clear();
    pixels->show();
}

String Grid::toCSV() {
    String csvString = "containerId,Name,Qty,ledNum\n";
    for (uint16_t i = 0; i < maxVials; i++)
    {
        if (vials[i].containerId >= 0)
        {
            csvString = csvString + String(vials[i].containerId) + ',' + vials[i].name + ',' + vials[i].qty + ',' + i + '\n';
        }
    }
    return csvString;
}

bool Grid::deletePart(int containerId) {
    for (uint16_t i = 0; i < maxVials; i++)
    {
        if (vials[i].containerId == containerId)
        {
            vials[i].name = "";
            vials[i].qty = -1;
            vials[i].containerId = -1;
            return true;
        }
    }
    return false;
}

void Grid::testPixels() {
    Serial.println("testing pixels");
    for (uint16_t i = 0; i < maxVials; i++)
    {
        pixels->clear();
        pixels->setPixelColor(i,pixels->Color(10,10,10));
        pixels->show();
        delay(250);
    }
    Serial.println("done testing pixels");
}

bool Grid::setQuantity(int cid, int newQty) {
    for (uint16_t i = 0; i < maxVials; i++)
    {
        if (vials[i].containerId == cid)
        {
            vials[i].qty = newQty;
            return true;
        }
    }
    return false;
}

#endif //GRID_H