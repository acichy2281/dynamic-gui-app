#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#define GUI_PROTOCOL_ID_HIGH 0
#define GUI_PROTOCOL_ID_LOW 0x0F
#define PROPERTY_GATHERER_PROTOCOL_ID_HIGH 0
#define PROPERTY_GATHERER_PROTOCOL_ID_LOW 0xFF

/* System includes */
#include <any>

/**
 * @brief Used to define the type of a widget (i.e Text Box, Slider, Button, etc..)
 * 
 */
enum class WidgetTypes_E
{
    NONE,
    TEXT, 
    SLIDER, 
    BUTTON,
    CHECKBOX,
    RADIOBUTTON
};

/**
 * @brief Used to define the type of a widget event (i.e Click, ScrollDown, ScrollUp, etc...)
 */
enum class EventTypes_E
{
    NONE,
    WIDGET_CLOSE,
    WIDGET_OPEN,
    WIDGET_RESIZE,
    BUTTON_PRESS,
    SLIDER_SET
};

/**
 * @brief Used to store widget info
 * 
 */
struct WidgetInfo_T
{
    WidgetTypes_E   type; 
    std::any        value;
};


struct PortInfo_T
{
    std::string destIp;
    uint16_t destPort;
};

#endif // CUSTOM_TYPES_H