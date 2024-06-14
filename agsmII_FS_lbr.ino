/*
agsmII_FS_lbr.c v 0.96/20170323 - a-gsmII 2.105 LIBRARY SUPPORT
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

/* delete "filename" from RAM*/
int deleteModemFile(char *filename){//AT+QFDEL="RAM:filename"
	int res=0;
	char f [40];
	sprintf(f,"+QFDEL=\"RAM:%s\"\r",filename);
	res = sendATcommand(f, "OK","ERROR",5);
	return res;
}

/* read "filename" content from RAM, return it in "fileBuffer"*/
int readModemFile(char *filename, char *fileBuffer){
	int res=0;
	char f [40];
	memset(f,0x00, sizeof(f));
	sprintf(f,"+QFDWL=\"RAM:%s\"\r",filename);
	res = sendATcommand(f, "OK","ERROR",10);//+QFDWL:
	if(res==1){
		memset (fileBuffer,0x00, sizeof(fileBuffer));
		char * pch0;
		char * pch1;
		pch0 = strstr(buffd,"CONNECT");
		pch1 = strstr(buffd,"+QFDWL:");
		strncpy(fileBuffer, pch0+9, pch1-pch0-11);
	}
	else if(res==0){
		if(strstr(buffd,"4010")) {Serial.println(F("no file found"));return -10;}//no file
		else if(strstr(buffd,"4013")) {Serial.println(F("no TF card"));return -13;}//no SD
	}
	return res;
}

/* write "fileBuffer" in "filename". if "filename" exists, will be erased first.*/
int writeModemFile(char *filename, char *fileBuffer){
	int res=0;
	char f [40];
	deleteModemFile(filename);//just free the space
	//clearBUFFD();
	memset(f,0x00, sizeof(f));
	sprintf(f,"+QFUPL=\"RAM:%s\",%i\r",filename,strlen(fileBuffer));
	res = sendATcommand(f, "CONNECT","ERROR",3);
	if(res==1){
		clearagsmSerial();
		agsmSerial.println(fileBuffer);
		//res = recUARTDATA("OK","ERROR",5);
		res = recUARTDATA();
	}
	return res;
}

/*list files from RAM. Can be filtered using "pattern". Eg.: *.txt. NOT CASE SENSITIVE!*/
int listModemFile(char *pattern){
	int res=0;
	#if defined(UNO_MODE)
		char lbuff [160];
	#else
		char lbuff [1024];
	#endif
	sprintf(lbuff,"+QFLST=\"RAM:%s\"\r",pattern);
	//myDebugLN(f);
	//res = sendATcommand(lbuff, "OK","ERROR",5);
	res = sendATcommand(lbuff);
	if(res==1){//here process list files
		memset (lbuff,0x00, sizeof(lbuff));
		char * pch0;
		char * pch1;
		pch0 = strstr(buffd,"+QFLST:");
		pch1 = strstr(buffd,"OK");
		strncpy(lbuff, pch0, pch1-pch0-2);//extract header(AT+QFLST=..) and footer(OK) information
		clearBUFFD();
		memcpy(buffd, lbuff, strlen(lbuff));
	}
	return res;
}
