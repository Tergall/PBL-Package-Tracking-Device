/*
agsmII_IP_lbr.ino v 0.97/20170323 - a-gsmII 2.105 LIBRARY SUPPORT
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

void setSSLMODE(int SSLMODE){
	memset(HTTP_PROTOCOL,0x00,sizeof(HTTP_PROTOCOL)); 
	memset(SERVER_PORT,0x00,sizeof(SERVER_PORT)); 
	if(SSLMODE==1){//SSL enabled
	  	sprintf(HTTP_PROTOCOL,"%s","https://");
		sprintf(SERVER_PORT,"%s","443");
	}else{
	  	sprintf(HTTP_PROTOCOL,"%s","http://");
		sprintf(SERVER_PORT,"%s","80");
	}
}

/* prepare modem for IP usage*/
void prepareIP(){
	agsmSerial.println(F("AT+QIMODE=0"));delay(200);      
	agsmSerial.println(F("AT+QINDI=0"));delay(200);      
	agsmSerial.println(F("AT+QIMUX=0"));delay(200);      
	agsmSerial.println(F("AT+QIDNSIP=1"));delay(200);      
	clearagsmSerial();
}

/*
	check send data ACK...
	IP_STATE  <== 1-9 - IP/socket transmission status
	1	- modem ok
	<0	- modem freezed...resetModem() + restartMODEM()
*/
int getIPState(){
	//aGsmCMD("ATE0",20);
	//aGsmCMD("ATV0",200);
	agsmSerial.println(F("ATE0"));delay(20);
	agsmSerial.println(F("ATV0"));delay(200);
	clearagsmSerial();
	//aGsmCMD("AT+QISTAT",50);
	agsmSerial.println(F("AT+QISTAT"));delay(50);
	if(readline(5) < 1) { //emergencyReset();
          return -1;
	}
	ch = buffd[2];
	IP_STATE = (int)ch-0x30;
	//Serial.println(buffd);
	agsmSerial.println(F("ATE1"));delay(20);
	agsmSerial.println(F("ATV1"));delay(20);
	clearagsmSerial();
	//Serial.print(F("retrived state >> "));
	//Serial.println(IP_STATE);
	delay(200);
	return 1; 
}

/*
	check send data ACK...
	0 	- socket transmission fine
	>0 	- some bytes waits for ack.... AT+QIDEACT + openSocket()
	<0	- modem freezed...resetModem() + restartMODEM()
	check for AT+QIMUX=0
*/
int getIPAck(){//looks r third par in +QISACK: 0, 0, 0
	int res = 0;
	char tmpChar[10];
	memset(tmpChar,0x00, sizeof(tmpChar));
	//res = sendATcommand("+QISACK","OK","ERROR",4);
	res = fATcmd(F("+QISACK"));
	if (res > 0){//process data
		parseResponce("OK", "+QISACK:", tmpChar, ",", 2);
		//Serial.println(tmpChar);
		return atoi(tmpChar);
	}else return res-1;//error processing data
}
/* return:
	1 	connected
	0 	connect fail
	-1	timeout
*/
int socketOpen(){//todo ==> update initial server port value
	int res=0;
	memset(readBuffer, 0x00, sizeof(readBuffer));
	Serial.print(F("Socket open >>> "));Serial.flush();
	sprintf(readBuffer,"+QIOPEN=\"%s\",\"%s\",\"%s\"",SERVER_PROTOCOL,SERVER_ADDRESS,SERVER_PORT);
	res = sendATcommand(readBuffer,"CONNECT OK","CONNECT FAIL",TCPCONNECTTIMEOUT);
	//Serial.println(buffd);Serial.flush();
	switch(res){
		case 1:
			Serial.println(F("succeed"));Serial.flush();
		break;
		case 0: //failed
			Serial.println(F("failed"));Serial.flush();
			socketClose();
		break;
		default: //timeout
			//HERE, ALSO TOO, WHEN "ERROR" RECEIVED ==> CREDIT...
			Serial.println(F("timeout"));Serial.flush();
			socketClose();
		break;
	}
	return res;
}
void socketClose(){
	//aGsmCMD("AT+QICLOSE",2500);
	agsmSerial.println(F("AT+QICLOSE"));delay(2500);
	clearagsmSerial();
}

/*
	AT+QIDEACT
	deactivate PDP
	IP_STATE goes to IP INITIAL
*/

int qiDeact(int timeout){//if retval !=1, emergencyReset
	int res;
	//res = sendATcommand("+QIDEACT","OK","ERROR",timeout);
	res = fATcmd(F("+QIDEACT"),timeout,"OK","ERROR");
	if(res == 1) {
		//IP_STATE = IPINITIAL;
		Serial.println(F("PDP context inactive"));
	}else{
		Serial.println(F("PDP dezactivation failed/timeout"));
		emergencyReset();
	}	
	return res;
}
/*
	AT+QIREGAPP...
	deactivate PDP
	IP_STATE goes to IP START
*/
int qiRegapp(int timeout){//if retval !=1, emergencyReset
	if(IP_STATE != IPINITIAL) return -100;
	int res;
	sprintf(readBuffer, "+QIREGAPP=\"%s\",\"%s\",\"%s\"", GPRS_context, GPRS_user, GPRS_password);//prepare the GPRS context string
	res = sendATcommand(readBuffer,"OK","ERROR",timeout);
	Serial.print(F("Start TCPIP task "));
	if(res == 1) {
		//IP_STATE = IPSTART;
		Serial.print(F("succeed"));
	}else if(res < 0){
		Serial.println(F("timeout"));
	}else 
		Serial.println(F("error"));
	return res;
}
/*
	AT+QIACT
	activate PDP
	IP_STATE goes to IP CONFIG
*/
//int qiAct(int timeout = 10){//if retval !=1, emergencyReset
int qiAct(int timeout /*= 10*/){//if retval !=1, emergencyReset
	int res;
	//res = sendATcommand("+QIACT","OK","ERROR",timeout);
	res = fATcmd(F("+QIACT"),timeout,"OK","ERROR");
	if(res == 1) {
		//IP_STATE = IPCONFIG or IPGPRSACT;
		Serial.println(F("GPRS context activated"));
	}else
		Serial.println(F("GPRS context activation failed/timeout"));
	return res;
}

/*
	AT+QILOCIP
	activate PDP
	IP_STATE goes to IP CONFIG
*/
int qiLocip(int timeout){//if retval !=1, try deactivate first!
	int res;
	clearagsmSerial();
	//aGsmCMD("AT+QILOCIP",10);
	agsmSerial.println(F("AT+QILOCIP"));
	delay(10);
	readline(10);
	res = recUARTDATA("\r\n","ERR",timeout);
	Serial.println(buffd); Serial.flush();
	if(res == 1) {
		//IP_STATE = IPCONFIG;
		Serial.println(F("GET LOCAL IP"));
	}else
		Serial.println(F("dhcp process failed/timeout"));
	return res;
}

/*	try to open the socket for data transmission
	Parameter dependent!!! SEE the "user section" in agsm_kickstart_IP_lbr
	8	SOCKET CONNECT
	-1	timeout processing connection
	-2  Modem has been reset.. you may call again the function
	-3  GPRS is not available/modem it is not registered
*/
int PROCESS_IP(unsigned long timeout){
	prepareIP();
	int res = 0;
	int cnt = 0;
	unsigned long startTime;
	startTime = millis();
ckreg:	Serial.println(F("Check GPRS registration"));
	res = registration(GPRS);	
	if(!(res==1 || res==5)){ 
        	delay(3000);//wait a while
        	if(cnt++>=3) return -3;
        	goto ckreg;//retry check registration
	}
	cnt = 0;

	while(1){
		res = getIPState();
		Serial.print(F("IP STATE >> "));		
		Serial.println(IP_STATE);
		Serial.flush();		
		if(res < 0){//modem re-initialisation needed!
			Serial.println(F("FREEZED?"));
			Serial.flush();		
			if(cnt >= 2){//freezed
				emergencyReset();
				return -2;
			}else{
				agsmSerial.flush();
				clearagsmSerial();
			}
			cnt++;
			continue;
		}else cnt =0;

		if(millis() - startTime > timeout){
				Serial.println(F("Timeout processing GPRS/IP"));
				Serial.flush();		
				clearagsmSerial();
		        return -1;//return timeout
		}
		
		
		switch(IP_STATE){
			case IPINITIAL:		//0-TCPIP IDLE
				res = qiRegapp();
				if(res != 1){//process errors
					getIPState();//IP_STATE
					if( IP_STATE != IPSTART && qiDeact()!=1) continue;
				}
			break;
			case IPSTART:		//1-TCPIP REGISTERED
				res = qiAct();
				if(res != 1){//process errors
					getIPState();//IP_STATE update
					if( IP_STATE != IPGPRSACT && qiDeact()!=1) continue;
				}
			break;
			/*
			case IPIND:			//2-running (activating) GRS/CSD context 
			break;
			*/
			case IPCONFIG:		//3-startup to activate GRS/CSD context
			break;
			case IPGPRSACT:		//4-GRS/CSD context ACTIVATED 
				res = qiLocip();//getLocalIP
				if(res != 1){//process errors
					getIPState();//IP_STATE update
					if( IP_STATE != IPSTATUS && qiDeact()!=1) continue;
				}
				delay(1000);

			break;
			case IPCONNECTING:	//6-SOCKET CONNECTING ==>"TCP CONNECTING" or "UDP CONNECTING" 
				//...ce si cum....timer global...
			break;			
			case IPSTATUS:		//5-local IP address obtained
			case IPCLOSE:		//7-SOCKET CLOSED 
				res = getIPAck();//check socket sanity
				if(res==0) socketOpen();
				else{
					getIPState();//IP_STATE update
					if( !(IP_STATE == IPCONNECTING || IP_STATE == CONNECTOK) && qiDeact()!=1) continue;
				}
			break;
			case CONNECTOK:		//8-SOCKET CONNECTED 
				res = getIPAck();
				if(res==0) {
					Serial.println(F("SOCKET READY!"));
					Serial.flush();		
					clearagsmSerial();
					return(IP_STATE);//8 - socket ready.. exit loop ...
				}
				else qiDeact();				
			break;
			case PDPDEACT:		//9-GRS/CSD context DE-ACTIVATED (unknown reason) 
				//processGPRS(ATTACHGPRS);//try to release GPRS
			break;
			default:			//nothing 
			break;
		}
		delay(500);//some delay
	}
}

int HTTP_REQUEST(char* HTTP_VARIABLES, int METHOD){
	int res=0;
	int connection = PROCESS_IP();
	switch(connection){
		case -1: //
		case -2:
			Serial.print(F("SOCKET FAIL >> "));
			Serial.println(connection);
			Serial.flush();		
		case -3:
			return connection;
		break;
		case CONNECTOK:	//let's do the job
			getIPState();//update IP_STATE
			if(IP_STATE == CONNECTOK && getIPAck()==0) {//the socket is open & errorless
				//prepare POST/GET request
				int totalChars = 0; 

				totalChars = strlen(HTTP_PROTOCOL); 
				totalChars += strlen(SERVER_ADDRESS);//HTTP_PROTOCOL 
				totalChars += strlen(HTTP_PATH);
				if(METHOD == GET){
					totalChars ++; //"?"
					totalChars += strlen(HTTP_VARIABLES);
				}

				char tChars[6];
				memset(tChars, 0x00, sizeof(tChars));
				sprintf(tChars,"%i",totalChars);

				Serial.print(F("send data..."));
				Serial.flush();        
				Serial.print(HTTP_PROTOCOL);
				Serial.print(SERVER_ADDRESS);
				Serial.flush();
				Serial.println(HTTP_PATH);
				Serial.flush();

				agsmSerial.flush();      
				clearagsmSerial();      

				agsmSerial.print("AT+QHTTPURL=");
				agsmSerial.print(tChars);
				agsmSerial.println(",10");//total chars      
				agsmSerial.flush();      
				delay(500);      
				readline(5000); //search for CONNECT     

				agsmSerial.print(HTTP_PROTOCOL); 
				agsmSerial.print(SERVER_ADDRESS); 
				agsmSerial.print(HTTP_PATH);
				if(METHOD == GET){
					agsmSerial.print("?");
					agsmSerial.print(HTTP_VARIABLES);
				}
				agsmSerial.println("");      
				/*aGsmWRITE("AT+QHTTPURL=");
				aGsmWRITE(tChars);
				aGsmCMD(",30",500);//total chars      

				aGsmWRITE(HTTP_PROTOCOL); 
				aGsmWRITE(SERVER_ADDRESS); 
				aGsmWRITE(HTTP_PATH);
				if(METHOD == GET){
					aGsmWRITE("?");
					aGsmWRITE(HTTP_VARIABLES);
				}
				aGsmCMD("",1);*/      

				agsmSerial.flush();
				clearagsmSerial();


				if(METHOD == GET){
					//res = sendATcommand("+QHTTPGET=50","OK","ERROR",20); //wait for ok ---send http get--  20sec
					res = fATcmd(F("+QHTTPGET=50"),20); //wait for ok ---send http get--  20sec
				}
				else{//POST				
					totalChars = strlen(HTTP_VARIABLES);
					memset(tChars, 0x00, sizeof(tChars));
					sprintf(tChars,"%i",totalChars);
					agsmSerial.print("AT+QHTTPPOST=");
					agsmSerial.print(tChars);//total chars
					agsmSerial.println(",30,50"); 
					//aGsmCMD(",30,50", 1); 
					agsmSerial.flush();
					res = recUARTDATA("CONNECT","ERROR",20); //wait for ok ---send http get--  20sec       
				}
				if(res==1){
					if(METHOD != GET){//POST the real data
						//send post data
						delay(250);
						agsmSerial.flush();
						clearagsmSerial();
						agsmSerial.print(HTTP_VARIABLES);
						agsmSerial.println("");      
						agsmSerial.flush();
						//delay(150);
						res = recUARTDATA("OK","ERROR",20);
					} else res = 1;
					if(res==1){//real data has been POSTed    
						//res = sendATcommand("+QHTTPREAD=30","OK","ERROR",32);        
						res = fATcmd(F("+QHTTPREAD=30"),32);        
						if(res==1){
  							Serial.println(F("server return data:\r\n"));Serial.flush();
   							//extract RESPONSE start
  							char* pch0;
  							char* pch1;
   							pch1 = strstr(buffd, "OK");
  							pch0 = strstr(buffd, "CONNECT");
  							pch0 = pch0+7+2;
  							strncpy(buffd, pch0, pch1 - pch0 /*- 2*/ - 2 );
  							buffd[pch1 - pch0 - 2 - 2  +1]=0;
   							//extract RESPONSE stop  
							//Serial.print("\""); Serial.flush();
							Serial.print(buffd); Serial.flush();
							//Serial.println("\""); Serial.flush();
							return 0;//show success
						}else{
							Serial.println(F("some error reading data..."));Serial.flush();        
							Serial.println(buffd); Serial.flush();
							return 1;//show error
						}
					}
					else{//real data post return ERROR or TIMEOUT
						//check for 3813....DNS error
						//check for 3825....TIMEOUT DATA..la POST
						//check for 3822....la GET
						Serial.println(F("can not POST data!"));
						Serial.flush(); 
						return 2;//show error
					}
				}else{
						Serial.println(F("DNS error?!"));Serial.flush();        
						return 3;//show error
				}
				socketClose();//just to be sure
			}else{//NO SOCKET!
				Serial.println(F("no sheet! socket unavailable before send data "));
				Serial.flush();        
				return 10;//show error
			} 
			
		break;
	}
}

int PROCESS_HTTP_REQUEST(char* HTTP_VARIABLES, int METHOD, unsigned long timeout){
	int res=0;
	HTTP_STARTTIME = millis();
	while(millis() - HTTP_STARTTIME < timeout){
		res = HTTP_REQUEST(HTTP_VARIABLES, METHOD);
		if(res>0){//HTTP PROCESS ERRORS
			qiDeact();//deactivate PDP and try to restart the process
		}
		else if(res==-3){//no GPRS registration, try later
			Serial.println(F("no GPRS registration, try later")); return res;break;
		}
		else if(res<0){//IP PROCESS ERRORS
			qiDeact();//deactivate PDP and try to restart the process
		}
		else{//HTTP_REQUEST SUCCED
			return res;break;
		}
	}
	return -1;//show timeout
}
