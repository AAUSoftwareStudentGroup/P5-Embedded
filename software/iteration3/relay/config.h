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

	// Encog parser configuration
	#define ENCOG_LABEL_MAX_LENGTH 16 				// Size of buffer wherein ENCOG labels are read into
	#define ENCOG_FORCE_OVERWRITE_MODEL true		// ReWrite the default model on boot
	#define ENCOG_MODEL_FILE_PATH "/model"			// File path of encog model

	// WiFi configuration
	#define WIFI_AP_SSID "FootballRelay"
	#define WIFI_SSID "AAU-1-DAY"
	#define WIFI_PASS "wet76shut"
	#define UDP_PORT 8085
	#define HTTP_HOST "p5datahub.azurewebsites.net"
	#define HTTP_PORT 80
	#define RELAY_BUFFERING_TIME_MS 3000
	#define RELAY_HTTP_REQUEST_TIMEOUT_MS 2000

	#define NODE_BUFFER_SIZE 3000
	#define NODE_BUFFER_LENGTH 3000

#endif
