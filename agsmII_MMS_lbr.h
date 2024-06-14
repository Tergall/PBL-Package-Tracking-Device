/*
agsmII_MMS_lbr.h v 0.971/220170323 - a-gsmII 2.105 LIBRARY SUPPORT
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

#define agsm_MMS
#if !defined (agsm_BASIC)
	#include "agsmII_basic_lbr.h"
#endif

/*MMS mobile provider related settings*/
#define MMSAPN 				"mms"
#define MMSCURL 			"http://wap.mms.orange.ro:8002"
#define MMSCPROXYADDRESS 	"62.217.247.252"
#define MMSCPROXYPORT 		"8799"
/*MMS mobile provider related settings - END*/

void setupModemForMMS(void);
void clearMMS(void);
void addReceipment(char* receipment);
int setMMSSubject(char* subject);
int attachMMS(char* filename);
int uplAttachement(char *filename, char *fileBuffer);
void delAttachement(char *filename);
void addAtachement(char *filename);
int sendMMS(void);

