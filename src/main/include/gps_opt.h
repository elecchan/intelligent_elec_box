#ifndef _GPS_OPT_H 
#define _GPS_OPT_H

#define GPS_DEV "/dev/ttymxc1"

struct GPRMC{
	unsigned int time;          //时间 时分秒
    char pos_state;             //定位状态 'A'定位成功 'V'定位失败
    float latitude;             //纬度 北纬
    float longitude;            //经度 东经
    float speed;                //移动速度
    float direction;            //方向
    unsigned int date;          //日期 年月日
    float declination;          //磁偏角
    char dd;                    //磁偏角方向
    char mode;
};

struct gps_Opt{
	int fd;
	pthread_t pthread_gps;
	struct GPRMC gprmc;
};
struct gps_Opt gps_opt;

int gps_opt_init(void);
void gps_opt_deinit(void);
int gps_opt_gprmc (char *buff,struct GPRMC *gprmc);

#endif