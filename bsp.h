#ifndef BSP_H
#define BSP_H
// #define USE_EXTERNAL_XTAL
#define USE_LORAWAN
#define LORA_DIO1 3
#define LORA_BUSY 4
#define LORA_NRST 5
#define LORA_MISO 6
#define LORA_MOSI 7
#define LORA_NSS 8
#define LORA_SCK 10
#define REGION (LoRaWANBand_t) AS923_1
#define SUB_BAND 0

#define USE_AT_COMMAND_SERVICE
#define USE_NIMBLE
#define USE_PTC
#define USE_MAX31865
#define pMISO 19
#define pMOSI 18
#define pSCK 20
#define pCS 2
#define CUSTOM_PINS
#define pVext 9
#define pADC_BAT 2
#define USE_BATTERY
#define USE_BAT_ADC
#define BAT_ADC_PIN 2
#define BAT_MAX_VOLTAGE 4.2
#define BAT_MIN_VOLTAGE 3.0
#define BAT_VOLTAGE_MULTIPLIER 1.08 // if the value is 1.0, the battery voltage will be multiplied by 1.0, otherwise, the battery voltage will be multiplied by the value
#endif