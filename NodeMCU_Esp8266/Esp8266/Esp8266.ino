//Include Lib for Arduino to Nodemcu
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// Load Wi-Fi library
#include <ESP8266WiFi.h>

//Network credentials
const char* ssid     = "Sossy";
const char* password = "sumer319";

// Set web server port number to 80
WiFiServer server(80);

//D6 = Tx & D5 = Rx
SoftwareSerial nodemcu(D5, D6);


// Arrays of image links for "good" and "bad" categories
const char* goodPlantImages[] = {
  "https://img.freepik.com/premium-vector/leuke-karakter-kamerplant-in-een-pot-met-bril-en-vrolijke-emoties-lachend-gezicht-blije-ogen-armen-en-benen-planter-met-vetplant-voor-interieurdecoratie_427567-1280.jpg?size=626&ext=jpg",
  "https://img.freepik.com/premium-vector/leuke-karakter-kamerplant-in-een-pot-wordt-verliefd-op-ogen-harten-kus-gezicht-armen-en-benen-plantenbak_427567-1436.jpg?size=626&ext=jpg",
  "https://thumbs.dreamstime.com/b/cute-happy-smiling-plant-pot-show-muscle-cute-happy-smiling-plant-pot-show-muscle-biceps-vector-flat-cartoon-illustration-157203359.jpg"
};

const char* badPlantImages[] = {
  "https://img.freepik.com/premium-vector/leuke-karakter-kamerplant-in-een-pot-met-huilen-en-tranen-emotie-droevig-gezicht-depressieve-ogen-armen-en-benen-planter-met-vetplant-voor-interieurdecoratie_427567-1284.jpg?size=626&ext=jpg",
  "https://img.freepik.com/premium-vector/cute-character-indoor-plant-pot-with-emotions-panic-grabs-his-head-surprised-face_427567-1340.jpg?size=626&ext=jpg",
  "https://img.freepik.com/premium-vector/cute-sad-funny-monstera-plant-pot-vector-cartoon-character-illustration-design-isolated_92289-1280.jpg"
};

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output1State = "off";
String output2State = "off";

// Assign output variables to GPIO pins
const int output1 = D1;
const int output2 = D2;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

float temp;
int humidity;
float light;
int flag0=0;
int flag1=0;
int starePlanta=1;

void setup() {
  // Initialize Serial port
  Serial.begin(115200);
  nodemcu.begin(115200);
 
  // Initialize the output variables as outputs
  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
 
  // Set outputs to LOW
  digitalWrite(output1, LOW);
  digitalWrite(output2, LOW);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  while (!Serial) continue;
}

void loop() {

   StaticJsonDocument<1000> data; 
  DeserializationError error = deserializeJson(data, nodemcu);
if (error)
  return;

  Serial.println("JSON Object Recieved");
  Serial.print("Recieved Humidity:  ");
  humidity = data["humidity"];
  Serial.println(humidity);
  Serial.print("Recieved temp:  ");
  temp = data["temperature"];
  Serial.println(temp);
   Serial.print("Recieved Light:  ");
  light = data["isLight"];
  Serial.println(light);
  Serial.println("-----------------------------------------");



  starePlanta=calculateState(temp,humidity,light);
  
   WiFiClient client = server.available();

   if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    unsigned long currentTime = millis();
    unsigned long previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= 2000) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

 if (header.indexOf("GET /1/on HTTP/1.1") >= 0) {
              Serial.println("Pump 1 on");
              output1State = "on";
              //digitalWrite(output1,HIGH); 
              flag0=1; 
            } else if (header.indexOf("GET /1/off") >= 0) {
              Serial.println("Pump 1 off");
              output1State = "off";
              //digitalWrite(output1, LOW);
              flag0=0;
            } else if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("Artificial LED 2 on");
              output2State = "on";
              flag1=2;
              //digitalWrite(output2, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("Artificial LED 2 off");
              output2State = "off";
              flag1=3;
              //digitalWrite(output2, LOW);
            }
            
             

           client.println("<!DOCTYPE html><html>");
client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
client.println("<link rel=\"icon\" href=\"data:,\">");
client.println("<style>");
client.println("html, body { height: 100%; margin: 0; display: flex; flex-direction: column; align-items: center; justify-content: center; }");
client.println(".sensor { font-size: 24px; margin: 20px 10px; text-align: center; }"); // Adjusted margin
client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
client.println("text-decoration: none; font-size: 24px; margin: 10px; cursor: pointer; }");
client.println("#centeredImage { max-width: 100%; height: auto; margin: 20px 0; }"); // Adjusted margin
client.println("</style></head>");

 // Web Page Heading
 switch(starePlanta){
    case 0:
      client.println("<body><h1>Refresh the page, still calculating the plant's condition...</h1>");
      break;
    case 1:
      client.println("<body><h1>The plant is feeling the love</h1>");
      break;
    case 2:
      client.println("<body><h1 style=\"font-size: 24px; margin: 20px 0;\">The plant doesn't like the climate change</h1>");
      break;
    case 3:
      client.println("<body><h1 style=\"font-size: 24px; margin: 20px 0;\">The plant needs a drink</h1>");
      break;
    case 5:
      client.println("<body><h1 style=\"font-size: 24px; margin: 20px 0;\">The plant wants to get some tan</h1>");
      break;
    case 6:
      client.println("<body><h1 style=\"font-size: 24px; margin: 20px 0;\">The plant needs some water and a blanket</h1>");
      break;
    case 10:
      client.println("<body><h1 style=\"font-size: 24px; margin: 20px 0;\">The plant would like some water and light</h1>");
      break;
    case 15:
      client.println("<body><h1 style=\"font-size: 24px; margin: 20px 0;\">The plant wants the temperature to be better and to see the sun</h1>");
      break;
    case 30:
      client.println("<body><h1 style=\"font-size: 24px; margin: 20px 0;\">The plant is barely alive, give it all it needs </h1>");
      break;
    default:
      client.println("<body><h1 style=\"font-size: 24px; margin: 20px 0;\">Refresh the page, still calculating the plant's condition...</h1>");
      break;
  }
            
client.println("<div class=\"sensor\">");
client.println("Temperature: " + String(temp, 2) + " C<br>");
client.println("Humidity: " + String(humidity) + " wfv");

//Button for Humidity
// Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>Pump - State " + output1State + "</p>");
            // If the output5State is off, it displays the ON button       
          if (output1State=="off") {
              client.println("<p><a href=\"/1/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/1/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 


if (light == 0)
    client.println("<br>The plant has access to light ");
else
    client.println("<br>The plant is starved of light ");

// Button for light intensity control
  client.println("<p>Artificial LED - State " + output2State + "</p>");
            // If the output5State is off, it displays the ON button       
          if (output2State=="off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 

client.println("</div>");
client.println("</body></html>");

            // Determine which plant image to show based on conditions
            if (temp >= 20 && temp <= 30 &&
                humidity >= 200 && humidity <= 800 &&
                light == 0) {
              // Show a random image from the "good" category
              int randomIndex = random(0, sizeof(goodPlantImages) / sizeof(goodPlantImages[0]));
              client.println("<img class=\"plant\" src=\"" + String(goodPlantImages[randomIndex]) + "\" alt=\"Happy Plant\">");
            } else {
              // Show a random image from the "bad" category
              int randomIndex = random(0, sizeof(badPlantImages) / sizeof(badPlantImages[0]));
              client.println("<img class=\"plant\" src=\"" + String(badPlantImages[randomIndex]) + "\" alt=\"Sad Plant\">");
            }

            client.println("</body></html>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
     
    }
    
    header = "";

    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  sendFlag(flag0,flag1);
}

void sendFlag(int flag0, int flag1){
  StaticJsonDocument<1000> data2;
  Serial.print("Valoare de flag0 e ");
  Serial.println(flag0);
  Serial.print("Valoare de flag1 e ");
  Serial.println(flag1);
   data2["flag0"]=flag0;
   //data2["flag1"]=flag1;
   Serial.print("Are size:");
Serial.println(data2.size());
if(data2.isNull()==true)
Serial.println("E null" );
   serializeJson(data2,nodemcu);
   delay(2000);
}


int calculateState(float temp,int humidity,float light){
   int starePlanta=1;
   if(temp==0 && humidity == 0 && light ==0)
   starePlanta=0;
  if(temp <20 || temp >30)
  starePlanta*=2;

  if(humidity<200 || humidity >800)
  starePlanta*=3;

  if(light==1)
  starePlanta*=5;

return starePlanta;
}

