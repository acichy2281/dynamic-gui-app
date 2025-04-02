#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#define GUI_PROTOCOL_ID_HIGH 0
#define GUI_PROTOCOL_ID_LOW 0x0F
#define PROPERTY_GATHERER_PROTOCOL_ID_HIGH 0
#define PROPERTY_GATHERER_PROTOCOL_ID_LOW 0xFF

/* System includes */
#include <any>
#include <string>
#include <variant>

/**
 * @brief Used to define the type of a widget (i.e Text Box, Slider, Button, etc..)
 * 
 */
enum class WidgetTypes_E
{
    None,
    Text, 
    Slider, 
    Button,
    Checkbox,
    Radio,
    Menu,
    MenuItem,
};

enum class WidgetDataTypes_E
{
    None,
    String, 
    Int,
    Float,
    Bool,
    Unknown,
};

enum WidgetFlags_E {
    Interactable = 1 << 3,   
    Static = 1 << 2,  
    Readable = 1 << 1,  
    Writeable = 1 << 0,  
};


/**
 * @brief Used to define the type of a widget event (i.e Click, ScrollDown, ScrollUp, etc...)
 */
enum class EventTypes_E
{
    None,
    widgetClose,
    widgetOpen,
    widgetResize,
    buttonPress,
    sliderSet,
    checkBoxToggle,
    radioSelected,
};

enum WidgetValueVariantType_E 
{
    WIDGET_VARIANT_TYPE_STRING,
    WIDGET_VARIANT_TYPE_INT,
    WIDGET_VARIANT_TYPE_FLOAT,
    WIDGET_VARIANT_TYPE_BOOL,
};

using WidgetValueVariant_T = std::variant<std::string, int, float, bool>;

/**
 * @brief Used to store widget info
 * 
 */
struct WidgetInfo_T
{
    WidgetTypes_E   type; 
    std::any        value;
};

/**
 * @brief Contains widget information such as ID, type, and interactability status
 */
struct WidgetDescriptor_T
{
    uint32_t widgetId;
    uint8_t flags : 4; // Interactable, Static, Readable, Writeable
    uint8_t reserved : 4; // Reserved for future use
    uint8_t widgetType;
    uint8_t dataType; 
    std::string widgetName;
};

struct PortInfo_T
{
    std::string destIp;
    uint16_t destPort;
};

struct Message_T
{
    std::unique_ptr<char[]> data;
    uint16_t size;
};

#endif // CUSTOM_TYPES_H