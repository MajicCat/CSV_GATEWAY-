#include "wiced_resource.h"

const char resources_apps_DIR_bt_smartbridge_DIR_smartbridge_report_html_data[3358] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n" \
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\r\n" \
"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\r\n" \
"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\r\n" \
"  <head>\r\n" \
"    <link rel=\"stylesheet\" type=\"text/css\" href=\"../../styles/buttons.css\" />\r\n" \
"    <title>Broadcom WICED SmartBridge Demo Application</title>\r\n" \
"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\r\n" \
"    <script src=\"../../scripts/general_ajax_script.js\" type=\"text/javascript\"></script>\r\n" \
"    <script type=\"text/javascript\">\r\n" \
"    /* <![CDATA[ */\r\n" \
"       function reloadData()\r\n" \
"       {\r\n" \
"         do_ajax(\'/bluetooth_device_report.html\', ajax_handler);\r\n" \
"         timeoutID = setTimeout(\'reloadData()\', 1000);\r\n" \
"       }\r\n" \
"       function ajax_handler( result, data )\r\n" \
"       {\r\n" \
"         switch( result )\r\n" \
"         {\r\n" \
"             case AJAX_PARTIAL_PROGRESS:\r\n" \
"                 break;\r\n" \
"             case AJAX_STARTING:\r\n" \
"                 break;\r\n" \
"             case AJAX_FINISHED:\r\n" \
"                 document.getElementById(\"currentData\").innerHTML = data;\r\n" \
"                 break;\r\n" \
"             case AJAX_NO_BROWSER_SUPPORT:\r\n" \
"                 document.getElementById(\"currentData\").innerHTML = \"Failed - your browser does not support this script\";\r\n" \
"                 break;\r\n" \
"             case AJAX_FAILED:\r\n" \
"                 document.getElementById(\"currentData\").innerHTML = \"There was a problem retrieving data\";\r\n" \
"                 break;\r\n" \
"         }\r\n" \
"       }\r\n" \
"       function do_connect(x)\r\n" \
"       {\r\n" \
"         s = document.getElementById(x).innerHTML;\r\n" \
"         \r\n" \
"         do_ajax( \'/connect?\' + s, null, null, 0, null, null, null, null );\r\n" \
"       }\r\n" \
"       function do_disconnect(x)\r\n" \
"       {\r\n" \
"         s = document.getElementById(x).innerHTML;\r\n" \
"         do_ajax( \'/disconnect?\' + s, null, null, 0, null, null, null, null );\r\n" \
"       }\r\n" \
"       function do_details(x)\r\n" \
"       {\r\n" \
"         s = document.getElementById(x).innerHTML;\r\n" \
"         do_ajax( \'/details?\' + s, null, null, 0, null, null, null, null );\r\n" \
"       }\r\n" \
"       function set_passkey()\r\n" \
"       { \r\n" \
"         passkey_value = document.getElementById(\"passkey\").value;\r\n" \
"         \r\n" \
"         do_ajax( \'/set_passkey?\' + passkey_value, null, null, 0, null, null, null, null );\r\n" \
"         clearTimeout(timeoutID);\r\n" \
"         reloadData();\r\n" \
"       }\r\n" \
"    /* ]]> */\r\n" \
"    </script>\r\n" \
"  </head>\r\n" \
"  <body onload=\"reloadData()\" style=\"font-family:verdana;\">\r\n" \
"    <table border=\'0\' cellpadding=\'0\' cellspacing=\'0\' width=\"98%\">\r\n" \
"      <tr style=\"height:20px\"><td>&nbsp;</td></tr>\r\n" \
"      <tr style=\"border-collapse: collapse; padding: 0;\">\r\n" \
"        <td style=\"width:20px\"></td>\r\n" \
"        <td style=\"width:117px\"><img src=\"../../images/brcmlogo.png\" alt=\"Broadcom Logo\" /></td>\r\n" \
"        <td style=\"width:20px;background-image:url(\'../../images/brcmlogo_line.png\');\"></td>\r\n" \
"        <td style=\"vertical-align:middle; text-align:center; font: bold 25px/100% Verdana, Arial, Helvetica, sans-serif;background-image:url(\'../../images/brcmlogo_line.png\');\">\r\n" \
"        WICED&trade; SmartBridge&trade; Demo Application\r\n" \
"        </td>\r\n" \
"        <td style=\"width:137px;background-image:url(\'../../images/brcmlogo_line.png\');\"></td>\r\n" \
"      </tr>\r\n" \
"      <tr><td>&nbsp;</td></tr>\r\n" \
"    </table>\r\n" \
"    <div id=\"currentData\" >Loading SmartBridge Demo Application...\r\n" \
"    </div>\r\n" \
"  </body>\r\n" \
"</html>";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_smartbridge_report_html = { RESOURCE_IN_MEMORY, 3357, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_smartbridge_report_html_data }}};
