#include"libvghubpd.h"
#include<stdio.h>
#include<unistd.h>
#include<time.h>

//使うポートの数
#define N_PORT 7
#define N 20
void FuncPortEventNotify(PORT_ID portID, PORT_EVENT_BITMAP bmPortEvents);
int SetPowerPole(PORT_ID portID,uint8_t Role);
int SetPortConfig(PORT_ID portID,uint8_t Role, uint32_t SourceMax, uint32_t SinkMax);

void main(){
	int i;
		/*
	VGHUBPD_STATUS vghubpd_statusvg = VGHUBPD_Init();
    if(vghubpd_statusvg == VGHUBPD_STATUS_SUCCESS){
 		printf("finish init\n");
	}
	if(vghubpd_statusvg != VGHUBPD_STATUS_SUCCESS){
 		printf("VGHub init error\n");
 		return 0;
	}
	printf("\n");
	*/
	VGHUBPD_STATUS VGHUBPD_Status_Val = VGHUBPD_Init(); // おまじない
	if(VGHUBPD_STATUS_SUCCESS==VGHUBPD_Status_Val){
 		printf("#### VGHub init success ####\n");
	}else{
 		printf("#### VGHub init error ####\n");
 		//return 0;
	}
	printf("---------------------\n");
	VGHUBPD_SetEventNotificationCallback(FuncPortEventNotify); //コールバックの登録，おまじない

	PORT_INFO information[N_PORT]; //PORT_INFO型変数の定義
	VGHUBPD_STATUS getinformation[N_PORT]; //VGHUBPD_STATUS型の定義
	
	/*SWAPPORT →ロールスワップしたいポート番号　しない場合-1
	int SWAPPORT =-1;
	if(SWAPPORT != -1){
		VGHUBPD_STATUS SWAPC= VGHUBPD_SendPRSwap(SWAPPORT,1);
		printf("Swapped the Role of %d     STATUS = %d\n", SWAPPORT, SWAPC);
		sleep(1);
	}	
	*/
		
	//左から　ポートID、ロール（Source→0、Sink→1）、Source最大W、Sink最大W
	// SINKはVGHUBがもらう方　SOURCEは渡す方
	VGHUBPD_STATUS sets0 = SetPortConfig(0,1,0,30);
	//VGHUBPD_STATUS sets1 = SetPortConfig(1,0,60,0);
	//VGHUBPD_STATUS sets2 = SetPortConfig(2,1,0,30);
	//VGHUBPD_STATUS sets3 = SetPortConfig(3,1,0,15);
	//VGHUBPD_STATUS sets4 = SetPortConfig(4,1,0,15);
	//VGHUBPD_STATUS sets5 = SetPortConfig(5,1,0,15);
	VGHUBPD_STATUS sets6 = SetPortConfig(6,0,18,0);

	//sleep(1);
	
	printf("#### Seted PortConfig ####\n");
	printf("---------------------\n");
	//ポート情報を取得
	for(i=0;i<N_PORT;i++){
		getinformation[i] = VGHUBPD_GetPortInfo(i, &information[i]);
	}
	printf("---------------------\n");
	printf("getinfocheck:%d %d %d\n",getinformation[0],getinformation[1],getinformation[2]);
	printf("Power Role\n");

	//本来VGHubではなく向こうの立場に立ってのSink Sourceなんだけどわかりにくいので反転
	for(i=0; i<N_PORT;i++){
		if(information[i].PowerRole == 1){
			printf("Port %d: Source Role  %d %d Connect:%d  PDCPower%d SMBE%d UFp%d DFP%d PDC%d\n", i,information[i].SourceEnabled, information[i].SinkEnabled, 
				   information[i].PortConnect,information[i].PDCPower,information[i].SMBusError,information[i].UFPEnabled,information[i].DFPEnabled,information[i].PDContract);
		}else if(information[i].PowerRole == 0){
			printf("Port %d: Sink Role %d %d Connect:%d PDCPower%d SMBE%d UFp%d DFP%d PDC%d\n", i,information[i].SourceEnabled, information[i].SinkEnabled, 
					information[i].PortConnect,information[i].PDCPower,information[i].SMBusError,information[i].UFPEnabled,information[i].DFPEnabled,information[i].PDContract);
		}
	}
	
	printf("---------------------\n");

	//sleep(1);
	
	//update
	for(i=0;i<N_PORT;i++){
		getinformation[i] = VGHUBPD_GetPortInfo(i, &information[i]);
		printf("%d ",getinformation[i]);
	}
	
	//GetPortInfoで更新した最大能力を表示する
	printf("Max Power\n");
	for(i=0;i<N_PORT;i++){
		printf("Port %d:Source>%dmW Sink>%dmW \n",
			   				i,information[i].SourceMaxPower,information[i].SinkMaxPower);
	}
	
	printf("---------------------\n");

	FILE *fp;
	
	char filename[100];
	char aaa[100] = "a";
	//filename="record.csv";
	printf("file name?: ");
	scanf("%s", filename);
	double sampling_time=0.001;
	printf("-------------\n");
	
	if((fp=fopen(filename,"a")) !=NULL){
		//mili volt current
		fprintf(fp,"unix_time,time_count,PORT[0]C,PORT[0]V,PORT[1]C,PORT[1]V,PORT[2]C,PORT[2]V,PORT[3]C,PORT[3]V,PORT[4]C,PORT[4]V,PORT[5]C,PORT[5]V,PORT[6]C,PORT[6]V\n");
	}
	
	double count = 0;
	int watt[N_PORT];
	int input, output;
	time_t t =time(NULL);
	double num=(double)N;
		
	while(count<N){
		input=0;
		output=0;

		PORT_VBUS_INFO v_information[N_PORT];
		VGHUBPD_STATUS getvinformation[N_PORT];
		
		//GetPortVbusInfoで取得した状態を表示　
		/*for(i=0;i<N_PORT;i++){
			getvinformation[i] = VGHUBPD_GetPortVbusInfo(i, &v_information[i]);
			printf("PortID: %d\n Voltage: %dV\n Current: %dmA\n Watt: %dmW\n", 
					i, v_information[i].Voltage/1000,v_information[i].Current,  (v_information[i].Voltage*v_information[i].Current)/1000);
		}*/
		
		//printf("\n");
	
		for(i=0;i<N_PORT; i++){
			if(watt[i] <0){
				input+= watt[i];
			}
			else if(watt[i] >0){
				output += watt[i];
			}
		}
		
		//printf("Input:  %dmW\nOutput: %dmW\nEfficiency: %f%\n\n",-input,output,-(double)output/(double)input *100);	
		t =time(NULL);
		fprintf(fp,"%d,%lf,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", t, count,
				v_information[0].Current,v_information[0].Voltage,
				v_information[1].Current,v_information[1].Voltage,
				v_information[2].Current,v_information[2].Voltage,
			   	v_information[3].Current,v_information[3].Voltage,
				v_information[4].Current,v_information[4].Voltage,
			   	v_information[5].Current,v_information[5].Voltage,
			   	v_information[6].Current,v_information[6].Voltage);

		count+=sampling_time;
		//printf("%lf",count);
		
		usleep(sampling_time*1000*1000);
	
	}
	fclose(fp);
		/*
	MIDB_INFO m_infomation[N_PORT];
	VGHUBPD_STATUS getmanuinformation[N_PORT];
	
	foqr(i=0;i<N_PORT;i++){
		getmanuinformation[i]= VGHUBPD_GetFarEndPortManuInfo(i, &m_infomation[i]);
		if(getmanuinformation[i] == 0){
			printf("PORT %d:\nVID:%d \nPID:%d \nManufacture:%s\n%d\n\n",
								i, m_infomation[i].VID, m_infomation[i].PID , m_infomation[i].ManufacturerString,
								getmanuinformation[i]);
		}
		else{
			printf("faii to get the data of ManuInfo of %d\n",i);
		}
		
		sleep(2);
	}
				*/
	printf("\n");
	VGHUBPD_STATUS VGHUBPD_Status_Val_Exit = VGHUBPD_Exit();
   	if(VGHUBPD_Status_Val_Exit == VGHUBPD_STATUS_SUCCESS){
 		printf("#### VGHub exit success ####\n");
	}else{
		printf("#### VGHub exit error ####\n");
		
	}
	return;
}



void FuncPortEventNotify(PORT_ID portID, PORT_EVENT_BITMAP bmPortEvents){
	printf("Callback %d %d\n", portID,bmPortEvents);
	printf("che %d\n ",bmPortEvents.event.PDContracted	);
	//if( bmPortEvents.event.DataRoleChanged ==1){
		//printf("Changed the port connect of %d", portID);
	//}
}

int SetPowerPole(PORT_ID portID,uint8_t Role){
	PORT_INFO information;
	VGHUBPD_STATUS getinformation;
	VGHUBPD_STATUS SWAPC=-1;
	getinformation = VGHUBPD_GetPortInfo(portID, &information);
	//printf("RoleCheck: %d %d %d\n",portID , information.PowerRole , Role);

	//informationは向こう側のRole SWAPはこちら側のRole
	if(information.PowerRole == Role){
		SWAPC = VGHUBPD_SendPRSwap(portID,1);
		printf("Try to Role Swap of %d   >%d",portID,SWAPC);
	}
	sleep(1);
	return SWAPC;
}


int SetPortConfig(PORT_ID portID,uint8_t Role, uint32_t SourceMax, uint32_t SinkMax){
	VGHUBPD_STATUS sets;
		
	VGHUBPD_STATUS enable1 = VGHUBPD_EnablePort(portID,0);
	printf("disenabled Port%d   %d\n",portID,enable1);
	sleep(1);
	if(Role == 0){
		sets = VGHUBPD_SetPortPowerConfig(portID,1,0,SourceMax*1000,0,1);
		printf("Set the config of port%d   %d Source\n ",portID,sets);
	}
	else if(Role == 1){
		sets = VGHUBPD_SetPortPowerConfig(portID,0,1,0,SinkMax*1000,1);
		printf("Set the config of port%d   %d Sink\n ",portID,sets);

		
	}
	sleep(1);
	enable1 = VGHUBPD_EnablePort(portID,1);
	printf("enabled Port%d   %d\n",portID,enable1);
	sleep(1);	

 return enable1;
}
