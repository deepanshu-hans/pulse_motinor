#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define PULSE_PIN 0
#define ONE_WIRE_BUS D5

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

IPAddress server_addr(192, 168, 0, 5);
/*char user[] = "u123130497_jou";
  char password[] = "divya123";*/

char user[] = "csingh";
char password[] = "password";

// WiFi Connection Parameters
/*char ssid[] = "Nik";
  char pass[] = "bapspmahant31";*/

char ssid[] = "Tenda";
char pass[] = "Singhsp+";

float val = 32.0;

WiFiClient client;
MySQL_Connection conn((Client *)&client);
bool connSt;
char INSERT_SQL_DB[] = "INSERT INTO u123130497_journ.data (pulse_val, pulse_stat, temp_val, temp_stat) VALUES ('%s', '%s', '%s', '%s')";
char mainQuery[128];

void setup() {
  pinMode(PULSE_PIN, INPUT);
  Serial.begin(9600);
  //sensors.begin();
  delay(500);
  connectWIFI();
  connectDB();
}

void loop() {
  //sensors.requestTemperatures();
  String pStat = "";
  String tStat = "";

  //float temperatureVal = sensors.getTempCByIndex(0);
  float pulseVal = analogRead(PULSE_PIN);

  if (connSt == false) {
    connectDB();
  }

  if (pulseVal < 500) {
    pStat = "Normal";
  } else {
    pStat = "High";
  }

  if (val < 40) {
    tStat = "Normal";
  } else {
    tStat = "High";
  }

  //Values to String
  String plse, temp = "";
  temp += String(int(val));
  char tVal[temp.length() + 1];
  temp.toCharArray(tVal, temp.length() + 1);

  plse += String(int(pulseVal));
  char pVal[plse.length() + 1];
  plse.toCharArray(pVal, plse.length() + 1);

  //Message to CharArray
  char cPlse[pStat.length() + 1];
  char cTemp[tStat.length() + 1];
  tStat.toCharArray(cTemp, tStat.length() + 1);
  pStat.toCharArray(cPlse, pStat.length() + 1);

  Serial.print("Pulse: ");
  Serial.print(pVal);
  Serial.print(", Temp: ");
  Serial.println(tVal);

  //Insert into DB
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  sprintf(mainQuery, INSERT_SQL_DB, pVal, cPlse, tVal, cTemp);
  cur_mem->execute(mainQuery);
  delete cur_mem;
  closeDB();

  val = val + (0.1 * random(1, 5));
  if (val > 33.5) {
    val = 32.2;
  }

  delay(5000);
}

void connectDB() {
  Serial.println("Connecting to database...");
  if (conn.connect(server_addr, 3306, user, password)) {
    connSt = true;
    Serial.println("DB Connection Created");
  } else {
    Serial.println("A Connection Could not be Established with Database, Trying Again");
    connSt = false;
  }
}

void closeDB() {
  conn.close();
  connSt = false;
  Serial.println("DB Connection Closed");
}

void connectWIFI() {
  WiFi.begin(ssid, pass);
  int count = 0;
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    if (count > 60) {
      count = 0;
      WiFi.disconnect();
      ESP.eraseConfig();
      ESP.restart();
    }
    delay(500);
    Serial.print(".");
    count++;
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print ("IP address: ");
  Serial.println (WiFi.localIP());

  Serial.println("Server started");
  WiFi.mode(WIFI_STA);
}

