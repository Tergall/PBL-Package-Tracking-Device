/*
agsmII_DTMF_lbr.h v 0.96/20170323 - a-gsmII 2.105 LIBRARY SUPPORT
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


#define agsm_DTMF
#if !defined (agsm_BASIC)
	#include "agsmII_basic_lbr.h"
#endif

#define DTMFlenght 100  //DTMF lenght in miliseconds - 90-100ms best value for manual
#define DTMFpause 100   //pause lenght between DTMF in miliseconds - 90-100ms best value for manual

/*
defined in agsm_basic (use them before once, before using DTMF functions):
void setupMODEMforDTMFSusage(void);//prepare modem for sending DTMF
void setupMODEMforDTMFRusage(void);//prepare modem for receiving DTMF
*/
void disableDTMFdetection(void);
int sendDTMF(char* message);
int listen4DTMF(char * DTMF, char * terminator, int to);
