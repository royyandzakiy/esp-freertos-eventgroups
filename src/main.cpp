#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

// Event group handle (global)
EventGroupHandle_t xEventGroup;

// Bit definitions (up to 24 bits)
#define BIT_SENSOR_READY    (1 << 0)  // Bit 0
#define BIT_GPS_READY       (1 << 1)  // Bit 1  
#define BIT_BUTTON_PRESS    (1 << 2)  // Bit 2
#define BIT_NETWORK_READY   (1 << 3)  // Bit 3

// Simulated sensor task
void sensorTask(void *pvParameters) {
  while(1) {
    vTaskDelay(pdMS_TO_TICKS(2000)); // Simulate work
    Serial.println("Sensor: Reading complete");
    xEventGroupSetBits(xEventGroup, BIT_SENSOR_READY);
  }
}

// Simulated GPS task  
void gpsTask(void *pvParameters) {
  while(1) {
    vTaskDelay(pdMS_TO_TICKS(3000)); // Simulate work
    Serial.println("GPS: Fix acquired");
    xEventGroupSetBits(xEventGroup, BIT_GPS_READY);
  }
}

// Main task that waits for BOTH sensor AND GPS
void dataLoggerTask(void *pvParameters) {
  while(1) {
    Serial.println("Logger: Waiting for sensor AND GPS...");
    
    // Wait for BOTH bits, clear them on exit, wait for ALL
    EventBits_t bits = xEventGroupWaitBits(
      xEventGroup,                    // Event group handle
      BIT_SENSOR_READY | BIT_GPS_READY, // Wait for these bits
      pdTRUE,                         // Clear bits on exit
      pdTRUE,                         // Wait for ALL bits (AND)
      portMAX_DELAY                   // Wait forever
    );
    
    if((bits & (BIT_SENSOR_READY | BIT_GPS_READY)) == (BIT_SENSOR_READY | BIT_GPS_READY)) {
      Serial.println("Logger: BOTH ready! Processing data...");
      // Do work with both sensor and GPS data
      vTaskDelay(pdMS_TO_TICKS(500)); // Simulate processing
    }
  }
}

// Simulated button handler
void buttonTask(void *pvParameters) {
  while(1) {
    vTaskDelay(pdMS_TO_TICKS(random(1000, 5000))); // Random delay
    Serial.println("Button: Press detected");
    xEventGroupSetBits(xEventGroup, BIT_BUTTON_PRESS);
  }
}

// Simulated network handler
void networkTask(void *pvParameters) {
  while(1) {
    vTaskDelay(pdMS_TO_TICKS(random(2000, 6000))); // Random delay  
    Serial.println("Network: Connection established");
    xEventGroupSetBits(xEventGroup, BIT_NETWORK_READY);
  }
}

// UI task that waits for EITHER button OR network
void uiTask(void *pvParameters) {
  while(1) {
    Serial.println("UI: Waiting for button OR network...");
    
    // Wait for ANY of the bits, clear on exit, wait for ANY
    EventBits_t bits = xEventGroupWaitBits(
      xEventGroup,                      // Event group handle
      BIT_BUTTON_PRESS | BIT_NETWORK_READY, // Wait for these bits
      pdTRUE,                           // Clear bits on exit  
      pdFALSE,                          // Wait for ANY bit (OR)
      portMAX_DELAY                     // Wait forever
    );
    
    if(bits & BIT_BUTTON_PRESS) {
      Serial.println("UI: Handling button press!");
    }
    
    if(bits & BIT_NETWORK_READY) {
      Serial.println("UI: Handling network event!");
    }
    
    vTaskDelay(pdMS_TO_TICKS(200)); // Small delay after handling
  }
}

void setup() {
  Serial.begin(115200);
  vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for serial
  Serial.println("\n\nFreeRTOS Event Groups Demo");

  // Create the event group
  xEventGroup = xEventGroupCreate();
  
  if(xEventGroup == NULL) {
    Serial.println("Failed to create event group!");
    return;
  }

  // Create AND scenario tasks
  xTaskCreatePinnedToCore(dataLoggerTask, "DataLogger", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(sensorTask, "Sensor", 2048, NULL, 1, NULL, 1);  
  xTaskCreatePinnedToCore(gpsTask, "GPS", 2048, NULL, 1, NULL, 1);

  // Create OR scenario tasks  
  xTaskCreatePinnedToCore(uiTask, "UI", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(buttonTask, "Button", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(networkTask, "Network", 2048, NULL, 1, NULL, 1);

  Serial.println("All tasks created - system running!");
}

void loop() {
  // Empty - everything runs in tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
}