#include "tft.h"
#include <regex>
#include <vector>
#include "chi_def.h"

#include "CppMPCPlayAPI.h"
#include "Player.h"
#include "CppMPCPlay.h"
#include <pthread.h>
#include <iconv.h>

using namespace std;


MPCPlayer mpcplayer;
PyObject *lizhifm;
pair<string, string> url;
int flags = 0;
//vector<string> playlist_img_path;
string song_img_path = "./Cover/sleep";
char *name = (char*)"play soon";
char name_cut[31];
char current_name[100];
uint16_t COLOR_CURREN = RED;
pthread_t tid1,tid2;
char * to_str_gbk;
int len;
time_t timep;
struct tm *timenow;

#if 0

//线程２
void *show_img_progress(void *arg)
{
	FILE *stream;
	char  buf[100];
	const char *rate;
	string buf_rate;
	string buf_time;
	string ration;
	const char * show_time_begin;
	const char * show_time_end;
	int n  = 0;
	int times = 0;
//	const char * show_rate;

    const char *pattern_time = "\\w+([-.]\\w+)*:\\w+([-.]\\w+)*/\\w+([-.]\\w+)*:\\w+([-.]\\w+)*";
    const char *pattern_rate = "\\w+([-.]\\w+)*%";

	int flag = 0;
	BACK_COLOR = GREEN;
	POINT_COLOR = RED;
	while(1)
{
		time (&timep);
		timenow = localtime(&timep);
		LCD_ShowNum(240,100,timenow->tm_hour,2);
		LCD_ShowString(256,100,(const char *)":",0);
		LCD_ShowNum(264,100,timenow->tm_min,2);
		LCD_ShowNum(220,116,timenow->tm_year+1900,4);
		LCD_ShowString(252,116,(const char *)"/",0);
		LCD_ShowNum(260,116,timenow->tm_mon,2);
		LCD_ShowString(276,116,(const char *)"/",0);
		LCD_ShowNum(284,116,timenow->tm_mday,2);

		show_bpm(song_img_path.c_str(),0,40,0);
		stream = popen("mpc","r");
		while(fgets(buf,100,stream))
		{
			if (flag == 0)
			{
				name = buf;
				if(strlen(name)>60)
				{
					name = (char*)"welcomee";
				    COLOR_CURREN = BLACK;
				    LCD_Fill_Color(0,200,320,220,&COLOR_CURREN);
				}
				//判断歌曲名字更新（切歌）
				if(strcmp(name,current_name) != 0)
				{
					to_str_gbk = (char*)calloc(1, strlen(name) * 3);
					if (-1 == (len = charset_convert("UTF-8", "GBK", name,
						strlen(name), to_str_gbk, strlen(name) * 3))) {
						perror("UTF8=>GBK error");
					}
				    int x_star = (320-strlen(to_str_gbk)*8)/2 + 4;
				    to_str_gbk[strlen(to_str_gbk)] = NULL;
				    to_str_gbk[strlen(to_str_gbk)-1] = NULL;
				    COLOR_CURREN = BLACK;
				    LCD_Fill_Color(0,200,320,220,&COLOR_CURREN);
				    display_hzk_asc(x_star,203,(unsigned char *)to_str_gbk,1);
					strcpy(current_name,(const char*)name);
			    }
			}
			else if (flag == 1)
			{
				rate = buf;
				Regex(pattern_rate,rate,buf_rate);
				Regex(pattern_time,rate,buf_time);
				ration = buf_rate;
				int pos = buf_rate.find("%");
				if(pos > -1)
				{
					buf_rate.erase(pos,1);
					n = atoi(buf_rate.c_str());
				}
				vector<string> b = split(buf_time,"/");
				show_time_begin = b[0].c_str();
				show_time_end = b[1].c_str();
				//擦除进度
				LCD_Fill_Color(74+1.55*n, 210+15,85+1.55*n, 218+15,&COLOR_CURREN);
				//进度条
				LCD_Fill_Color(85, 213+15, 240, 214+15,&BACK_COLOR);
				//进度
				Draw_Circle(85+1.55*n,214+15,4);
				Draw_Circle(85+1.55*n,214+15,3);
				Draw_Circle(85+1.55*n,214+15,2);
				Draw_Circle(85+1.55*n,214+15,1);
				//显示时间,进度
				BACK_COLOR = BLACK;
				LCD_ShowString(0,205+15, show_time_begin,0);
				LCD_ShowString(32,205+15, "/",0);
				LCD_ShowString(40,205+15, show_time_end,0);
				LCD_ShowString(260,205+15, (const char*)ration.c_str(),0);
				BACK_COLOR = GREEN;
				if(n>=99)
				{
					LCD_Fill_Color(75, 208+15, 240, 220+15,&COLOR_CURREN);
				}
				if(n == 0 &&times ==0)
				{
					LCD_Fill_Color(75, 207+15, 240, 220+15,&COLOR_CURREN);
					LCD_Fill_Color(255, 208+15, 320, 218+15,&COLOR_CURREN);
					LCD_Fill_Color(0, 208+15, 95, 218+15,&COLOR_CURREN);
					times+=1;
				}
				if (n==1)
				{
					times = 0;
				}
			}
			flag++;
		}
		flag = 0;
		pclose( stream);
		sleep(1);
//		if (pthread_cancel(tid1)==0)  // 线程tid2向线程tid1发送cancel
//				printf("Send Cancel cmd to Thread 1.\n");
	}
	pthread_exit((void *)0);
}

//线程１
void  *show_playlist(void *arg)
{
	// 设置其他线程可以cancel掉此线程
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	while(1)
	{
		flags = run_main_thread(mpcplayer, lizhifm, url);
		if (flags == 1) {
	      break;
		}
		song_img_path = mpcplayer.get_song_img_path();
		cout << song_img_path << endl;
	}
	end();
    pthread_exit((void *)0);
}

void int_sys()
{
	/*----------------int spi & lcd -----------------*/
	pwm_init();
	usleep(500 * 1000);
	/*--------------------tft init---------------------*/
	Lcd_Init();
	usleep(500 * 1000);
	/*--------------------------------------------------*/
	show_bpm("/home/widora/pic/timg2.bmp",0,0,0);
	sleep(2);
	show_bpm("/home/widora/pic/part.bmp",0,40,0);
	/*--------------Chinese display---------------*/
	display_hzk_asc(130,206,(unsigned char *)name,1);
	/************************************/
//	LCD_Fill_Color(0, 208+15, 320, 220+15,&COLOR_CURREN);
//	LCD_Fill_Color(85, 213+15, 240, 214+15,&BACK_COLOR);
	/***********about the player************/
	  start("/home/widora");
	  url = run(mpcplayer, lizhifm);
	/************************************/
}

int main(int argc,char** argv)
{
	BACK_COLOR = GREEN;
	POINT_COLOR = RED;
	int_sys();
    // 创建线程tid1，线程函数thrd_func1
    if (pthread_create(&tid1,NULL,show_playlist,NULL)!=0) {
        printf("Create thread 1 error!\n");
        exit(1);
    }
    // 创建线程tid2，线程函数thrd_func2
    if (pthread_create(&tid2,NULL,show_img_progress,NULL)!=0) {
        printf("Create thread 2 error!\n");
        exit(1);
    }
    // 等待线程tid1退出
    if (pthread_join(tid1,NULL)!=0){
        printf("Join thread 1 error!\n");
        exit(1);
    }else
    {
        LCD_Fill_Color(0,220,320,240,&COLOR_CURREN);
        printf("Thread 1 Joined!\n");
    }
    // 等待线程tid2退出
    if (pthread_join(tid2,NULL)!=0){
        exit(1);
    }else
        printf("Thread 2 Joined!\n");
	SPI_Close();
	return 0;
}

#endif

#if 1
//线程１
void  *show_playlist(void *arg)
{
	// 设置其他线程可以cancel掉此线程
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	while(1)
	{
		flags = run_main_thread(mpcplayer, lizhifm, url);
		if (flags == 1) {
	      break;
		}
		song_img_path = mpcplayer.get_song_img_path();
		cout << song_img_path << endl;
	}
	end();
    pthread_exit((void *)0);
}

//线程２
void *show_img_progress(void *arg)
{
	FILE *stream;
	char  buf[100];
	const char *rate;
	string buf_rate;
	string buf_time;
	string ration;
	const char * show_time_begin;
	const char * show_time_end;
	int n  = 0;
	int times = 0;
//	const char * show_rate;

    const char *pattern_time = "\\w+([-.]\\w+)*:\\w+([-.]\\w+)*/\\w+([-.]\\w+)*:\\w+([-.]\\w+)*";
    const char *pattern_rate = "\\w+([-.]\\w+)*%";

	int flag = 0;

	LCD_ShowString(0,277-6,(const char *)"     ",0);
	LCD_ShowString(0,293-6,(const char *)"     ",0);
	LCD_ShowString(205,285-6,(const char *)"     ",0);
	while(1)
	{
		show_bpm(song_img_path.c_str(),20,60,1);
		stream = popen("mpc","r");
		while(fgets(buf,100,stream))
		{
			if (flag == 0)
			{
				name = buf;
				if(strlen(name)>30 && strlen(name)<60)
				{
					memcpy(name_cut,name,30);
					name = name_cut;
				}
				else if(strlen(name)>60) name = "WELCOME BACK ";
				//判断歌曲名字更新（切歌）
				if(strcmp(name,current_name) != 0)
				{
					LCD_Fill_Color(30, 10, 200, 34,&BACK_COLOR);
					to_str_gbk = (char*)calloc(1, strlen(name) * 3);
					if (-1 == (len = charset_convert("UTF-8", "GBK", name,
						strlen(name), to_str_gbk, strlen(name) * 3))) {
						perror("UTF8=>GBK error");
					}
				    to_str_gbk[strlen(to_str_gbk)] = NULL;
				    to_str_gbk[strlen(to_str_gbk)-1] = NULL;
				    COLOR_CURREN = BLACK;
				    display_hzk_asc(30,15,(unsigned char *)to_str_gbk,0);
					strcpy(current_name,(const char*)name);
			    }
			}
			else if (flag == 1)
			{
				rate = buf;
				Regex(pattern_rate,rate,buf_rate);
				Regex(pattern_time,rate,buf_time);
				ration = buf_rate;
				int pos = buf_rate.find("%");
				if(pos > -1)
				{
					buf_rate.erase(pos,1);
					n = atoi(buf_rate.c_str());
				}
				vector<string> b = split(buf_time,"/");
				show_time_begin = b[0].c_str();
				show_time_end = b[1].c_str();
				//进度条
				COLOR_CURREN = RED;
				POINT_COLOR = COLOR_CURREN;
				LCD_DrawLine(40, 285, 40+1.6*n, 286);
				LCD_DrawLine(40, 286, 40+1.6*n, 287);
				POINT_COLOR = WHITE;
				//进度
//				Draw_Circle(40+1.6*n,285,4);
//				Draw_Circle(40+1.6*n,285,3);
//				Draw_Circle(40+1.6*n,285,2);
//				Draw_Circle(40+1.6*n,285,1);
				//显示时间,进度
				LCD_ShowString(0,277-6,show_time_begin,0);
				LCD_ShowString(0,293-6,show_time_end,0);
				LCD_ShowString(205,285-6,(const char*)ration.c_str(),0);
//				if(n>=99)
//				{
//					LCD_Fill_Color(75, 208+15, 240, 220+15,&COLOR_CURREN);
//				}
				if(n == 0 &&times ==0)
				{
					POINT_COLOR = GRAY;
					LCD_Fill_Color(40, 283, 210, 288,&BACK_COLOR);
					LCD_DrawLine(40, 285, 200, 286);
					LCD_DrawLine(40, 286, 200, 287);
					LCD_ShowString(0,277-6,(const char *)"     ",0);
					LCD_ShowString(0,293-6,(const char *)"     ",0);
					LCD_ShowString(205,285-6,(const char *)"     ",0);
					POINT_COLOR = WHITE;
					times+=1;
				}
				if (n==1)
				{
					times = 0;
				}
			}
			flag++;
		}
		flag = 0;
		pclose( stream);
		sleep(1);
//		if (pthread_cancel(tid1)==0)  // 线程tid2向线程tid1发送cancel
//				printf("Send Cancel cmd to Thread 1.\n");
	}
	pthread_exit((void *)0);
}

void int_sys()
{
	/*----------------int spi & lcd -----------------*/
	pwm_init();
	usleep(500 * 1000);
	/*--------------------tft init---------------------*/
	Lcd_Init();
	usleep(500 * 1000);
	/*--------------------------------------------------*/
	show_bpm("/home/widora/pic/vertical.bmp",0,0,1);
	sleep(1);
	show_bpm("/home/widora/pic/back.bmp",0,0,1);
	sleep(1);

    lcddev.width = 240;
    lcddev.height = 320;

	POINT_COLOR = WHITE;
	BACK_COLOR = GREEN;
	LCD_DrawLine(30, 40, 210, 41);

	LCD_DrawLine(6, 23, 25, 23);
	LCD_DrawLine(6, 23, 14, 14);
	LCD_DrawLine(6, 23, 14, 32);

	LCD_DrawLine(210, 15+5, 222, 8+5);
	LCD_DrawLine(210, 15+5, 222, 22+5);
	Draw_Circle(210,15+5,3);
	Draw_Circle(222,8+5,3);
	Draw_Circle(222,22+5,3);

	//进度条
	POINT_COLOR = GRAY;
	LCD_DrawLine(40, 285, 200, 286);
	LCD_DrawLine(40, 286, 200, 287);
	//时间
	LCD_ShowString(0,277-6,(const char *)"00:00",0);
	LCD_ShowString(0,293-6,(const char *)"00:00",0);
	LCD_ShowString(205,285-6,(const char *)"00%",0);

	char* song_name = (char *)"别说－李玖哲";
	to_str_gbk = (char*)calloc(1, strlen(song_name) * 3);
	if (-1 == (len = charset_convert("UTF-8", "GBK", song_name,
		strlen(song_name), to_str_gbk, strlen(song_name) * 3))) {
		perror("UTF8=>GBK error");
	}
	display_hzk_asc(30,15,(unsigned char*)to_str_gbk,0);
	POINT_COLOR = WHITE;

	show_bpm("/home/widora/Cover/sleep",20,60,1);
	sleep(1);
	/***********about the player************/
	  start("/home/widora");
	  url = run(mpcplayer, lizhifm);
	/************************************/
}

int main()
{
	int_sys();
	 // 创建线程tid1，线程函数thrd_func1
	    if (pthread_create(&tid1,NULL,show_playlist,NULL)!=0) {
	        printf("Create thread 1 error!\n");
	        exit(1);
	    }
	    // 创建线程tid2，线程函数thrd_func2
	    if (pthread_create(&tid2,NULL,show_img_progress,NULL)!=0) {
	        printf("Create thread 2 error!\n");
	        exit(1);
	    }
	    // 等待线程tid1退出
	    if (pthread_join(tid1,NULL)!=0){
	        printf("Join thread 1 error!\n");
	        exit(1);
	    }else
	    {
	        printf("Thread 1 Joined!\n");
	    }
	    // 等待线程tid2退出
	    if (pthread_join(tid2,NULL)!=0){
	        exit(1);
	    }else
	        printf("Thread 2 Joined!\n");
		SPI_Close();
	return 0;
}

#endif
