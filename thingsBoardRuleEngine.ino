/* Ejemplo para utilizar la comunicación con thingboards y el motor de Reglas con nodeMCU v2
 *  Gastón Mousqués
 *  Basado en varios ejemplos de la documentación de  https://thingsboard.io
 *  
 */

// includes de bibliotecas para comunicación
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>


//***************MODIFICAR PARA SU PROYECTO *********************
//  configuración datos wifi 
// decomentar el define y poner los valores de su red y de su dispositivo
#define WIFI_AP "SSID RED"
#define WIFI_PASSWORD "PASSWORD RED"



//  configuración datos thingsboard
#define NODE_NAME "NOMBRE DISPOSITIVO"   //nombre que le pusieron al dispositivo cuando lo crearon
#define NODE_TOKEN "TOKEN DISPOSITIVO"   //Token que genera Thingboard para dispositivo cuando lo crearon


//***************NO MODIFICAR *********************
char thingsboardServer[] = "demo.thingsboard.io";

/*definir topicos.
 * telemetry - para enviar datos de los sensores
 * request - para recibir una solicitud y enviar datos 
 * attributes - para recibir comandos en baes a atributtos shared definidos en el dispositivo
 */
char telemetryTopic[] = "v1/devices/me/telemetry";
char requestTopic[] = "v1/devices/me/rpc/request/+";  //RPC - El Servidor usa este topico para enviar rquests, cliente response
char attributesTopic[] = "v1/devices/me/attributes";  // Permite recibir o enviar mensajes dependindo de atributos compartidos


// declarar cliente Wifi y PubSus
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// declarar variables control loop (para no usar delay() en loop
unsigned long lastSend;
const int elapsedTime = 2000; // tiempo transcurrido entre envios al servidor


//***************MODIFICAR PARA SU PROYECTO *********************
// configuración sensores que utilizan
int pinLDR = A0;
int pinLED = D0;
// Declarar e Inicializar sensores.



//
//************************* Funciones Setup y loop *********************
// función setup micro
void setup()
{
  Serial.begin(9600);

  // inicializar wifi y pubsus
  connectToWiFi();
  client.setServer( thingsboardServer, 1883 );

  // agregado para recibir callbacks
  client.setCallback(on_message);
   
  lastSend = 0; // para controlar cada cuanto tiempo se envian datos


  // ******** AGREGAR INICIALZICION DE SENSORES PARA SU PROYECTO *********
  pinMode(pinLED, OUTPUT);   // LED pin as output.
  delay(10);
}

// función loop micro
void loop()
{
  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > elapsedTime ) { // Update and send only after 1 seconds
    
    // FUNCION DE TELEMETRIA para enviar datos a thingsboard
    getAndSendTelemetryData();   // FUNCION QUE ENVIA INFORMACIÓN DE TELEMETRIA
    
    lastSend = millis();
  }

  client.loop();
}

//***************MODIFICAR PARA SU PROYECTO *********************
/*
 * función para leer datos de sensores y enviar telementria al servidor
 * Se debe sensar y armar el JSON especifico para su proyecto
 * Esta función es llamada desde el loop()
 */
void getAndSendTelemetryData()
{

  // Lectura de sensores 
  Serial.println("Collecting light data.");

  // Reading LDR 
  
  int rawLDR= analogRead(pinLDR);

  String valorLDR = String(rawLDR);


  // Preparar el payload del JSON payload, a modo de ejemplo el mensaje se arma utilizando la clase String. esto se puede hacer con
  // la biblioteca ArduinoJson (ver on message)
  // el formato es {key"value, Key: value, .....}
  // en este caso seria {"valorLDR": valor_leidoLDR}
  //
  String payload = "{";
  payload += "\"valorLDR\":"; payload += valorLDR; payload += "}";

  // Enviar el payload al servidor usando el topico para telemetria
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  if (client.publish( telemetryTopic, attributes ) == true)
    Serial.println("publicado ok");
  
  Serial.println( attributes );

}


//***************MODIFICAR PARA SU PROYECTO *********************
/* 
 *  Este callback se llama cuando se utilizan widgets de control que envian mensajes por el topico requestTopic
 *  Notar que en la función de reconnect se realiza la suscripción al topico de request
 *  
 *  El formato del string que llega es "v1/devices/me/rpc/request/{id}/Operación. donde operación es el método get declarado en el  
 *  widget que usan para mandar el comando e {id} es el indetificador del mensaje generado por el servidor
 */
void on_message(const char* topic, byte* payload, unsigned int length) 
{
  // Mostrar datos recibidos del servidor
  Serial.println("On message");

  char json[length + 1];
  strncpy (json, (char*)payload, length);
  json[length] = '\0';

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(json);

  // Decode JSON request
  // Notar que a modo de ejemplo este mensaje se arma utilizando la librería ArduinoJson en lugar de desarmar el string a "mano"
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject((char*)json);

  if (!data.success())
  {
    Serial.println("parseObject() failed");
    return;
  }

  // Obtener el nombre del método invocado, esto lo envia el switch de la puerta y el knob del motor que están en el dashboard
  String methodName = String((const char*)data["method"]);
  int action = data["params"];
  Serial.print("Nombre metodo:");
  Serial.print(methodName);
  Serial.print(" -> parámetro:");
  Serial.println(action);


  /* Responder segun el método 
   *  En este caso se envian comandos para prender, apgar o poner en dim el led
   */
  if (methodName.equals("lightON") || methodName.equals("lightDIM") || methodName.equals("lightOFF")) {
    // invocar metodo que procesa el comando
    setLightIntensity(action);

  }
  
 
}

//***************MODIFICAR PARA SU PROYECTO PARA PROCESAR UN COMANDO *********************

/*
 * función que "abre" la puerta (simulada).
 * En su proyecto habria que programar lo que hace el dispositivo al recibir el comando
 */
void setLightIntensity(int value)
{

  // aqui agregar las sentencias para procesar el comando enviado
  analogWrite(pinLED, value);
}




//***************NO MODIFICAR *********************
/*
 * funcion para reconectarse al servidor de thingsboard y suscribirse a los topicos de RPC y Atributos
 */
void reconnect() {
  int statusWifi = WL_IDLE_STATUS;
  // Loop until we're reconnected
  while (!client.connected()) {
    statusWifi = WiFi.status();
    connectToWiFi();
    
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect(NODE_NAME, NODE_TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
      
      // Suscribir al Topico de request
     client.subscribe(requestTopic); 
      
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

/*
 * función para conectarse a wifi
 */
void connectToWiFi()
{
  Serial.println("Connecting to WiFi ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}



