#ifndef __CONFIG__
#define __CONFIG__

	// Pin configuration
	#define PIN_MPU_SDA 5
	#define PIN_MPU_SCL 4 
	#define PIN_LED 16
	
	// MPU6050 configuration
	#define MPU_ADDR 0x68 // MPU I2C address

	// Time between interrupt triggerings
	#define TIMER0_INTERRUPT_TIME_MICRO_SECONDS 1999 // it takes about a millisecond to start the ISR

	// Enable debug print
	#define DEBUG
	#define TEST

	// WiFi configuration
	#define WIFI_SSID "FootballRelay"
	#define WIFI_USE_PASS false
	#define WIFI_PASS "TannerHelland"
	#define UDP_PORT 8085

	// Encog parser configuration
	#define ENCOG_LABEL_MAX_LENGTH 16 				// Size of buffer wherein ENCOG labels are read into
	#define ENCOG_FORCE_OVERWRITE_MODEL true		// ReWrite the default model on boot
	#define ENCOG_MODEL_FILE_PATH "/model"			// File path of encog model

#endif
