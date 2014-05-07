#include "ros/ros.h"
#include "std_msgs/String.h"

#include <phidget21.h>

CPhidgetTextLCDHandle txt_lcd;

int CCONV AttachHandler(CPhidgetHandle TXT, void *userptr)
{
    int serialNo;
    const char *name;

    CPhidget_getDeviceName (TXT, &name);
    CPhidget_getSerialNumber(TXT, &serialNo);
    printf("%s %10d attached!\n", name, serialNo);

    return 0;
}

int CCONV DetachHandler(CPhidgetHandle TXT, void *userptr)
{
    int serialNo;
    const char *name;

    CPhidget_getDeviceName (TXT, &name);
    CPhidget_getSerialNumber(TXT, &serialNo);
    printf("%s %10d detached!\n", name, serialNo);

    return 0;
}

int CCONV ErrorHandler(CPhidgetHandle TXT, void *userptr, int ErrorCode, const char *Description)
{
    printf("Error handled. %d - %s\n", ErrorCode, Description);
    return 0;
}

//Display the properties of the attached phidget to the screen.  We will be displaying the name, serial number and version of the attached device.
int display_properties(CPhidgetTextLCDHandle phid)
{
    int serialNo, version, numRows, numColumns, backlight, cursor, contrast, cursor_blink, numScreens;
    const char* ptr;
    CPhidget_DeviceID id;

    CPhidget_getDeviceType((CPhidgetHandle)phid, &ptr);
    CPhidget_getSerialNumber((CPhidgetHandle)phid, &serialNo);
    CPhidget_getDeviceVersion((CPhidgetHandle)phid, &version);
    CPhidget_getDeviceID((CPhidgetHandle)phid, &id);

    CPhidgetTextLCD_getRowCount (phid, &numRows);
    CPhidgetTextLCD_getColumnCount (phid, &numColumns);
    CPhidgetTextLCD_getBacklight (phid, &backlight);
    CPhidgetTextLCD_getContrast (phid, &contrast);
    CPhidgetTextLCD_getCursorOn (phid, &cursor);
    CPhidgetTextLCD_getCursorBlink (phid, &cursor_blink);

    printf("%s\n", ptr);
    printf("Serial Number: %10d\nVersion: %8d\n", serialNo, version);
    if(id == PHIDID_TEXTLCD_ADAPTER){
        CPhidgetTextLCD_getScreenCount (phid, &numScreens);
        printf("# Screens: %d\n", numScreens);
        CPhidgetTextLCD_setScreen(phid, 0);
        CPhidgetTextLCD_setScreenSize(phid, PHIDGET_TEXTLCD_SCREEN_2x16);
        CPhidgetTextLCD_initialize(phid);
    }

    printf("# Rows: %d\n# Columns: %d\n", numRows, numColumns);
    printf("Current Contrast Level: %d\nBacklight Status: %d\n", contrast, backlight);
    printf("Cursor Status: %d\nCursor Blink Status: %d\n", cursor, cursor_blink);

    return 0;
}

void textlcdCallback(const std_msgs::String::ConstPtr& msg)
{
    std_msgs::String local_msg(*msg);
    local_msg.data.resize(40,' ');
    CPhidgetTextLCD_setDisplayString (txt_lcd, 0, (char*)local_msg.data.substr(0,20).c_str());
    CPhidgetTextLCD_setDisplayString (txt_lcd, 1, (char*)local_msg.data.substr(20,20).c_str());
}

int main(int argc, char **argv)
{
    int result;
    const char *err;

    ros::init(argc, argv, "phidget_textlcd");

    ros::NodeHandle n;

    txt_lcd = 0;
    CPhidgetTextLCD_create(&txt_lcd);
    CPhidget_set_OnAttach_Handler((CPhidgetHandle)txt_lcd, AttachHandler, NULL);
    CPhidget_set_OnDetach_Handler((CPhidgetHandle)txt_lcd, DetachHandler, NULL);
    CPhidget_set_OnError_Handler((CPhidgetHandle)txt_lcd, ErrorHandler, NULL);
    CPhidget_open((CPhidgetHandle)txt_lcd, -1);
    ROS_INFO("Waiting for TextLCD to be attached....\n");
    if((result = CPhidget_waitForAttachment((CPhidgetHandle)txt_lcd, 10000)))
    {
        CPhidget_getErrorDescription(result, &err);
        ROS_ERROR("Problem waiting for attachment: %s\n", err);
        return 0;
    }
    display_properties(txt_lcd);
    CPhidgetTextLCD_setContrast (txt_lcd, 255);

    ros::Subscriber sub = n.subscribe("phidget_textlcd_msg", 1000, textlcdCallback);

    ros::spin();

    return 0;
}
