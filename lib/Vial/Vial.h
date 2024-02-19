#ifndef VIAL_H
#define VIAL_H
#include <Arduino.h>

struct Vial
{
    int16_t containerId = -1;
    int16_t qty = -1;
    String name;
    String toJson() const {
        String jsonString = "{";
        jsonString += "\"containerId\":" + String(containerId) + ",";
        jsonString += "\"qty\":" + String(qty) + ",";
        jsonString += "\"name\":\"" + name + "\"";
        jsonString += "}";
        return jsonString;
    };
};

#endif //VIAL_H