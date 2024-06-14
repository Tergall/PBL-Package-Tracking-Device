/*
agsmII_MMS_lbr.ino v 0.971/20170323 - a-gsmII 2.105 LIBRARY SUPPORT
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

/*update agsm_MMS_lbr.h, "MMS mobile provider related settings" section with your provider MMS settings!!!*/

/*shut down MMS IP*/
void deactMMSPDP(){
	agsmSerial.println(F("AT+QIDEACT"));
}

/*setup the modem for MMS send*/
void setupModemForMMS(){

	clearMMS();

	deactMMSPDP();

	clearagsmSerial();
	agsmSerial.println(F("AT+QIFGCNT=0"));
	delay(100);
	//set MMS APN
	agsmSerial.print(F("AT+QICSGP=1,\""));
	agsmSerial.print(MMSAPN);
	agsmSerial.println(F("\""));
	delay(100);
	clearagsmSerial();
	//set MMSC URL (PROXY & POST)
	agsmSerial.print(F("AT+QMMURL=\""));
	agsmSerial.print(MMSCURL);
	agsmSerial.println(F("\""));
	delay(100);
	clearagsmSerial();
	//set MMSC PROXY ADDRESS & PORT
	agsmSerial.print(F("AT+QMMPROXY=1,\""));
	agsmSerial.print(MMSCPROXYADDRESS);
	agsmSerial.print(F("\","));
	agsmSerial.println(MMSCPROXYPORT);
	delay(100);
	clearagsmSerial();
}

/*clear all MMS content*/
void clearMMS(){
	clearagsmSerial();
	agsmSerial.println(F("AT+QMMSW=0"));
	delay(50);
	clearagsmSerial();
}

/*you can add multiple receipments address (phone numbers and emails). Simply call this function for each address. */
void addReceipment(char* receipment){
	clearagsmSerial();
	agsmSerial.print(F("AT+QMMSW=1,1,\""));
	agsmSerial.print(receipment);
	agsmSerial.println(F("\""));
	delay(100);
	clearagsmSerial();
}

/*set MMS subject*/
int setMMSSubject(char* subject){
	int res=0;
	clearBUFFD();
	clearagsmSerial();
	//agsmSerial.println(F("AT+QMMSCS=\"UTF8\",1"));
	agsmSerial.println(F("AT+QMMSCS=\"ASCII\",1"));
	delay(10);
	clearagsmSerial();
	agsmSerial.println(F("AT+QMMSW=4,1"));
	delay(10);
	res = recUARTDATA(">","ERROR",3);  
	if(res==1) {    
		clearBUFFD();
		sprintf(buffd,"%s%c",subject,0x1A);
		aGsmCMD(buffd,2);  
		clearBUFFD();
		res = recUARTDATA("OK","ERROR",3);  
		delay(50);
		Serial.flush();
		delay(50);
	}
	return res;
}

/*RAM disk file support for MMS attachments*/
void delAttachement(char *filename){
	//delete atachement
	agsmSerial.print(F("AT+QFDEL=\"RAM:"));
	agsmSerial.print(filename);
	agsmSerial.println(F("\""));
	delay(100);
	clearagsmSerial();
}

/*RAM disk file support for MMS attachments*/
int uplAttachement(char *filename, char *fileBuffer){
	int res=0;
	char f [40];
	delAttachement(filename);//just free the space
	memset(f,0x00, sizeof(f));
	sprintf(f,"+QFUPL=\"RAM:%s\",%i\r",filename,strlen(fileBuffer));
	res = sendATcommand(f, "CONNECT","ERROR",3);
	if(res==1){
		clearagsmSerial();
		agsmSerial.println(fileBuffer);
		res = recUARTDATA();
	}
	return res;
}

/*you can add multiple attachements, previously uploaded to the RAM disk*/
void addAtachement(char* filename){
	agsmSerial.print(F("AT+QMMSW=5,1,\"RAM:"));
	agsmSerial.print(filename);
	agsmSerial.println(F("\""));
	delay(100);
	clearagsmSerial();
}

/*here it is*/
int sendMMS(){
	int res = 0;
	clearagsmSerial();
	res = sendATcommand("+QMMSEND=1","0,0","ERR",30); 
	delay(250);
	clearagsmSerial();
	deactMMSPDP();
	return res;
}
