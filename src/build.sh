rm ./shm_conf
rm ./intelligent_app
CC=/home/wooya/work/intelligent_app/OpenWrt-Toolchain-imx6_gcc/toolchain-arm_cortex-a9+neon_gcc-4.8-linaro_uClibc-0.9.33.2_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc
$CC main/main.c main/shm_opt.c main/uci_opt.c main/conf_opt.c main/dev_opt.c main/cmd_opt.c main/file_opt.c main/history_opt.c main/socket_opt.c main/gps_opt.c -I main/include/ -ldl -lpthread -o intelligent_app 
$CC share/intelligent_conf.c -I main/include/ -o shm_conf