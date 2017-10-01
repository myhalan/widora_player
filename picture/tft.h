/*-------------------------------------------------
  RM68140 controlled LCD display  and  pin-control functions

------------------------------------------------*/
#ifndef _TFT_H
#define _TFT_H

#include <iostream>
#include <stdio.h>
#include <stdint.h> // data type
#include <unistd.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include "spi.h"
#include "LCD.h"
#include "sys.h"
#include "font.h"
#include "pwmconfig.h"
#include "time.h"
#include <signal.h>
#include <dirent.h>
#include <string>
#include <regex.h>
#include<stdlib.h>
#include <unistd.h>
#include <vector>

using namespace std;

#define BUFFSIZE 1024 // --- to be times of SPIBUFF
#define SPIBUFF 33 //-- spi write buff-size for SPI_Write()
#define SPIBUFF_new 240*3
//#define SPIBUFF 720  //-- spi write buff-size for SPI_Write()
#define STRBUFF 256   // --- size of file name
#define MAX_WIDTH 240
#define MAX_HEIGHT 320

int change_to_RGB16();
int show_bpm(const char* filepath,uint16_t sx,uint16_t sy,uint8_t rotation);
//int show_bpm(const char* filepath,uint16_t sx,uint16_t sy,uint8_t mode);
int Regex(const char *pattern,const char *buf,string &buff);
vector<string> split(string& str,const char* c);
int charset_convert(const char *from_charset, const char *to_charset,char *in_buf, size_t in_left, char *out_buf, size_t out_left);

//clock_t startTime = clock();
//clock_t endTime = clock();
//cout << "\n Totle Time : " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
#endif



