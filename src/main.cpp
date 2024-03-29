#include <Arduino.h>
#include <ESP_Google_Sheet_Client.h>

#if defined(ESP32)
#include <WiFi.h>

#elif defined(ESP8266)
#include <ESP8266WiFi.h>

#endif

#include <HTTPClient.h>
// Alterado para a biblioteca do sensor LM35
#define LM35_PIN 34
#define BUTTON_PIN 32
// Your WiFi credentials.
// Set password to "" for open networks.
#define CONNECTION_TIMEOUT_MS 60000
#define WIFI_SSID "Moto LuisV"
#define WIFI_PASSWORD "morcegoo"
#define PROJECT_ID "projeto-sucesu"
#define CLIENT_EMAIL "projeto-sucesu@projeto-sucesu.iam.gserviceaccount.com"
#define USER_EMAIL "brassilomonitoramento@gmail.com"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCnCevBIop6lN6i\ngpV3z01m3v+E3Gq1wvuJSasrYS9sa4wLE1ETb85nUd/BSSZ4/DVrHrR1V/mpntnh\nNNZubXmP+FRNG7Bpu3vmj4LkUf7xXVixr7VCAVDnpQ7hyQTNIyzj7Sf/rTExOhUE\neC9YJS+/4w6C8bnX9jVyUyCYvGaNFhQcifNzDY9oZkTCTh5qzNCy2NHabKZj6D/S\nEXcmmXO5QwuwIH9oomJMSzRSJ27FRDC6gePcXcv7OCjNILcSd6UF3jTQsB4uBclW\nNTBkdL3ZE/woJM6P3PcKQdd7FLMvISfA0v3iOgyG3ScPR7YCRVC9Gpi0cSGw2pVt\ngE9OA6hdAgMBAAECggEAAwJS8YJXLxW6Qpn9nVBQgaQbKeAnbMM1cTSwu7Hr5WHo\nMbuuwARJZv0UZDcwIglqrbvtqxhJn0k4SmFMSsyRqwrhlReeZvLORmObC25N6Thl\nETzFrbzrIZeMnBUMXBgThlhoWkFbDufDJyoPGEebNs0dUgIoXRnKi31J3taBNm6b\ntt4eVTha6/bpZLyuKJ0DenTZuyWHba//4oMRBpNLBNRWNAarfC/FfhUx/LS5StZv\neZsI1MzNMdaFvmDs2cde9tSLQjqKufJVTRtK5Gc9J9+jPlOjC5ThK7zoD7iIFECG\nd+RGQnzYgSkGdhK2X7WXNJvV0ecsELY801O2vFdw1QKBgQDfzJcjrhcsuikYG4yC\nxnHROwT2OB1vwlEzQT8ZiUC823CpPYNvg5BmCmX1opRMrHyHtQNWL1X+P4wKEONS\n34aMWhYDPZtVpHETBnmrA7RkUQG8QJvhbgAN6QudvcxspgINyLZ0kMfNt5BCMXS6\nL8bh/IGy9EfJ3FRVeQzyP3Hb6wKBgQC/Ep6MQlHRPhUnsmf9STPRIteXHRIR+1ry\nhVb/dEBi4UHrXivkNhGqJoLqOhE/P5D+DojS1wv31aro5NxI2tBSnDCEabVMGJhN\nIs4KEAP4D0sfbgA8tUlMDD+X129DX5L/VnLkZhEEN/ZIrYSmut266AJ95e/B7tOL\nd0LSRJxi1wKBgAalMo+yqamAVZwLpV50ugh2ci1FeUpKQNL+yvrdInj5Ix1DYFW9\nWFpPULu8eh7UuFXnQBQdD7ae+G+MFNPvIzWjryeleqlOWrUv6gjz8TVRWnXWjUpi\nU8oV82xZqGpJe6IhJZnMesJXAweUJF0q8gLf1pITwtXxnudy0rmWvLDFAoGAJkWN\nelyiVL4RsLTcfXBXL7HB6yPIhc+KBP/OUDax2txxvrpLIlfuK92w5UeiQr/xw+GX\naFvoqsiihimpMfKaPaHJWgxuYwKeJv0stflFQj+V9YKsC3zoZJIjfr+JKr6PDBP9\nyV/kUqr+6snOwzK0zQZB0YPoBTlYKBxIRyO1C1sCgYBTdRb+v6V68ydK183TlJ5r\nRrwa7AMSGfvAUp+UEKw68Ph57DKwKflmYmoZvwMO+IplEe9fuILcacZb7ESe16fe\neG9lUpDGbSb4O1c6/vVBjMSbw6uch64xFmyUr6yz/vUn+KEkqx/nk/MHI4x61KdZ\npgRwlWVVKbBkfCj9bHJutw==\n-----END PRIVATE KEY-----\n";

void tokenStatusCallback(TokenInfo info){
    if (info.status == esp_signer_token_status_error){
        Serial.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        Serial.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
    else{
        Serial.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    }
}

bool taskComplete = false;

int SensValue = 0;
char numberArray[20];

const String GAS_ID = "AKfycbwhMUnoPkY6-rGbyvkWemEaDAFfjvUWTTkGkFP2Zzb_V5Jp0v5LAAg2RQA368RB_Xf1"; //--> spreadsheet script ID

// Your Domain name with URL path or IP address with path
const char* host = "script.google.com"; // only google.com not https://google.com

// Intervalo de atualização (em milissegundos)
const unsigned long UPDATE_INTERVAL_MS = 1000; // 30 segundos

float temperature;

void update_google_sheet(float temp) {
    // WiFiClient client;
  
    // // Monta a URL com os dados a serem enviados
    // String url = "/macros/s/" + GAS_ID + "/exec?temperature=";
    // url += String(temp);
   
    // Serial.print("Requesting URL: ");
    // Serial.println(url);
    
    // // Realiza a requisição GET
    // if (client.connect(host, 80)) {
    //     client.print(String("GET ") + url + " HTTP/1.1\r\n" +
    //                  "Host: " + host + "\r\n" +
    //                  "Connection: close\r\n\r\n");
    //     delay(10); // Aguarda o envio dos dados
    //     client.stop(); // Encerra a conexão
    // } else {
    //     Serial.println("Connection failed");
    // }
}

bool ready = false;
FirebaseJson spreadsheet;
FirebaseJson response;
String spreadsheetId, spreadsheetURL;
void setup() {
    Serial.begin(9600);
    pinMode(BUTTON_PIN, INPUT_PULLDOWN);

    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connection to WiFi");
    unsigned long connection_begin_ms = millis();
    while(WiFi.status() != WL_CONNECTED && millis() - connection_begin_ms < CONNECTION_TIMEOUT_MS){
        Serial.print(".");
        delay(300);
    }
    Serial.println("");
    if(WiFi.status() != WL_CONNECTED){
        Serial.print("[ERROR] Connection failed!");
        return;
    }
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());

    GSheet.setTokenCallback(tokenStatusCallback);
    // Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
    GSheet.setPrerefreshSeconds(10 * 60);

    GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

    ready = GSheet.ready();
    if (ready && !taskComplete){

        Serial.println("\nCreate spreadsheet...");
        Serial.println("------------------------");

        spreadsheet.set("properties/title", "Temperatura [°C]");
        spreadsheet.set("sheets/properties/gridProperties/rowCount", 100 );
        spreadsheet.set("sheets/properties/gridProperties/columnCount", 2);

        bool success = false;

        // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets/create
        Serial.println("Crating sheet");
        while(!success) success = GSheet.create(&response /* returned response */, &spreadsheet /* spreadsheet object */, USER_EMAIL /* your email that this spreadsheet shared to */);
        Serial.println("Seeht created");
        response.toString(Serial, true);
        Serial.println();

        if (success){
            // Get the spreadsheet id from already created file.
            FirebaseJsonData result;
            response.get(result, FPSTR("spreadsheetId")); // parse or deserialize the JSON response
            if(result.success)
                spreadsheetId = result.to<const char *>();

            // Get the spreadsheet URL.
            result.clear();
            response.get(result, FPSTR("spreadsheetUrl")); // parse or deserialize the JSON response
            if (result.success){
                spreadsheetURL = result.to<const char *>();
                Serial.println("\nThe spreadsheet URL");
                Serial.println(spreadsheetURL);
            }
        }
    }
}

unsigned long lastUpdateTime = 0;
uint16_t counter = 0;
bool last_button_state = true;
void loop() {
    // Call ready() repeatedly in loop for authentication checking and processing
    // bool ready = GSheet.ready();

    bool success = false;
    ready = GSheet.ready();
    if (ready && !taskComplete){
    //     FirebaseJson response;

    //     Serial.println("\nCreate spreadsheet...");
    //     Serial.println("------------------------");

    //     FirebaseJson spreadsheet;
    //     spreadsheet.set("properties/title", "Gas Sensor Data Log");
    //     spreadsheet.set("sheets/properties/gridProperties/rowCount", 100 );
    //     spreadsheet.set("sheets/properties/gridProperties/columnCount", 2);

    //     String spreadsheetId, spreadsheetURL;
    //     bool success = false;

    //     // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets/create

    //     success = GSheet.create(&response /* returned response */, &spreadsheet /* spreadsheet object */, USER_EMAIL /* your email that this spreadsheet shared to */);
    //     response.toString(Serial, true);
    //     Serial.println();

    //     if (success){
    //         // Get the spreadsheet id from already created file.
    //         FirebaseJsonData result;
    //         response.get(result, FPSTR("spreadsheetId")); // parse or deserialize the JSON response
    //         if(result.success)
    //             spreadsheetId = result.to<const char *>();

    //         // Get the spreadsheet URL.
    //         result.clear();
    //         response.get(result, FPSTR("spreadsheetUrl")); // parse or deserialize the JSON response
    //         if (result.success){
    //             spreadsheetURL = result.to<const char *>();
    //             Serial.println("\nThe spreadsheet URL");
    //             Serial.println(spreadsheetURL);
    //         }

            struct tm timeinfo;
            char timeStringBuff[50];
            String asString;
            char buffer[40];


            FirebaseJson valueRange;
            while(true){
                bool button_state = digitalRead(BUTTON_PIN);
                // Serial.println(button_state);
                if(!button_state || last_button_state){
                    last_button_state = button_state;
                    continue;
                }
                // Serial.println("in");
            // for (int counter = 0; counter < 10; counter++){
                Serial.println("\nUpdate spreadsheet values...");
                Serial.println("------------------------------");
                unsigned long connection_begin_ms = millis();
                while(WiFi.status() != WL_CONNECTED && millis() - connection_begin_ms < CONNECTION_TIMEOUT_MS){
                    Serial.print(".");
                    delay(300);
                }
                if (!getLocalTime(&timeinfo)) {
                    Serial.println("Failed to obtain time");
                    return;
                }
                strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
                asString = timeStringBuff;
                asString.replace(" ", "-");
                // Lugar de ler o valor do sensor
                SensValue = analogRead(LM35_PIN);
                SensValue = map(SensValue, 0, 1023, 2, 150);
                itoa(SensValue, numberArray, 10);

                sprintf(buffer, "values/[%d]/[1]", counter);
                valueRange.set(buffer, numberArray);
                sprintf(buffer, "values/[%d]/[0]", counter);
                valueRange.set(buffer, asString);
                sprintf(buffer, "Sheet1!A%d:B%d", 1 + counter, 10 + counter);

                success = GSheet.values.update(&response /* returned response */, spreadsheetId /* spreadsheet Id to update */, "Sheet1!A1:B1000" /* range to update */, &valueRange /* data to update */);
                response.toString(Serial, true);
                Serial.println();
                //valueRange.clear();
                counter++;
            }

            // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/update

        //     if (success){
        //         Serial.println("\nGet spreadsheet values...");
        //         Serial.println("------------------------------");

        //         success = GSheet.values.get(&response /* returned response */, spreadsheetId /* spreadsheet Id to read */, "Sheet1!A1:B10" /* range to read */);
        //         response.toString(Serial, true);
        //         Serial.println();

        //         Serial.println(ESP.getFreeHeap());
        //     }
        // }

        // taskComplete = true;
    }

    // Verifica se é hora de ler o sensor e atualizar o Google Sheets
    // if (millis() - lastUpdateTime >= UPDATE_INTERVAL_MS) {
    //     lastUpdateTime = millis();
    //     // Leitura do sensor LM35
    //     int sensorValue = analogRead(LM35_PIN);
    //     // Conversão para temperatura em Celsius
    //     temperature = ((sensorValue / 4095.0) * 3300 / 10.0)*2;
    //     // Envia os dados para o Google Sheets
    //     update_google_sheet(temperature);
    //     Serial.print("Temperature: ");
    //     Serial.print(temperature);
    //     Serial.println(" °C");
    // }
}