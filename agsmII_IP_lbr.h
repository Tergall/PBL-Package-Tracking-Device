/*
agsmII_IP_lbr.h v 0.97/20170323 - a-gsmII 2.105 LIBRARY SUPPORT
COPYRIGHT (c) 2017 Dragos Iosub / R&D Software Solutions srl

You are legaly entitled to use this SOFTWARE ONLY IN CONJUNCTION WITH a-gsmII DEVICES USAGE. Modifications, derivates and redistribution 
of this software must include unmodified this COPYRIGHT NOTICE. You can redistribute this SOFTWARE and/or modify it under the terms 
of this COPYRIGHT NOTICE. Any other usage may be permited only after written notice of Dragos Iosub / R&D Software Solutions srl.

This SOFTWARE is distributed is provide "AS IS" in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Dragos Iosub, Bucharest 2017.
http://itbrainpower.net
***************************************************************************************
SOFTWARE:
This file MUST be present, toghether with other files, inside a folder named 
like your main sketch!
***************************************************************************************
HARDWARE:
Read the readme file(s) inside the arhive/folder.
***************************************************************************************
HEALTH AND SAFETY WARNING!!!!!!!!!!!!!!!!!!!!
High power audio (around 700mW RMS)! You can damage your years! 
Use it with care when headset is connected. USE IT AT YOUR OWN RISK!
We recomend to set the volume at maximum level 20!
***************************************************************************************
*/

/*user section start*/
#include "user_GPRS_HTTP_PARS.h"
/*user section stop*/

#define agsm_IP
#if !defined (agsm_BASIC)
	#include "agsmII_basic_lbr.h"
#endif

#define HTTP_PROCESSING_GENERAL_TIMEOUT 		120000 //in miliseconds
#define SOCKET_PROCESSING_GENERAL_TIMEOUT 		 60000 //in miliseconds
#define DHCPTIMEOUT 		30
#define TCPCONNECTTIMEOUT 	30
#define PDPTIMEOUT 	        30

#define GET 	0			//GET METHOD 
#define POST 	1			//POST METHOD

#define IPINITIAL 0			//TCPIP IDLE 
#define IPSTART 1			//TCPIP REGISTERED
#define IPCONFIG 2			//startup to activate GRS/CSD context

#define IPGPRSACT 4			//GRS/CSD context ACTIVATED
#define IPSTATUS 5			//local IP address obtained
#define IPCONNECTING 6		//SOCKET CONNECTING ==>"TCP CONNECTING" or "UDP CONNECTING"
#define IPCLOSE 7			//SOCKET CLOSED
#define CONNECTOK 8			//SOCKET CONNECTED
#define PDPDEACT 9			//GRS/CSD context DE-ACTIVATED (unknown reason)

#define SSLENABLED 1
#define SSLDISABLED 0

char HTTP_PROTOCOL[9];			//server protocol+ can be HTTP or HTTPS
char SERVER_PORT[6];        		//server PORT (usual for HTTPit is 80 and for HTTPS it is 443)
void setSSLMODE(int SSLMODE);

int IP_STATE=0;
int unsigned long HTTP_STARTTIME = 0;

int PROCESS_HTTP_REQUEST(char* HTTP_VARIABLES = "", int METHOD = GET, unsigned long timeout = HTTP_PROCESSING_GENERAL_TIMEOUT);
int HTTP_REQUEST(char* HTTP_VARIABLES = "", int METHOD = GET);
int qiAct(int timeout = 10);
int qiLocip(int timeout = DHCPTIMEOUT);
int qiDeact(int timeout = PDPTIMEOUT);
int qiRegapp(int timeout = 10);
void prepareIP(void);
int getIPState(void);
int getIPAck(void);
int socketOpen(void);
void socketClose(void);
int PROCESS_IP(unsigned long timeout = SOCKET_PROCESSING_GENERAL_TIMEOUT);
