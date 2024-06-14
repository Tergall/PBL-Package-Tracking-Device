/*
agsmII_DTMF_lbr.ino v 0.96/20170323 - a-gsmII 2.105 LIBRARY SUPPORT
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

int sendDTMF(char* message){
	memset(readBuffer,0x00,sizeof(readBuffer));
	sprintf(readBuffer,"+QWDTMF=6,0,\"%s,%i,%i\"", message, DTMFlenght, DTMFpause);//format the send DTMF string
	if(sendATcommand(readBuffer,"OK","ERROR",10)==1){
		Serial.println(F("DTMF send"));
		return 1;
	}
	return 0;
}

/*
Listen for DTMF until "terminator" has been found or "to" (in secs) timeout reached
return: int 
  -1 TIMEOUT
  1  SUCCESS
read DTMF string => DTMF
*/
int listen4DTMF(char * DTMF, char * terminator, int to){
  int run = 1;
  int res = 0;
  int i=0, j=0;
  unsigned long startTime;
  char u8_c;
  char* pch0;
  char* pch1;
  char DTMFstr[3];
  char DTMFint;
  
  clearBUFFD();
  startTime = millis();	
  while(run){
    if(millis() - startTime > (unsigned long)to *1000) {
      #if defined(atDebug)
        Serial.println("to!");
      #endif
      clearagsmSerial();
      run=0;
      res=-1;//timeout!
    }
    
    while(TXavailable()){//read it baby
      u8_c = aGsmREAD();
      buffd[i]=u8_c;
      i++;
      if(u8_c == 0x0D) {//found EOL, let's process it
        if (strstr(buffd,"+QTONEDET:")){
            pch0 = strstr(buffd,"+QTONEDET:");
            pch1 = strstr(buffd,"\r\n");
            memset(DTMFstr,0x00,sizeof(DTMFstr));
            strncpy(DTMFstr, pch0+11, 2);
            //Serial.print("dir: ");Serial.flush();
            //Serial.println(DTMFstr);Serial.flush();
            DTMFint = (char) atoi(DTMFstr);
            DTMF[j] = DTMFint;
            //Serial.print("conv: ");
            //Serial.println(DTMF);
            j++;
            if(strstr(DTMF,terminator)){//found logical terminator!
                //Serial.println("found");
                clearagsmSerial();
                run=0;//break loop
                res=1;//go back with success
            }
        }
        clearBUFFD();//prepare for and go to receive next DTMF char
        i=0;
        break;
      }
    }
  }
  return(res);
}

/*disable the DTMF detection*/
void disableDTMFdetection(){
	int res = 0;
	res = fATcmd(F("+QTONEDET=0"));//disable DTMF detection 
	if(res==1) Serial.println(F("DTMF detection disabled"));
}
