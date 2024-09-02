
#include "pn532.h"


static const char *TAG = "PN532_LIB";

uint8_t pn532ack[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00}; ///< ACK message from PN532
uint8_t pn532response_firmwarevers[] = {0x00, 0x00, 0xFF, 0x06, 0xFA, 0xD5}; ///< Expected firmware version message from PN532

// #define PN532DEBUG
// #define MIFAREDEBUG

// #define PN532DEBUGPRINT Serial ///< Fixed name for debug Serial instance
//#define PN532DEBUGPRINT SerialUSB ///< Fixed name for debug Serial instance

#define PN532_PACKBUFFSIZ 64                ///< Packet buffer size in uint8_ts
uint8_t pn532_packetbuffer[PN532_PACKBUFFSIZ]; ///< Packet buffer used in various
                                            ///< transactions


void delay(int x){
  // x=x*1000;
  vTaskDelay(x / portTICK_PERIOD_MS);
}


#define UART_NUM_P2         UART_NUM_2    // Sử dụng UART2
#define TX_GPIO_NUM     17    // Chân TX (thay đổi nếu cần)
#define RX_GPIO_NUM      16    // Chân RX (thay đổi nếu cần)
#define BAUD_RATE        115200         // Tốc độ baud
#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
static QueueHandle_t uart0_queue;

void uart_config(void){    
        const uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    // Cấu hình UART với các tham số
    uart_param_config(UART_NUM_P2, &uart_config);
    // Thiết lập các chân RX_GPIO_NUM
    uart_set_pin(UART_NUM_P2, TX_GPIO_NUM, RX_GPIO_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // Cài đặt driver UART
    uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0);
}


int pn532_write(const void* data, size_t size) {
    return uart_write_bytes(UART_NUM_P2, (const char*)data, size);
}

int pn532_read(const uint8_t *buffer, size_t len_buffer){
    // ESP_LOGE("uart_read_bytes","uart_read_bytes");
    //  uart_read_bytes(UART_NUM, &buffer, len_buffer, portMAX_DELAY);
    // return uart_read_bytes(UART_NUM_P2,&buffer, len_buffer,portMAX_DELAY);
    // return 1;
    return uart_read_bytes(UART_NUM_P2,buffer, len_buffer,portMAX_DELAY);
}



void test_uart(){
        const char *message = "Hello, UART2!\n";
    // Gửi dữ liệu qua UART2
    // int len = uart_write_bytes(UART_NUM_P2, message, strlen(message));
    // int len=uart_write_bytes(UART_NUM_P2,(unsigned char*)message, strlen(message));
    uint8_t data = 0xFF;
    // int len = pn532_write((unsigned char*)message,strlen(message));
    int len = uart_write_bytes(UART_NUM_P2, (const char*)message, strlen(message));
    // int len=write(&data, sizeof(data));
    // int len = pn532_write(&data, sizeof(data));

    // int len=uart_write_bytes(UART_NUM_P2, (const char*)data, sizeof(data));
    // int len = uart_write_bytes(UART_NUM_P2, (const char*)&data, sizeof(data));
    if (len == strlen(message)) {
        ESP_LOGI("UART", "Message sent successfully via UART2");
    } else {
        ESP_LOGE("UART", "Failed to send message via UART2");
    }
}
void get_buffer(size_t *length){
    uart_get_buffered_data_len(UART_NUM_P2, &length);
    
}

void pn532_begin(void) {
  uart_config();
        ESP_LOGE(TAG, "uart_config");
  // reset(); // HW reset - put in known state
size_t *length;
 uint8_t *buffer;
get_buffer(&length);
printf("%zu \n",*length);
// pn532_read(&buffer, 1);
// uart_read_bytes(UART_NUM_P2,&buffer, length,portMAX_DELAY);
// if (uart_read_bytes(UART_NUM_P2,&buffer, 1, 100 / portTICK_PERIOD_MS)){
//   printf("readd \n");
// }
// else{
//   printf("read fail \n");
// }

  delay(10);
  // vTaskDelay(10 / portTICK_PERIOD_MS);
        ESP_LOGE(TAG, "delay");
  wakeup(); // hey! wakeup!
        ESP_LOGE(TAG, "wakeup");

  // return true;
}

// int pn532_read(const uint8_t *buffer, size_t len_buffer){
//     // ESP_LOGE("uart_read_bytes","uart_read_bytes");
//     //  uart_read_bytes(UART_NUM, &buffer, len_buffer, portMAX_DELAY);
//     return uart_read_bytes(UART_NUM,&buffer, len_buffer,portMAX_DELAY);
//     // return 1;
// }

// void pn532_write( uint8_t *cmd , uint8_t cmdlen){
//     uart_write_bytes(UART_NUM,(unsigned char*)cmd, cmdlen);
// }
/**************************************************************************/
/*!
    @brief  Perform a hardware reset. Requires reset pin to have been provided.
*/
/**************************************************************************/
// void reset(void) {
//   // see Datasheet p.209, Fig.48 for timings
//   if (_reset != -1) {
//     digitalpn532_write(_reset, LOW);
//     delay(1); // min 20ns
//     digitalpn532_write(_reset, HIGH);
//     delay(2); // max 2ms
//   }
// }

/**************************************************************************/
/*!
    @brief  Wakeup from LowVbat mode into Normal Mode.
*/
/**************************************************************************/
void wakeup(void) {

    uint8_t wake[3] = {0x55, 0x00, 0x00};
    // pn532_write(wake, 3);
    int len = uart_write_bytes(UART_NUM_P2, (const char*)wake, 3);
// pn532_write(wake, 3)
        if ( len == 3) {
        ESP_LOGI("UART", "Wake Up");
    } else {
        ESP_LOGE("UART", "Failed Wake Up");
    }
    delay(2);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
  // need to config SAM to stay in Normal Mode
  pn532_SAMConfig();
}

/**************************************************************************/
/*!
    @brief  Prints a hexadecimal value in plain characters

    @param  data      Pointer to the uint8_t data
    @param  numuint8_ts  Data length in uint8_ts
*/
/**************************************************************************/
void PrintHex(const uint8_t *data, const uint32_t numBytes) {
  uint32_t szPos;
  for (szPos = 0; szPos < numBytes; szPos++) {
    printf("0x");
    // Append leading 0 for small values
    if (data[szPos] <= 0xF)
      printf("0");
    // print(data[szPos] & 0xff, HEX);
    printf("%u",data[szPos]);
    if ((numBytes > 1) && (szPos != numBytes - 1)) {
      printf(" ");
    }
  }
  printf("\n");
}

/**************************************************************************/
/*!
    @brief  Prints a hexadecimal value in plain characters, along with
            the char equivalents in the following format

            00 00 00 00 00 00  ......

    @param  data      Pointer to the byte data
    @param  numBytes  Data length in bytes
*/
/**************************************************************************/
void PrintHexChar(const uint8_t *data, const uint32_t numBytes) {
  uint32_t szPos;
  for (szPos = 0; szPos < numBytes; szPos++) {
    // Append leading 0 for small values
    if (data[szPos] <= 0xF)
      printf("0");
    // print(data[szPos], HEX);
    printf("%u",data[szPos]);
    if ((numBytes > 1) && (szPos != numBytes - 1)) {
      printf(" ");
    }
  }
  printf("  ");
  for (szPos = 0; szPos < numBytes; szPos++) {
    if (data[szPos] <= 0x1F)
      printf(".");
    else
      // printf((char*)data[szPos]);
      printf("%d\n", data[szPos]);
  }
  printf("\n");
}

/**************************************************************************/
/*!
    @brief  Checks the firmware version of the PN5xx chip

    @returns  The chip's firmware version and ID
*/
/**************************************************************************/
uint32_t pn532_getFirmwareVersion(void) {
  uint32_t response;
    ESP_LOGI("PN532","pn532_getFirmwareVersion");

  pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;

  if (!sendCommandCheckAck(pn532_packetbuffer, 1)) {
    return 0;
  }
    ESP_LOGI("PN532","sendCommandCheckAck");

  // read data packet
  readdata(pn532_packetbuffer, 13);
    ESP_LOGI("PN532","readdata");

  // check some basic stuff
  if (0 != memcmp((char *)pn532_packetbuffer,
                  (char *)pn532response_firmwarevers, 6)) {
// #ifdef PN532DEBUG
    ESP_LOGE("ERROR","Firmware doesn't match!");
// #endif
    return 0;
  }

  int offset = 7;
  response = pn532_packetbuffer[offset++];
  response <<= 8;
  response |= pn532_packetbuffer[offset++];
  response <<= 8;
  response |= pn532_packetbuffer[offset++];
  response <<= 8;
  response |= pn532_packetbuffer[offset++];

  return response;
}

/**************************************************************************/
/*!
    @brief  Sends a command and waits a specified period for the ACK

    @param  cmd       Pointer to the command buffer
    @param  cmdlen    The size of the command in bytes
    @param  timeout   timeout before giving up

    @returns  1 if everything is OK, 0 if timeout occured before an
              ACK was recieved
*/
/**************************************************************************/
// default timeout of one second

bool CommandCheckAck(uint8_t *cmd, uint8_t cmdlen,
                                         uint16_t timeout){
{
// uint16_t timeout=100;

  uint8_t SLOWDOWN = 0;


  // write the command
  writecommand(cmd, cmdlen);

  // I2C TUNING
  delay(SLOWDOWN);

  // Wait for chip to say its ready!
  if (!waitready(timeout)) {
    return false;
  }

#ifdef PN532DEBUG
  if (spi_dev == NULL) {
    ESP_LOGE("ERROR","IRQ received");
  }
#endif

  // read acknowledgement
  if (!readack()) {
#ifdef PN532DEBUG
    ESP_LOGE("ERROR","No ACK frame received!");
#endif
    return false;
  }

  // I2C TUNING
  delay(SLOWDOWN);

  // Wait for chip to say its ready!
  if (!waitready(timeout)) {
    return false;
  }

  return true; // ack'd command
}
                                         }
bool sendCommandCheckAck(uint8_t *cmd, uint8_t cmdlen) {
uint16_t timeout=200;

  uint8_t SLOWDOWN = 0;


  // write the command
  writecommand(cmd, cmdlen);
ESP_LOGW("writecommand","writecommand");
  // I2C TUNING
  // delay(SLOWDOWN);

  // Wait for chip to say its ready!
  if (!waitready(timeout)) {
    return false;
  }
ESP_LOGW("waitready","waitready");



  // read acknowledgement
if (!readack()) {
#ifdef PN532DEBUG
    ESP_LOGE("ERROR","No ACK frame received!");
#endif
    return false;
  }
ESP_LOGW("readack","readack");


  // Wait for chip to say its ready!
  if (!waitready(timeout)) {
    return false;
  }
ESP_LOGW("waitready","waitready");

  return true; // ack'd command
}

/**************************************************************************/
/*!
    @brief   Writes an 8-bit value that sets the state of the PN532's GPIO
             pins.
    @param   pinstate  P3 pins state.

    @warning This function is provided exclusively for board testing and
             is dangerous since it will throw an error if any pin other
             than the ones marked "Can be used as GPIO" are modified!  All
             pins that can not be used as GPIO should ALWAYS be left high
             (value = 1) or the system will become unstable and a HW reset
             will be required to recover the PN532.

             pinState[0]  = P30     Can be used as GPIO
             pinState[1]  = P31     Can be used as GPIO
             pinState[2]  = P32     *** RESERVED (Must be 1!) ***
             pinState[3]  = P33     Can be used as GPIO
             pinState[4]  = P34     *** RESERVED (Must be 1!) ***
             pinState[5]  = P35     Can be used as GPIO

    @return  1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool writeGPIO(uint8_t pinstate) {
  // uint8_t errorbit;

  // Make sure pinstate does not try to toggle P32 or P34
  pinstate |= (1 << PN532_GPIO_P32) | (1 << PN532_GPIO_P34);

  // Fill command buffer
  pn532_packetbuffer[0] = PN532_COMMAND_WRITEGPIO;
  pn532_packetbuffer[1] = PN532_GPIO_VALIDATIONBIT | pinstate; // P3 Pins
  pn532_packetbuffer[2] = 0x00; // P7 GPIO Pins (not used ... taken by SPI)

#ifdef PN532DEBUG
  print(F("Writing P3 GPIO: "));
  println(pn532_packetbuffer[1], HEX);
#endif

  // Send the WRITEGPIO command (0x0E)
  if (!sendCommandCheckAck(pn532_packetbuffer, 3))
    return 0x0;

  // Read response packet (00 FF PLEN PLENCHECKSUM D5 CMD+1(0x0F) DATACHECKSUM
  // 00)
  readdata(pn532_packetbuffer, 8);

#ifdef PN532DEBUG
  print(F("Received: "));
  PrintHex(pn532_packetbuffer, 8);
  println();
#endif

  int offset = 6;
  return (pn532_packetbuffer[offset] == 0x0F);
}

/**************************************************************************/
/*!
    Reads the state of the PN532's GPIO pins

    @returns An 8-bit value containing the pin state where:

             pinState[0]  = P30
             pinState[1]  = P31
             pinState[2]  = P32
             pinState[3]  = P33
             pinState[4]  = P34
             pinState[5]  = P35
*/
/**************************************************************************/
uint8_t readGPIO(void) {
  pn532_packetbuffer[0] = PN532_COMMAND_READGPIO;

  // Send the READGPIO command (0x0C)
  if (!sendCommandCheckAck(pn532_packetbuffer, 1))
    return 0x0;

  // Read response packet (00 FF PLEN PLENCHECKSUM D5 CMD+1(0x0D) P3 P7 IO1
  // DATACHECKSUM 00)
  readdata(pn532_packetbuffer, 11);

  /* READGPIO response should be in the following format:

    byte            Description
    -------------   ------------------------------------------
    b0..5           Frame header and preamble (with I2C there is an extra 0x00)
    b6              P3 GPIO Pins
    b7              P7 GPIO Pins (not used ... taken by SPI)
    b8              Interface Mode Pins (not used ... bus select pins)
    b9..10          checksum */

  int p3offset = 7;

#ifdef PN532DEBUG
  print(F("Received: "));
  PrintHex(pn532_packetbuffer, 11);
  println();
  print(F("P3 GPIO: 0x"));
  println(pn532_packetbuffer[p3offset], HEX);
  print(F("P7 GPIO: 0x"));
  println(pn532_packetbuffer[p3offset + 1], HEX);
  print(F("IO GPIO: 0x"));
  println(pn532_packetbuffer[p3offset + 2], HEX);
  // Note: You can use the IO GPIO value to detect the serial bus being used
  switch (pn532_packetbuffer[p3offset + 2]) {
  case 0x00: // Using UART
    ESP_LOGE("ERROR","Using UART (IO = 0x00)");
    break;
  case 0x01: // Using I2C
    ESP_LOGE("ERROR","Using I2C (IO = 0x01)");
    break;
  case 0x02: // Using SPI
    ESP_LOGE("ERROR","Using SPI (IO = 0x02)");
    break;
  }
#endif

  return pn532_packetbuffer[p3offset];
}

/**************************************************************************/
/*!
    @brief   Configures the SAM (Secure Access Module)
    @return  true on success, false otherwise.
*/
/**************************************************************************/
bool pn532_SAMConfig(void) {
  pn532_packetbuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
  pn532_packetbuffer[1] = 0x01; // normal mode;
  pn532_packetbuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
  pn532_packetbuffer[3] = 0x01; // use IRQ pin!

  if (!sendCommandCheckAck(pn532_packetbuffer, 4))
    return false;

  // read data packet
  readdata(pn532_packetbuffer, 9);

  int offset = 6;
  return (pn532_packetbuffer[offset] == 0x15);
}

/**************************************************************************/
/*!
    Sets the MxRtyPassiveActivation byte of the RFConfiguration register

    @param  maxRetries    0xFF to wait forever, 0x00..0xFE to timeout
                          after mxRetries

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool setPassiveActivationRetries(uint8_t maxRetries) {
  pn532_packetbuffer[0] = PN532_COMMAND_RFCONFIGURATION;
  pn532_packetbuffer[1] = 5;    // Config item 5 (MaxRetries)
  pn532_packetbuffer[2] = 0xFF; // MxRtyATR (default = 0xFF)
  pn532_packetbuffer[3] = 0x01; // MxRtyPSL (default = 0x01)
  pn532_packetbuffer[4] = maxRetries;

#ifdef MIFAREDEBUG
  print(F("Setting MxRtyPassiveActivation to "));
  print(maxRetries, DEC);
  ESP_LOGE("ERROR"," ");
#endif

  if (!sendCommandCheckAck(pn532_packetbuffer, 5))
    return 0x0; // no ACK

  return 1;
}

/***** ISO14443A Commands ******/

/**************************************************************************/
/*!
    @brief   Waits for an ISO14443A target to enter the field and reads
             its ID.

    @param   cardbaudrate  Baud rate of the card
    @param   uid           Pointer to the array that will be populated
                           with the card's UID (up to 7 bytes)
    @param   uidLength     Pointer to the variable that will hold the
                           length of the card's UID.
    @param   timeout       Timeout in milliseconds.

    @return  1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool readPassiveTargetID(uint8_t cardbaudrate, uint8_t *uid,
                                         uint8_t *uidLength, uint16_t timeout) {
  timeout=0;
  pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
  pn532_packetbuffer[1] = 1; // max 1 cards at once (we can set this to 2 later)
  pn532_packetbuffer[2] = cardbaudrate;

  if (!CommandCheckAck(pn532_packetbuffer, 3, timeout)) {
#ifdef PN532DEBUG
    ESP_LOGE("ERROR","No card(s) read");
#endif
    return 0x0; // no cards read
  }

  return readDetectedPassiveTargetID(uid, uidLength);
}

/**************************************************************************/
/*!
    @brief   Put the reader in detection mode, non blocking so interrupts
             must be enabled.
    @param   cardbaudrate  Baud rate of the card
    @return  1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool startPassiveTargetIDDetection(uint8_t cardbaudrate) {
  pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
  pn532_packetbuffer[1] = 1; // max 1 cards at once (we can set this to 2 later)
  pn532_packetbuffer[2] = cardbaudrate;

  return sendCommandCheckAck(pn532_packetbuffer, 3);
}

/**************************************************************************/
/*!
    Reads the ID of the passive target the reader has deteceted.

    @param  uid           Pointer to the array that will be populated
                          with the card's UID (up to 7 bytes)
    @param  uidLength     Pointer to the variable that will hold the
                          length of the card's UID.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool readDetectedPassiveTargetID(uint8_t *uid,
                                                 uint8_t *uidLength) {
  // read data packet
  readdata(pn532_packetbuffer, 20);
  // check some basic stuff

  /* ISO14443A card response should be in the following format:

    byte            Description
    -------------   ------------------------------------------
    b0..6           Frame header and preamble
    b7              Tags Found
    b8              Tag Number (only one used in this example)
    b9..10          SENS_RES
    b11             SEL_RES
    b12             NFCID Length
    b13..NFCIDLen   NFCID                                      */

#ifdef MIFAREDEBUG
  print(F("Found "));
  print(pn532_packetbuffer[7], DEC);
  ESP_LOGE("ERROR"," tags");
#endif
  if (pn532_packetbuffer[7] != 1)
    return 0;

  uint16_t sens_res = pn532_packetbuffer[9];
  sens_res <<= 8;
  sens_res |= pn532_packetbuffer[10];
#ifdef MIFAREDEBUG
  print(F("ATQA: 0x"));
  println(sens_res, HEX);
  print(F("SAK: 0x"));
  println(pn532_packetbuffer[11], HEX);
#endif

  /* Card appears to be Mifare Classic */
  *uidLength = pn532_packetbuffer[12];
#ifdef MIFAREDEBUG
  print(F("UID:"));
#endif
  for (uint8_t i = 0; i < pn532_packetbuffer[12]; i++) {
    uid[i] = pn532_packetbuffer[13 + i];
#ifdef MIFAREDEBUG
    print(F(" 0x"));
    print(uid[i], HEX);
#endif
  }
#ifdef MIFAREDEBUG
  println();
#endif

  return 1;
}

/**************************************************************************/
/*!
    @brief   Exchanges an APDU with the currently inlisted peer

    @param   send            Pointer to data to send
    @param   sendLength      Length of the data to send
    @param   response        Pointer to response data
    @param   responseLength  Pointer to the response data length
    @return  true on success, false otherwise.
*/
/**************************************************************************/
// bool inDataExchange(uint8_t *send, uint8_t sendLength,
//                                     uint8_t *response,
//                                     uint8_t *responseLength) {
//   if (sendLength > PN532_PACKBUFFSIZ - 2) {
// #ifdef PN532DEBUG
//     ESP_LOGE("ERROR","APDU length too long for packet buffer");
// #endif
//     return false;
//   }
//   uint8_t i;

//   pn532_packetbuffer[0] = 0x40; // PN532_COMMAND_INDATAEXCHANGE;
//   pn532_packetbuffer[1] = _inListedTag;
//   for (i = 0; i < sendLength; ++i) {
//     pn532_packetbuffer[i + 2] = send[i];
//   }

//   if (!CommandCheckAck(pn532_packetbuffer, sendLength + 2, 1000)) {
// #ifdef PN532DEBUG
//     ESP_LOGE("ERROR",("Could not send APDU");
// #endif
//     return false;
//   }

//   if (!waitready(1000)) {
// #ifdef PN532DEBUG
//     ESP_LOGE("ERROR","Response never received for APDU...");
// #endif
//     return false;
//   }

//   readdata(pn532_packetbuffer, sizeof(pn532_packetbuffer));

//   if (pn532_packetbuffer[0] == 0 && pn532_packetbuffer[1] == 0 &&
//       pn532_packetbuffer[2] == 0xff) {
//     uint8_t length = pn532_packetbuffer[3];
//     if (pn532_packetbuffer[4] != (uint8_t)(~length + 1)) {
// #ifdef PN532DEBUG
//       ESP_LOGE("ERROR","Length check invalid");
//       println(length, HEX);
//       println((~length) + 1, HEX);
// #endif
//       return false;
//     }
//     if (pn532_packetbuffer[5] == PN532_PN532TOHOST &&
//         pn532_packetbuffer[6] == PN532_RESPONSE_INDATAEXCHANGE) {
//       if ((pn532_packetbuffer[7] & 0x3f) != 0) {
// #ifdef PN532DEBUG
//         ESP_LOGE("ERROR","Status code indicates an error");
// #endif
//         return false;
//       }

//       length -= 3;

//       if (length > *responseLength) {
//         length = *responseLength; // silent truncation...
//       }

//       for (i = 0; i < length; ++i) {
//         response[i] = pn532_packetbuffer[8 + i];
//       }
//       *responseLength = length;

//       return true;
//     } else {
//       printf("Don't know how to handle this command: ");
//       // println(pn532_packetbuffer[6], HEX);
//       printf("%u",pn532_packetbuffer[6]);
      
//       return false;
//     }
//   } else {
//     ESP_LOGE("ERROR","Preamble missing");
//     return false;
//   }
// }

/**************************************************************************/
/*!
    @brief   'InLists' a passive target. PN532 acting as reader/initiator,
             peer acting as card/responder.
    @return  true on success, false otherwise.
*/
/**************************************************************************/
bool inListPassiveTarget() {
  pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
  pn532_packetbuffer[1] = 1;
  pn532_packetbuffer[2] = 0;

#ifdef PN532DEBUG
  print(F("About to inList passive target"));
#endif

  if (!CommandCheckAck(pn532_packetbuffer, 3, 1000)) {
#ifdef PN532DEBUG
    ESP_LOGE("ERROR","Could not send inlist message");
#endif
    return false;
  }

  if (!waitready(30000)) {
    return false;
  }

  readdata(pn532_packetbuffer, sizeof(pn532_packetbuffer));

  if (pn532_packetbuffer[0] == 0 && pn532_packetbuffer[1] == 0 &&
      pn532_packetbuffer[2] == 0xff) {
    uint8_t length = pn532_packetbuffer[3];
    if (pn532_packetbuffer[4] != (uint8_t)(~length + 1)) {
#ifdef PN532DEBUG
      
      ESP_LOGE("ERROR","Length check invalid");
      println(length, HEX);
      println((~length) + 1, HEX);
#endif
      return false;
    }
    if (pn532_packetbuffer[5] == PN532_PN532TOHOST &&
        pn532_packetbuffer[6] == PN532_RESPONSE_INLISTPASSIVETARGET) {
      if (pn532_packetbuffer[7] != 1) {
#ifdef PN532DEBUG
        ESP_LOGE("ERROR","Unhandled number of targets inlisted");
#endif
        ESP_LOGE("ERROR","Number of tags inlisted:");
        printf("%u \n",pn532_packetbuffer[7]);
        return false;
      }

      int8_t _inListedTag = pn532_packetbuffer[8];
      printf("Tag number: ");
      printf("%d \n",_inListedTag);

      return true;
    } else {
#ifdef PN532DEBUG
      print(F("Unexpected response to inlist passive host"));
#endif
      return false;
    }
  } else {
#ifdef PN532DEBUG
    ESP_LOGE("ERROR","Preamble missing");
#endif
    return false;
  }

  return true;
}

/***** Mifare Classic Functions ******/

/**************************************************************************/
/*!
    @brief   Indicates whether the specified block number is the first block
             in the sector (block 0 relative to the current sector)
    @param   uiBlock  Block number to test.
    @return  true if first block, false otherwise.
*/
/**************************************************************************/
bool mifareclassic_IsFirstBlock(uint32_t uiBlock) {
  // Test if we are in the small or big sectors
  if (uiBlock < 128)
    return ((uiBlock) % 4 == 0);
  else
    return ((uiBlock) % 16 == 0);
}

/**************************************************************************/
/*!
    @brief   Indicates whether the specified block number is the sector
             trailer.
    @param   uiBlock  Block number to test.
    @return  true if sector trailer, false otherwise.
*/
/**************************************************************************/
bool mifareclassic_IsTrailerBlock(uint32_t uiBlock) {
  // Test if we are in the small or big sectors
  if (uiBlock < 128)
    return ((uiBlock + 1) % 4 == 0);
  else
    return ((uiBlock + 1) % 16 == 0);
}

/**************************************************************************/
/*!
    Tries to authenticate a block of memory on a MIFARE card using the
    INDATAEXCHANGE command.  See section 7.3.8 of the PN532 User Manual
    for more information on sending MIFARE and other commands.

    @param  uid           Pointer to a byte array containing the card UID
    @param  uidLen        The length (in bytes) of the card's UID (Should
                          be 4 for MIFARE Classic)
    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  keyNumber     Which key type to use during authentication
                          (0 = MIFARE_CMD_AUTH_A, 1 = MIFARE_CMD_AUTH_B)
    @param  keyData       Pointer to a byte array containing the 6 byte
                          key value

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/

  // int8_t _uid[7];      // ISO14443A uid
  // int8_t _uidLen;      // uid len
  // int8_t _key[6];      // Mifare Classic key
uint8_t mifareclassic_AuthenticateBlock(uint8_t *uid,
                                                        uint8_t uidLen,
                                                        uint32_t blockNumber,
                                                        uint8_t keyNumber,
                                                        uint8_t *keyData) {
  // uint8_t len;
  uint8_t i;

  // Hang on to the key and uid data
  int8_t _uid[7];
  int8_t _key[6];
  memcpy(_key, keyData, 6);
  memcpy(_uid, uid, uidLen);
  int8_t _uidLen = uidLen;

#ifdef MIFAREDEBUG
  print(F("Trying to authenticate card "));
  PrintHex(_uid, _uidLen);
  print(F("Using authentication KEY "));
  print(keyNumber ? 'B' : 'A');
  print(F(": "));
  PrintHex(_key, 6);
#endif

  // Prepare the authentication command //
  pn532_packetbuffer[0] =
      PN532_COMMAND_INDATAEXCHANGE; /* Data Exchange Header */
  pn532_packetbuffer[1] = 1;        /* Max card numbers */
  pn532_packetbuffer[2] = (keyNumber) ? MIFARE_CMD_AUTH_B : MIFARE_CMD_AUTH_A;
  pn532_packetbuffer[3] =
      blockNumber; /* Block Number (1K = 0..63, 4K = 0..255 */
  memcpy(pn532_packetbuffer + 4, _key, 6);
  for (i = 0; i < _uidLen; i++) {
    pn532_packetbuffer[10 + i] = _uid[i]; /* 4 byte card ID */
  }

  if (!sendCommandCheckAck(pn532_packetbuffer, 10 + _uidLen))
    return 0;

  // Read the response packet
  readdata(pn532_packetbuffer, 12);

  // check if the response is valid and we are authenticated???
  // for an auth success it should be bytes 5-7: 0xD5 0x41 0x00
  // Mifare auth error is technically byte 7: 0x14 but anything other and 0x00
  // is not good
  if (pn532_packetbuffer[7] != 0x00) {
#ifdef PN532DEBUG
    print(F("Authentification failed: "));
    PrintHexChar(pn532_packetbuffer, 12);
#endif
    return 0;
  }

  return 1;
}

/**************************************************************************/
/*!
    Tries to read an entire 16-byte data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          Pointer to the byte array that will hold the
                          retrieved data (if any)

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t mifareclassic_ReadDataBlock(uint8_t blockNumber,
                                                    uint8_t *data) {
#ifdef MIFAREDEBUG
  print(F("Trying to read 16 bytes from block "));
  println(blockNumber);
#endif

  /* Prepare the command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;               /* Card number */
  pn532_packetbuffer[2] = MIFARE_CMD_READ; /* Mifare Read command = 0x30 */
  pn532_packetbuffer[3] =
      blockNumber; /* Block Number (0..63 for 1K, 0..255 for 4K) */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 4)) {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Failed to receive ACK for read command");
#endif
    return 0;
  }

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);

  /* If byte 8 isn't 0x00 we probably have an error */
  if (pn532_packetbuffer[7] != 0x00) {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Unexpected response");
    PrintHexChar(pn532_packetbuffer, 26);
#endif
    return 0;
  }

  /* Copy the 16 data bytes to the output buffer        */
  /* Block content starts at byte 9 of a valid response */
  memcpy(data, pn532_packetbuffer + 8, 16);

/* Display data for debug if requested */
#ifdef MIFAREDEBUG
  print(F("Block "));
  println(blockNumber);
  PrintHexChar(data, 16);
#endif

  return 1;
}

/**************************************************************************/
/*!
    Tries to write an entire 16-byte data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          The byte array that contains the data to write.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t mifareclassic_WriteDataBlock(uint8_t blockNumber,
                                                     uint8_t *data) {
#ifdef MIFAREDEBUG
  print(F("Trying to write 16 bytes to block "));
  println(blockNumber);
#endif

  /* Prepare the first command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;                /* Card number */
  pn532_packetbuffer[2] = MIFARE_CMD_WRITE; /* Mifare Write command = 0xA0 */
  pn532_packetbuffer[3] =
      blockNumber; /* Block Number (0..63 for 1K, 0..255 for 4K) */
  memcpy(pn532_packetbuffer + 4, data, 16); /* Data Payload */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 20)) {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Failed to receive ACK for write command");
#endif
    return 0;
  }
  delay(10);

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);

  return 1;
}

/**************************************************************************/
/*!
    Formats a Mifare Classic card to store NDEF Records

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t mifareclassic_FormatNDEF(void) {
  uint8_t sectorbuffer1[16] = {0x14, 0x01, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1,
                               0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
  uint8_t sectorbuffer2[16] = {0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1,
                               0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
  uint8_t sectorbuffer3[16] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0x78, 0x77,
                               0x88, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  // Note 0xA0 0xA1 0xA2 0xA3 0xA4 0xA5 must be used for key A
  // for the MAD sector in NDEF records (sector 0)

  // Write block 1 and 2 to the card
  if (!(mifareclassic_WriteDataBlock(1, sectorbuffer1)))
    return 0;
  if (!(mifareclassic_WriteDataBlock(2, sectorbuffer2)))
    return 0;
  // Write key A and access rights card
  if (!(mifareclassic_WriteDataBlock(3, sectorbuffer3)))
    return 0;

  // Seems that everything was OK (?!)
  return 1;
}

/**************************************************************************/
/*!
    Writes an NDEF URI Record to the specified sector (1..15)

    Note that this function assumes that the Mifare Classic card is
    already formatted to work as an "NFC Forum Tag" and uses a MAD1
    file system.  You can use the NXP TagWriter app on Android to
    properly format cards for this.

    @param  sectorNumber  The sector that the URI record should be written
                          to (can be 1..15 for a 1K card)
    @param  uriIdentifier The uri identifier code (0 = none, 0x01 =
                          "http://www.", etc.)
    @param  url           The uri text to write (max 38 characters).

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t mifareclassic_WriteNDEFURI(uint8_t sectorNumber,
                                                   uint8_t uriIdentifier,
                                                   const char *url) {
  // Figure out how long the string is
  uint8_t len = strlen(url);

  // Make sure we're within a 1K limit for the sector number
  if ((sectorNumber < 1) || (sectorNumber > 15))
    return 0;

  // Make sure the URI payload is between 1 and 38 chars
  if ((len < 1) || (len > 38))
    return 0;

  // Note 0xD3 0xF7 0xD3 0xF7 0xD3 0xF7 must be used for key A
  // in NDEF records

  // Setup the sector buffer (w/pre-formatted TLV wrapper and NDEF message)
  uint8_t sectorbuffer1[16] = {0x00,
                               0x00,
                               0x03,
                               (uint8_t)(len + 5),
                               0xD1,
                               0x01,
                               (uint8_t)(len + 1),
                               0x55,
                               uriIdentifier,
                               0x00,
                               0x00,
                               0x00,
                               0x00,
                               0x00,
                               0x00,
                               0x00};
  uint8_t sectorbuffer2[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t sectorbuffer3[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t sectorbuffer4[16] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7, 0x7F, 0x07,
                               0x88, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  if (len <= 6) {
    // Unlikely we'll get a url this short, but why not ...
    memcpy(sectorbuffer1 + 9, url, len);
    sectorbuffer1[len + 9] = 0xFE;
  } else if (len == 7) {
    // 0xFE needs to be wrapped around to next block
    memcpy(sectorbuffer1 + 9, url, len);
    sectorbuffer2[0] = 0xFE;
  } else if ((len > 7) && (len <= 22)) {
    // Url fits in two blocks
    memcpy(sectorbuffer1 + 9, url, 7);
    memcpy(sectorbuffer2, url + 7, len - 7);
    sectorbuffer2[len - 7] = 0xFE;
  } else if (len == 23) {
    // 0xFE needs to be wrapped around to final block
    memcpy(sectorbuffer1 + 9, url, 7);
    memcpy(sectorbuffer2, url + 7, len - 7);
    sectorbuffer3[0] = 0xFE;
  } else {
    // Url fits in three blocks
    memcpy(sectorbuffer1 + 9, url, 7);
    memcpy(sectorbuffer2, url + 7, 16);
    memcpy(sectorbuffer3, url + 23, len - 24);
    sectorbuffer3[len - 22] = 0xFE;
  }

  // Now write all three blocks back to the card
  if (!(mifareclassic_WriteDataBlock(sectorNumber * 4, sectorbuffer1)))
    return 0;
  if (!(mifareclassic_WriteDataBlock((sectorNumber * 4) + 1, sectorbuffer2)))
    return 0;
  if (!(mifareclassic_WriteDataBlock((sectorNumber * 4) + 2, sectorbuffer3)))
    return 0;
  if (!(mifareclassic_WriteDataBlock((sectorNumber * 4) + 3, sectorbuffer4)))
    return 0;

  // Seems that everything was OK (?!)
  return 1;
}

/***** Mifare Ultralight Functions ******/

/**************************************************************************/
/*!
    @brief   Tries to read an entire 4-byte page at the specified address.

    @param   page        The page number (0..63 in most cases)
    @param   buffer      Pointer to the byte array that will hold the
                         retrieved data (if any)
    @return  1 on success, 0 on error.
*/
/**************************************************************************/
uint8_t mifareultralight_ReadPage(uint8_t page,
                                                  uint8_t *buffer) {
  if (page >= 64) {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Page value out of range");
#endif
    return 0;
  }

#ifdef MIFAREDEBUG
  print(F("Reading page "));
  println(page);
#endif

  /* Prepare the command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;               /* Card number */
  pn532_packetbuffer[2] = MIFARE_CMD_READ; /* Mifare Read command = 0x30 */
  pn532_packetbuffer[3] = page; /* Page Number (0..63 in most cases) */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 4)) {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Failed to receive ACK for write command");
#endif
    return 0;
  }

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);
#ifdef MIFAREDEBUG
  ESP_LOGE("ERROR","Received: ");
  PrintHexChar(pn532_packetbuffer, 26);
#endif

  /* If byte 8 isn't 0x00 we probably have an error */
  if (pn532_packetbuffer[7] == 0x00) {
    /* Copy the 4 data bytes to the output buffer         */
    /* Block content starts at byte 9 of a valid response */
    /* Note that the command actually reads 16 byte or 4  */
    /* pages at a time ... we simply discard the last 12  */
    /* bytes                                              */
    memcpy(buffer, pn532_packetbuffer + 8, 4);
  } else {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Unexpected response reading block: ");
    PrintHexChar(pn532_packetbuffer, 26);
#endif
    return 0;
  }

/* Display data for debug if requested */
#ifdef MIFAREDEBUG
  print(F("Page "));
  print(page);
  ESP_LOGE("ERROR",":");
  PrintHexChar(buffer, 4);
#endif

  // Return OK signal
  return 1;
}

/**************************************************************************/
/*!
    Tries to write an entire 4-byte page at the specified block
    address.

    @param  page          The page number to write.  (0..63 for most cases)
    @param  data          The byte array that contains the data to write.
                          Should be exactly 4 bytes long.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t mifareultralight_WritePage(uint8_t page,
                                                   uint8_t *data) {

  if (page >= 64) {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Page value out of range");
#endif
    // Return Failed Signal
    return 0;
  }

#ifdef MIFAREDEBUG
  print(F("Trying to write 4 byte page"));
  println(page);
#endif

  /* Prepare the first command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1; /* Card number */
  pn532_packetbuffer[2] =
      MIFARE_ULTRALIGHT_CMD_WRITE; /* Mifare Ultralight Write command = 0xA2 */
  pn532_packetbuffer[3] = page;    /* Page Number (0..63 for most cases) */
  memcpy(pn532_packetbuffer + 4, data, 4); /* Data Payload */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 8)) {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Failed to receive ACK for write command");
#endif

    // Return Failed Signal
    return 0;
  }
  delay(10);

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);

  // Return OK Signal
  return 1;
}

/***** NTAG2xx Functions ******/

/**************************************************************************/
/*!
    @brief   Tries to read an entire 4-byte page at the specified address.

    @param   page        The page number (0..63 in most cases)
    @param   buffer      Pointer to the byte array that will hold the
                         retrieved data (if any)
    @return  1 on success, 0 on error.
*/
/**************************************************************************/
uint8_t ntag2xx_ReadPage(uint8_t page, uint8_t *buffer) {
  // TAG Type       PAGES   USER START    USER STOP
  // --------       -----   ----------    ---------
  // NTAG 203       42      4             39
  // NTAG 213       45      4             39
  // NTAG 215       135     4             129
  // NTAG 216       231     4             225

  if (page >= 231) {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Page value out of range");
#endif
    return 0;
  }

#ifdef MIFAREDEBUG
  print(F("Reading page "));
  println(page);
#endif

  /* Prepare the command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;               /* Card number */
  pn532_packetbuffer[2] = MIFARE_CMD_READ; /* Mifare Read command = 0x30 */
  pn532_packetbuffer[3] = page; /* Page Number (0..63 in most cases) */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 4)) {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Failed to receive ACK for write command");
#endif
    return 0;
  }

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);
#ifdef MIFAREDEBUG
  ESP_LOGE("ERROR","Received: ");
  PrintHexChar(pn532_packetbuffer, 26);
#endif

  /* If byte 8 isn't 0x00 we probably have an error */
  if (pn532_packetbuffer[7] == 0x00) {
    /* Copy the 4 data bytes to the output buffer         */
    /* Block content starts at byte 9 of a valid response */
    /* Note that the command actually reads 16 byte or 4  */
    /* pages at a time ... we simply discard the last 12  */
    /* bytes                                              */
    memcpy(buffer, pn532_packetbuffer + 8, 4);
  } else {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Unexpected response reading block: ");
    PrintHexChar(pn532_packetbuffer, 26);
#endif
    return 0;
  }

/* Display data for debug if requested */
#ifdef MIFAREDEBUG
  print(F("Page "));
  print(page);
  ESP_LOGE("ERROR",":");
  PrintHexChar(buffer, 4);
#endif

  // Return OK signal
  return 1;
}

/**************************************************************************/
/*!
    Tries to write an entire 4-byte page at the specified block
    address.

    @param  page          The page number to write.  (0..63 for most cases)
    @param  data          The byte array that contains the data to write.
                          Should be exactly 4 bytes long.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t ntag2xx_WritePage(uint8_t page, uint8_t *data) {
  // TAG Type       PAGES   USER START    USER STOP
  // --------       -----   ----------    ---------
  // NTAG 203       42      4             39
  // NTAG 213       45      4             39
  // NTAG 215       135     4             129
  // NTAG 216       231     4             225

  if ((page < 4) || (page > 225)) {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Page value out of range");
#endif
    // Return Failed Signal
    return 0;
  }

#ifdef MIFAREDEBUG
  print(F("Trying to write 4 byte page"));
  println(page);
#endif

  /* Prepare the first command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1; /* Card number */
  pn532_packetbuffer[2] =
      MIFARE_ULTRALIGHT_CMD_WRITE; /* Mifare Ultralight Write command = 0xA2 */
  pn532_packetbuffer[3] = page;    /* Page Number (0..63 for most cases) */
  memcpy(pn532_packetbuffer + 4, data, 4); /* Data Payload */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 8)) {
#ifdef MIFAREDEBUG
    ESP_LOGE("ERROR","Failed to receive ACK for write command");
#endif

    // Return Failed Signal
    return 0;
  }
  delay(10);

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);

  // Return OK Signal
  return 1;
}

/**************************************************************************/
/*!
    Writes an NDEF URI Record starting at the specified page (4..nn)

    Note that this function assumes that the NTAG2xx card is
    already formatted to work as an "NFC Forum Tag".

    @param  uriIdentifier The uri identifier code (0 = none, 0x01 =
                          "http://www.", etc.)
    @param  url           The uri text to write (null-terminated string).
    @param  dataLen       The size of the data area for overflow checks.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t ntag2xx_WriteNDEFURI(uint8_t uriIdentifier, char *url,
                                             uint8_t dataLen) {
  uint8_t pageBuffer[4] = {0, 0, 0, 0};

  // Remove NDEF record overhead from the URI data (pageHeader below)
  uint8_t wrapperSize = 12;

  // Figure out how long the string is
  uint8_t len = strlen(url);

  // Make sure the URI payload will fit in dataLen (include 0xFE trailer)
  if ((len < 1) || (len + 1 > (dataLen - wrapperSize)))
    return 0;

  // Setup the record header
  // See NFCForum-TS-Type-2-Tag_1.1.pdf for details
  uint8_t pageHeader[12] = {
      /* NDEF Lock Control TLV (must be first and always present) */
      0x01, /* Tag Field (0x01 = Lock Control TLV) */
      0x03, /* Payload Length (always 3) */
      0xA0, /* The position inside the tag of the lock bytes (upper 4 = page
               address, lower 4 = byte offset) */
      0x10, /* Size in bits of the lock area */
      0x44, /* Size in bytes of a page and the number of bytes each lock bit can
               lock (4 bit + 4 bits) */
      /* NDEF Message TLV - URI Record */
      0x03,               /* Tag Field (0x03 = NDEF Message) */
      (uint8_t)(len + 5), /* Payload Length (not including 0xFE trailer) */
      0xD1, /* NDEF Record Header (TNF=0x1:Well known record + SR + ME + MB) */
      0x01, /* Type Length for the record type indicator */
      (uint8_t)(len + 1), /* Payload len */
      0x55,               /* Record Type Indicator (0x55 or 'U' = URI Record) */
      uriIdentifier       /* URI Prefix (ex. 0x01 = "http://www.") */
  };

  // Write 12 byte header (three pages of data starting at page 4)
  memcpy(pageBuffer, pageHeader, 4);
  if (!(ntag2xx_WritePage(4, pageBuffer)))
    return 0;
  memcpy(pageBuffer, pageHeader + 4, 4);
  if (!(ntag2xx_WritePage(5, pageBuffer)))
    return 0;
  memcpy(pageBuffer, pageHeader + 8, 4);
  if (!(ntag2xx_WritePage(6, pageBuffer)))
    return 0;

  // Write URI (starting at page 7)
  uint8_t currentPage = 7;
  char *urlcopy = url;
  while (len) {
    if (len < 4) {
      memset(pageBuffer, 0, 4);
      memcpy(pageBuffer, urlcopy, len);
      pageBuffer[len] = 0xFE; // NDEF record footer
      if (!(ntag2xx_WritePage(currentPage, pageBuffer)))
        return 0;
      // DONE!
      return 1;
    } else if (len == 4) {
      memcpy(pageBuffer, urlcopy, len);
      if (!(ntag2xx_WritePage(currentPage, pageBuffer)))
        return 0;
      memset(pageBuffer, 0, 4);
      pageBuffer[0] = 0xFE; // NDEF record footer
      currentPage++;
      if (!(ntag2xx_WritePage(currentPage, pageBuffer)))
        return 0;
      // DONE!
      return 1;
    } else {
      // More than one page of data left
      memcpy(pageBuffer, urlcopy, 4);
      if (!(ntag2xx_WritePage(currentPage, pageBuffer)))
        return 0;
      currentPage++;
      urlcopy += 4;
      len -= 4;
    }
  }

  // Seems that everything was OK (?!)
  return 1;
}

/************** high level communication functions (handles both I2C and SPI) */

/**************************************************************************/
/*!
    @brief  Tries to read the SPI or I2C ACK signal
*/
/**************************************************************************/
bool readack() {
  // uint8_t ackbuff[6]=
  uint8_t ackbuff[6];
    // readdata(ackbuff, 6);
// pn532_read(ackbuff, 6);
  uart_read_bytes(UART_NUM_P2,ackbuff,6,portMAX_DELAY);

  return (0 == memcmp((char *)ackbuff, (char *)pn532ack, 6));
}

/**************************************************************************/
/*!
    @brief  Return true if the PN532 is ready with a response.
*/
/**************************************************************************/
bool isready() {
      
    // size_t length = 0;
    // Lấy độ dài của dữ liệu trong bộ đệm
    // uart_get_buffered_data_len(UART_NUM, &length);
    size_t *length;
//  uint8_t *buffer;
// get_buffer(&length);
    get_buffer(&length);
printf("%zu ",*length);
    // return length;
    // Serial ready check based on non-zero read buffer
    return (length != 0);

  // return false;
}

/**************************************************************************/
/*!
    @brief  Waits until the PN532 is ready.

    @param  timeout   Timeout before giving up
*/
/**************************************************************************/
bool waitready(uint16_t timeout) {
  uint16_t timer = 0;
  while (!isready()) {
    if (timeout != 0) {
      timer += 10;
      if (timer > timeout) {
// #ifdef PN532DEBUG
        printf("TIMEOUT! \n");
// #endif
        return false;
      }
    }
    delay(10);
  }
  return true;
}

/**************************************************************************/
/*!
    @brief  Reads n bytes of data from the PN532 via SPI or I2C.

    @param  buff      Pointer to the buffer where data will be written
    @param  n         Number of bytes to be read
*/
/**************************************************************************/

// int pn532_read(const uint8_t *buffer, size_t len_buffer){
//     // ESP_LOGE("uart_read_bytes","uart_read_bytes");
//     //  uart_read_bytes(UART_NUM, &buffer, len_buffer, portMAX_DELAY);
//     return uart_read_bytes(UART_NUM,&buffer, len_buffer,portMAX_DELAY);
//     // return 1;
// }
void readdata(uint8_t *buff, uint8_t len) {
    // readBytes(buff, n);
  // uart_read_bytes(UART_NUM,&buff, len,portMAX_DELAY);
  // pn532_read(buff,len);
    uart_read_bytes(UART_NUM_P2,buff, len,portMAX_DELAY);

  
#ifdef PN532DEBUG
  print(F("Reading: "));
  for (uint8_t i = 0; i < n; i++) {
    print(F(" 0x"));
    print(buff[i], HEX);
  }
  println();
#endif
}

/**************************************************************************/
/*!
    @brief   set the PN532 as iso14443a Target behaving as a SmartCard
    @return  true on success, false otherwise.
    @note    Author: Salvador Mendoza (salmg.net) new functions:
             -AsTarget
             -getDataTarget
             -setDataTarget
*/
/**************************************************************************/
// bool AsTarget() {
//   pn532_packetbuffer[0] = 0x8C;
//   uint8_t target[] = {
//       0x8C,             // INIT AS TARGET
//       0x00,             // MODE -> BITFIELD
//       0x08, 0x00,       // SENS_RES - MIFARE PARAMS
//       0xdc, 0x44, 0x20, // NFCID1T
//       0x60,             // SEL_RES
//       0x01, 0xfe, // NFCID2T MUST START WITH 01fe - FELICA PARAMS - POL_RES
//       0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xc0,
//       0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, // PAD
//       0xff, 0xff,                               // SYSTEM CODE
//       0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
//       0x33, 0x22, 0x11, 0x01, 0x00, // NFCID3t MAX 47 BYTES ATR_RES
//       0x0d, 0x52, 0x46, 0x49, 0x44, 0x49, 0x4f,
//       0x74, 0x20, 0x50, 0x4e, 0x35, 0x33, 0x32 // HISTORICAL BYTES
//   };
//   if (!sendCommandCheckAck(target, sizeof(target)))
//     return false;

//   // read data packet
//   readdata(pn532_packetbuffer, 8);

//   int offset = 6;
//   return (pn532_packetbuffer[offset] == 0x15);
// }
/**************************************************************************/
/*!
    @brief   Retrieve response from the emulation mode

    @param   cmd    = data
    @param   cmdlen = data length
    @return  true on success, false otherwise.
*/
/**************************************************************************/
uint8_t getDataTarget(uint8_t *cmd, uint8_t *cmdlen) {
  uint8_t length;
  pn532_packetbuffer[0] = 0x86;
  if (!CommandCheckAck(pn532_packetbuffer, 1, 1000)) {
    ESP_LOGE("ERROR","Error en ack");
    return false;
  }

  // read data packet
  readdata(pn532_packetbuffer, 64);
  length = pn532_packetbuffer[3] - 3;

  // if (length > *responseLength) {// Bug, should avoid it in the reading
  // target data
  //  length = *responseLength; // silent truncation...
  //}

  for (int i = 0; i < length; ++i) {
    cmd[i] = pn532_packetbuffer[8 + i];
  }
  *cmdlen = length;
  return true;
}

/**************************************************************************/
/*!
    @brief   Set data in PN532 in the emulation mode

    @param   cmd    = data
    @param   cmdlen = data length
    @return  true on success, false otherwise.
*/
/**************************************************************************/
uint8_t setDataTarget(uint8_t *cmd, uint8_t cmdlen) {
  uint8_t length;
  // cmd1[0] = 0x8E; Must!

  if (!sendCommandCheckAck(cmd, cmdlen))
    return false;

  // read data packet
  readdata(pn532_packetbuffer, 8);
  length = pn532_packetbuffer[3] - 3;
  for (int i = 0; i < length; ++i) {
    cmd[i] = pn532_packetbuffer[8 + i];
  }
  // cmdl = 0
  cmdlen = length;

  int offset = 6;
  return (pn532_packetbuffer[offset] == 0x15);
}

/**************************************************************************/
/*!
    @brief  Writes a command to the PN532, automatically inserting the
            preamble and required frame details (checksum, len, etc.)

    @param  cmd       Pointer to the command buffer
    @param  cmdlen    Command length in bytes
*/
/**************************************************************************/
void writecommand(uint8_t *cmd, uint8_t cmdlen) {
    // I2C or Serial command write.
    uint8_t packet[8 + cmdlen];
    uint8_t LEN = cmdlen + 1;

    packet[0] = PN532_PREAMBLE;
    packet[1] = PN532_STARTCODE1;
    packet[2] = PN532_STARTCODE2;
    packet[3] = LEN;
    packet[4] = ~LEN + 1;
    packet[5] = PN532_HOSTTOPN532;
    uint8_t sum = 0;
    for (uint8_t i = 0; i < cmdlen; i++) {
      packet[6 + i] = cmd[i];
      sum += cmd[i];
    }
    packet[6 + cmdlen] = ~(PN532_HOSTTOPN532 + sum) + 1;
    packet[7 + cmdlen] = PN532_POSTAMBLE;

// #ifdef PN532DEBUG
    printf("Sending : ");
    for (int i = 1; i < 8 + cmdlen; i++) {
      printf("0x");
      printf("%u",packet[i]);
      printf(", ");
    }
    printf("\n");
// #endif

  // int len = pn532_write(packet, 8 + cmdlen);
  int len = uart_write_bytes(UART_NUM_P2, (const char*)packet, 8+cmdlen);
  if (len == 8 + cmdlen) {
        ESP_LOGI("UART", "pn532_write");
    } else {
        ESP_LOGE("ERROR", "pn532_write");
    }
  
}
