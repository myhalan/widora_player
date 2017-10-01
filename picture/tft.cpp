#include "tft.h"
#include "iconv.h"

using namespace std;

/*将R G B 转换为RGB16*/
int change_to_RGB16()
{
	uint16_t color;
	uint16_t bmpbuf[3];
	bmpbuf[0] = 0xFF;//R
	bmpbuf[1] = 0;//G
	bmpbuf[2] = 0;//B

	color=bmpbuf[0]>>3; //B->R
	color+=((uint16_t)bmpbuf[1]<<3)&0X07E0;//G
	color+=((uint16_t)bmpbuf[2]<<8)&0XF800;//R->B
	printf("0x%04X\n",color);
	return 0;
}

int show_bpm(const char* filepath,uint16_t sx,uint16_t sy,uint8_t rotation) //rotation = 1:竖屏  0:横屏
{
	uint8_t *pmap,*oftemp;
	int fp; //file handle
	int MapLen; // file size,mmap size

	uint32_t total; // --total bytes of pic file
	uint16_t residual; // --residual after total divided by BUFFSIZE
	uint16_t nbuff;
	uint16_t Hs, He, Vs, Ve; //--GRAM area difinition parameters
	uint16_t Hb, Vb; //--GRAM area corner gap distance from origin

	uint8_t buff[8]; //--for buffering  data temporarily
	long offp; //offset position
	uint16_t picWidth, picHeight;

	//*reset file offset for pic width and height*/
	offp = 18;
	/*--------open file and show picture ---------*/
    LCD_WR_REG(0x36);    // Memory Access Control
    //48 68竖屏//28 E8 横屏,BGR(CC)
    if(rotation) LCD_WR_DATA8(0xC0);//竖屏
    else LCD_WR_DATA8(0xA0); //横屏　　RGB
    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x66);//0x55:16位色彩　　0x66:18位色彩

	fp = open(filepath, O_RDONLY);
	//WriteComm(0x3a); WriteData(0x66);
	if (fp < 0)
	{
		printf("\n Fail to open the file!\n");
	}
	else
	/*------seek position and readin picWidth and picHeight-----*/
	if (lseek(fp, offp, SEEK_SET) < 0)
		printf("\n Fail to offset seek position!");
	read(fp, buff, 8);

	picWidth = buff[3] << 24 | buff[2] << 16 | buff[1] << 8 | buff[0];
	picHeight = buff[7] << 24 | buff[6] << 16 | buff[5] << 8 | buff[4];

	/*------------------------------ MMAP -------------------------------*/
	MapLen = picWidth * picHeight * 3 + 54;
	pmap = (uint8_t*) mmap(NULL, MapLen, PROT_READ, MAP_PRIVATE, fp, 0);
	if (pmap == MAP_FAILED)
	{
		printf("\n pmap mmap failed!");
		return 1;
	}
	else

	/*------- WARNING: picWidth must be an 4xtimes number!! -------*/
	Hb=sx;	Vb=sy;
	Hs=Hb; He=Hb+picWidth-1;
	Vs=Vb; Ve=Vb+picHeight-1;
	Set_GRAM_Block(Hs,He,Vs,Ve);  //--GRAM area set

	LCD_WR_REG(0x2C); // --prepare for continous GRAM write

	//----------read RGB data from the file  and write to GRAM ----------*/
	/*total bytes for BGR data*/
	total = picWidth * picHeight * 3;
	nbuff = total / SPIBUFF;
	residual = total % SPIBUFF;

	/*--- start point where BGR data begins in BMP file---*/
	offp=54;
	oftemp=pmap+offp; //--start point to BMP data in mmap

	/*----------------开始连续写操作--------------------*/
	LCD_DC_1;
	/*------------24位色彩转化为16位色彩----------*/

	for (int i = 0; i < nbuff; i++)
	{
		 write(g_SPI_Fd,oftemp+i*SPIBUFF,SPIBUFF);
	}
	if (residual != 0)
	{
		write(g_SPI_Fd,oftemp+nbuff*SPIBUFF,residual);
	}

	munmap(pmap, MapLen);
	close(fp); //--fp will not set to NULL however close

	close(gpio_mmap_fd);

    LCD_WR_REG(0x36);    // Memory Access Control
    if(rotation)  LCD_WR_DATA8(0x40);//竖屏
    else LCD_WR_DATA8(0xE0); //横屏　ＲＧＢ
    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x55);//0x55:16位色彩　　0x66:18位色彩

	return 0;
}

int Regex(const char *pattern,const char *buf,string  &buff)
{
		int status;
	    int cflags = REG_EXTENDED;
	    regmatch_t pmatch[1];
	    const size_t nmatch = 1;
	    regex_t reg;
	    char match[] = {0};
//	    char * buff = NULL;
	    regcomp(&reg,pattern,cflags);//编译正则模式
	    status = regexec(&reg,buf,nmatch,pmatch,0);//执行正则表达式和缓存的比较
	    if(status == REG_NOMATCH)
	        printf("No match\n");
	    else if (0 == status)
	    {
	    	int len = pmatch[0].rm_eo -pmatch[0].rm_so;
	    	if(len)
	    	{
	            memcpy(match,buf+pmatch[0].rm_so,len);
	            char reg[len];
	            for(int i = 0;i<len;i++)
	            {
	            	reg[i] = match[i];
	            }
	            buff = reg;
	    	}
	    }
	    return 0;
}

vector<string> split(string& str,const char* c)
{
    char *cstr, *p;
    vector<string> res;
    cstr = new char[str.size()+1];
    strcpy(cstr,str.c_str());
    p = strtok(cstr,c);
    while(p!=NULL)
    {
        res.push_back(p);
        p = strtok(NULL,c);
    }
    return res;
}

int charset_convert(const char *from_charset, const char *to_charset,
    char *in_buf, size_t in_left, char *out_buf, size_t out_left) {
        iconv_t icd;
        const char *pin = in_buf;
        char *pout = out_buf;
        size_t out_len = out_left;
        if ((iconv_t)-1 == (icd = iconv_open(to_charset,from_charset))) {
        	cout<<"error "<<endl;
            return -1;

        }
        if ((size_t)-1 == iconv(icd, &pin, &in_left, &pout, &out_left)) {
        	cout<<"error 2"<<endl;
            iconv_close(icd);
            return -1;
        }
        out_buf[out_len - out_left] = 0;
        iconv_close(icd);
        return (int)out_len - out_left;
}






