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

	#ifdef DEBUG
		#define SERIALPRINTSHOT
	#endif

	#define MAC_SIZE 13
	#define NONE_STR "none"

	// Training configuration
	#define MAX_EPOCHS 100
	#define REQUIRED_IMPROVEMENT 0.01
	#define ALLOWED_ITERATIONS_WHERE_TRAINING_DOES_NOT_IMPROVE 5 
	#define TRAIN_DATA_BUFFER_SIZE 20
	#define VALIDATION_DATA_BUFFER_SIZE 10
	#define MAX_DATA_BUFFER_SIZE (TRAIN_DATA_BUFFER_SIZE + VALIDATION_DATA_BUFFER_SIZE)
  
	// Encog parser configuration
	#define ENCOG_LABEL_MAX_LENGTH 16 				// Size of buffer wherein ENCOG labels are read into
	#define ENCOG_FORCE_OVERWRITE_MODEL true		// ReWrite the default model on boot
	#define ENCOG_MODEL_FILE_PATH "/model"			// File path of encog model

	#define MAX_NUMBER_OF_NODES 4
	#define NUMBER_OF_LABELS 2
  #define HIDDEN_NEURONS 100
  #define INPUT_NEURONS 40

	// WiFi configuration
	#define WIFI_AP_SSID "FootballRelay"
	#define WIFI_SSID "AAU-1-DAY"
	#define WIFI_PASS "sugar94noise"
//	#define WIFI_SSID "Tardis"
//	#define WIFI_PASS "geronimo"
	#define UDP_PORT 8085
	#define HTTP_HOST "p5datahub.azurewebsites.net"
	#define HTTP_PORT 80
	#define RELAY_BUFFERING_TIME_MS 3000
	#define RELAY_HTTP_REQUEST_TIMEOUT_MS 2000

	#define NODE_BUFFER_SIZE 3000
	#define NODE_BUFFER_LENGTH 3000

	#define LEARNING_RATE 0.01

#endif
