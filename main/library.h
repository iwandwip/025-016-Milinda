
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <FirebaseClient.h>
#include "ExampleFunctions.h"  // Provides the functions used in the examples.
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MLX90614.h>
#include "AnalogUVSensor.h"

//== MLX90614 IR Sensor ============
double new_emissivity = 0.95;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
volatile float mlxVal ;
//===========================


// ============GUVA S12SD UV Sensor=====================
#define guvaPin 33
#define uvPin 14
AnalogUVSensor guva;
volatile float guvaVal ;



// ============Soil Moisture Sensor=====================
#define soilPin 36
volatile float soilVal ;

//=========================== LCD ============================
LiquidCrystal_I2C lcd (0x27 , 16 , 2 );
char buff[100];




//=========================== Network ============================

#define API_KEY "AIzaSyALx_N-SrPskXIXtxO7XpXct6M_IT3r2kA"
#define DATABASE_URL "https://farahapps-a9cb3-default-rtdb.firebaseio.com/"
#define USER_EMAIL "admin@gmail.com"
#define USER_PASSWORD "admin123"
#define WIFI_SSID "TIMEOSPACE"
#define WIFI_PASSWORD "1234Saja"


void processData(AsyncResult &aResult);
void set_async();

SSL_CLIENT ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult databaseResult;

bool taskComplete = false;