#include "HTMLPages.hpp"

const char* html_setup_page = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <title>ESP32 Konfiguration</title>

        <style>
            body {
                font-family: Arial, sans-serif;
                text-align: center;
            }
            h2 {
                margin-bottom: 10px;
            }
            h3 {
                margin-top: 10px;
                margin-bottom: 0px;
            }
            p {
                font-size: 12px;
            }
            form {
                max-width: 420px;
                margin: auto;
                display: flex;
                flex-direction: column;
            }
            input {
                margin-top: 5px;
                font-size: 16px;
                padding: 10px;
            }
            button {
                margin-top: 20px;
                padding: 12px;
                font-size: 16px;
            }
            .checkbox-inline {
                display: inline-flex;
                align-items: center;
                justify-content: flex-start;
                gap: 6px; /* Abstand zwischen Box und Text */
                white-space: nowrap;      /* verhindert Umbruch */
                font-size: 14px;
                margin-top: 5px;
            }
        </style>

        <script>
            function toggle(id) {
            var f = document.getElementById(id);
            f.type = (f.type === "password") ? "text" : "password";
            }
        </script>
    </head>

    <body>

        <form action="/save" method="POST">

            <h3>WLAN</h3>
            <input name="ssid" placeholder="WLAN Name" required>
            <input id="wifi_pass" name="password" type="password" placeholder="WLAN Passwort">
            <div class="checkbox-inline">
                <input type="checkbox" onclick="toggle('wifi_pass')">
                Passwort anzeigen
            </div>

            <h3>MQTT Broker</h3>
            <input name="mqtt_server" placeholder="MQTT Server (IP oder Hostname)" required>
            <input name="mqtt_port" type="number" value="1883" placeholder="MQTT Port">
            <input name="mqtt_user" placeholder="MQTT Benutzer">
            <input id="mqtt_password" name="mqtt_pass" type="password" placeholder="MQTT Passwort">
            <div class="checkbox-inline">
                <input type="checkbox" onclick="toggle('mqtt_pass')">
                Passwort anzeigen
            </div>

            <h3>MQTT Gerätename & ID festlegen</h3>
            <input name="mqtt_device_name" placeholder="MQTT Gerätename" required>
            <input name="mqtt_device_id" placeholder="Eindeutige MQTT Geräte-ID">

            <p>Stelle vor dem Speichern sicher, dass alle Einstellungen korrekt sind und dass die Verbindung mit dem WLAN und MQTT-Broker hergestellt werden kann!</p>
            <button type="submit">Speichern</button>

        </form>

    </body>
</html>

)rawliteral";

const char* html_save_page = R"rawliteral(
  <html>
    <head>
        <meta charset='UTF-8'>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
            body { font-family: Arial, sans-serif; }
        </style>
    </head>
    <body>
        <h3>Gespeichert, Controller startet neu!</h3>
        <p>Sie können das Fenster schließen, der Controller versucht sich mit dem WLAN und dem MQTT-Server zu verbinden. Bei fehlgeschlagener Verbindung muss das Setup wiederholt werden.</p>
    </body>
    </html>
  )rawliteral";