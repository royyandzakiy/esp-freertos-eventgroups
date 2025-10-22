# FreeRTOS - Event Groups (ESP + Arduino)

This project illustrates how **FreeRTOS event groups** can be used to coordinate multiple concurrent tasks in a real-time system. It is implemented for the **ESP32** using the **Arduino framework** on **PlatformIO**.

## Overview

This program demonstrates **task synchronization** and **event signaling** using FreeRTOS **Event Groups**—a mechanism that allows multiple tasks to communicate using bit flags.

An event group is essentially a set of bits shared across tasks.

* Each bit can represent the state of a subsystem (e.g., a sensor, GPS, button press, or network connection).
* Tasks can **set** bits when events occur or **wait** for one or multiple bits before continuing execution.

## Getting Started

1. Open the project in PlatformIO (VSCode).
2. Connect your ESP board.
3. Build and upload the firmware.
4. Open the serial monitor at **115200 baud** to observe the log output.

## Concept: Event Groups

Event groups provide a lightweight method for **inter-task signaling**:

* `xEventGroupSetBits()` sets one or more bits to signal an event.
* `xEventGroupWaitBits()` lets tasks block until specific bits are set.

Two modes of waiting are used:

* **AND logic (`pdTRUE`)** – waits until *all* specified bits are set.
* **OR logic (`pdFALSE`)** – continues when *any* specified bit is set.

Bits can also be cleared automatically after being consumed, allowing repeating cycles.

## Code Structure and Flow

### Event Bits Definition

```cpp
#define BIT_SENSOR_READY    (1 << 0)
#define BIT_GPS_READY       (1 << 1)
#define BIT_BUTTON_PRESS    (1 << 2)
#define BIT_NETWORK_READY   (1 << 3)
```

Each bit corresponds to a subsystem event. These are combined and managed within a single `EventGroupHandle_t`.

### Tasks Overview

| Task             | Description                                                              | Wait Type      |
| ---------------- | ------------------------------------------------------------------------ | -------------- |
| `sensorTask`     | Periodically simulates a sensor reading, sets `BIT_SENSOR_READY`.        | Producer       |
| `gpsTask`        | Simulates GPS fix acquisition, sets `BIT_GPS_READY`.                     | Producer       |
| `dataLoggerTask` | Waits until both sensor and GPS are ready, then processes data.          | AND (all bits) |
| `buttonTask`     | Randomly triggers button press events.                                   | Producer       |
| `networkTask`    | Randomly triggers network readiness events.                              | Producer       |
| `uiTask`         | Waits for *either* a button press or network readiness to update the UI. | OR (any bit)   |

### Execution Flow

1. Each producer task sets its corresponding event bit when work completes.
2. Consumer tasks (`dataLoggerTask`, `uiTask`) wait for these bits:

   * `dataLoggerTask` waits for **both** sensor and GPS readiness before processing.
   * `uiTask` waits for **either** a button press or a network event to update the UI.
3. Bits are cleared automatically after being processed to allow continuous operation.

## Key Takeaways

* Event groups provide **efficient synchronization** between concurrent tasks without polling.
* The **AND/OR wait logic** offers flexibility for multi-condition task coordination.
* The structure shown here scales easily for complex systems such as sensor fusion, communication managers, or user interfaces in embedded applications.