#ifndef __CONFIG__
#define __CONFIG__
		
	#define MPU_ADDR 0x68
	#define MPU_READ_DIFFTIME_MICRO_SECONDS 2000

	#define PIN_MPU_SDA 5
	#define PIN_MPU_SCL 4 
	#define PIN_LED 16

	#define CONFIG_WIFI_AS_ACCESSPOINT
	#define DEBUG





	#if defined CONFIG_WIFI_AS_ACCESSPOINT

		#define WIFI_SSID "Mote_01"
		#define WIFI_USE_PASS false
		#define WIFI_PASS "TannerHelland"

	#elif defined CONFIG_WIFI_AS_NODE

		#define WIFI_SSID "Tardis"
		#define WIFI_USE_PASS true
		#define WIFI_PASS "geronimo"

	#endif

#endif