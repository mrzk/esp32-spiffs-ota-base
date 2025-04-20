# ESP32 SPIFFS OTA Base

A comprehensive ESP32 Arduino sketch that provides web-based Over-The-Air (OTA) updates for SPIFFS (SPI Flash File System) with additional file management capabilities.

## Features

- **Web-based SPIFFS Upload**: Upload SPIFFS binary files through a web interface
- **File Management**: List all files stored in SPIFFS with their sizes
- **SPIFFS Formatting**: Format the SPIFFS partition through the web interface
- **OTA Support**: Built-in Arduino OTA functionality for firmware updates
- **Partition Size Detection**: Automatically detects SPIFFS partition size for safe uploads

## Setup

1. **Configure WiFi Credentials**:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```

2. **Upload the sketch** to your ESP32

3. **Monitor Serial Output** to see the assigned IP address

## Web Interface

Once the ESP32 is connected to WiFi, access the web interface using the IP address shown in the serial monitor.

### Available Routes

| Route | Description |
|-------|-------------|
| `/` | Home page with navigation links |
| `/spiffs/flash` | Upload form for SPIFFS binary files and handle uploads |
| `/spiffs/list` | Display all files in SPIFFS |
| `/spiffs/format` | Format SPIFFS partition with confirmation |

### Usage Examples

1. **Access the home page**: `http://[ESP32_IP]/`
2. **Upload SPIFFS binary**: `http://[ESP32_IP]/spiffs/flash`
3. **List files**: `http://[ESP32_IP]/spiffs/list`
4. **Format SPIFFS**: `http://[ESP32_IP]/spiffs/format`

## SPIFFS Binary Creation
Refer to https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/spiffs.html

## Troubleshooting

### Common Issues

1. **SPIFFS initialization failed**:
   - Check if SPIFFS partition is defined in partition table
   - Verify sufficient flash memory allocation

2. **Upload too large for SPIFFS partition**:
   - Check the SPIFFS partition size in your board's partition table
   - Reduce the size of your SPIFFS binary

3. **WiFi connection issues**:
   - Verify SSID and password are correct
   - Check WiFi signal strength
   - Ensure ESP32 is within range of the access point

## License

This project is open source. Please check the LICENSE file for details.

## Contributing

Feel free to submit issues, feature requests, or pull requests to improve this project.
