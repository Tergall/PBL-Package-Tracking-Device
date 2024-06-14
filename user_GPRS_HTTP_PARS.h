/*your SIM's GPRS related settings, next*/
#define GPRS_context "internet"
#define GPRS_user "internet"
#define GPRS_password "internet"

/*your IP related settings*/
#define SERVER_ADDRESS "itbrainpower.net"		//server URL - used also for http URL...
#define SERVER_PROTOCOL "TCP"        			//server PROTOCOL TCP or UDP (for HTTP is TCP)

/*your HTTP related settings*/
#define HTTP_PATH "/a-gsm/test/echo"         	// your WEB application path... 

//#define HTTP_PROTOCOL "https://"              	// do not change
//#define SERVER_PORT "443"        				//server PORT (usual for HTTPS is 443)
//#define HTTP_PROTOCOL "http://"              	// do not change
//#define SERVER_PORT "80"        				//server PORT (usual for HTTP is 80)

/*
in this example, the application will open 
http://itbrainpower.net:80//a-gsm/test/echo
or
https://itbrainpower.net:443//a-gsm/test/echo
in case of success the previous server script (echo.php) will reply to you with
via POST
[POSTpar0name]:[POSTpar0value]
[POSTpar1name]:[POSTpar1value]
....
[POSTparNname]:[POSTparNvalue]
via GET
[POSTpar0name]:[POSTpar0value]
[POSTpar1name]:[POSTpar1value]
....
[POSTparMname]:[POSTparMvalue]

The echo.php script is:
<?php
    echo 'via POST'.chr(0x0A).chr(0x0D);
    foreach($_POST as $key=>$data)
        echo $key.':'.$data.chr(0x0A).chr(0x0D);
    echo 'via GET'.chr(0x0A).chr(0x0D);
    foreach($_GET as $key=>$data)
        echo $key.':'.$data.chr(0x0A).chr(0x0D);
?>
*/
 
