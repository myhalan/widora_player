CC = mipsel-openwrt-linux-g++ -std=c++11
CPP = player
CPP_O = 
OBJS = $(CPP).o

# Enter your python head doc path
PYTHON_INCLUDE = /home/alan/player/API/python-install/include/python2.7

# Enter your python lib path
PYTHON_LIB_STATIC = /home/alan/player/API/python-install/lib/python2.7/config

# Enter your libz path
LIBZ_A = /home/alan/player/API/python-install/lib

#Enter the defition of the function for the tft_lcd_driver
CFLAGS += -I./font -I./LCD -I./pwm -I./spi -I./sys -I./picture -I./API -I./libiconv-full/include 
LDFLAGS := -lpython2.7 -lpthread -lm -lutil -lnsl -ldl -lz -liconv

#Enter the iconv lib path
LIB_ICONV = /home/alan/player/libiconv-full/lib

#the path of the support function
source = ./font/font.cpp ./LCD/LCD.cpp ./picture/tft.cpp  ./pwm/pwmconfig.cpp ./pwm/pwmtest.cpp ./spi/spi.cpp ./sys/mt76x8_gpio.cpp

Player : $(OBJS) $(source)
	$(CC) $(OBJS)  $(source) -o Player -I $(PYTHON_INCLUDE) $(CFLAGS) -L $(PYTHON_LIB_STATIC) -L $(LIBZ_A) -L $(LIB_ICONV)   $(LDFLAGS) 

$(CPP).o : $(CPP).cpp $(source)
	$(CC) -c $(CPP).cpp  -o $(CPP).o -I $(PYTHON_INCLUDE) $(CFLAGS) -L $(PYTHON_LIB_STATIC) -L $(LIBZ_A) -L $(LIB_ICONV)   $(LDFLAGS) 

clean :
	rm -rf *.o Player





