// npm install mqtt --save

var mqtt = require('mqtt');
var http = require('http');
const fs = require('fs')

const daysEN = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
const monthsEN = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"];
const daysCN = ["星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"];

var variables = [];

function updateDateTime() {
    var d = new Date(new Date().toLocaleString("en-US", { timeZone: "Europe/Berlin" }));

    var s = daysEN[d.getDay()] + ", " + monthsEN[d.getMonth()] + " " + d.getDate() + ", " + d.getFullYear();
    variables["date"] = s;

    var s = d.getFullYear() + "年" + (d.getMonth() + 1) + "月" + d.getDate() + "日  " + daysCN[d.getDay()];
    variables["dateCN"] = s;

    var s = d.getHours().toString().padStart(2, "0") + ":" + d.getMinutes().toString().padStart(2, "0")
    variables["time"] = s;

}

var mqttBroker = process.env.MQTT_URL || 'mqtt://mqtt.example.com:1883';
var mqttUsername = process.env.MQTT_USER || '';
var mqttPassword = process.env.MQTT_PASS || '';

var client = mqtt.connect(mqttBroker, {
    username: mqttUsername,
    password: mqttPassword
});

client.on('connect', function () {
    console.log('Connected to broker');
    client.subscribe('location/f41/#', function (err) {
        if (err) {
            console.log('Error subscribing: ' + err.toString());
        }
    });
});

client.on('reconnect', function () {
    console.log('Reconnected to broker');
    client.subscribe('location/f41/#', function (err) {
        if (err) {
            console.log('Error subscribing: ' + err.toString());
        }
    });
});

client.on('error', function (err) {
    console.log('Error connecting to broker: ' + err.toString());
});

client.on('message', function (topic, message) {
    // message is Buffer
    console.log(topic + ', ' + message.length.toString() + ' bytes');
    variables[topic] = message;
    //    client.end();
});

http.createServer(function (req, res) {
    fs.readFile('/data/display-template.html', 'utf8', (err, data) => {
        if (err) {
            console.error(err);
            res.writeHead(500, { 'Content-Type': 'text/plain' });
            res.write(err.toString())
            res.end();
            return
        }

        updateDateTime();
        data = data.replace(/{{([^}]+)}}/g, function (str, p1, offset, s) {
//            console.log("Replacing >" + p1 + "< with >" + variables[p1] + "<");
            return variables[p1] || p1;
        });

        console.log("Sending...")
        res.writeHead(200, { 'Content-Type': 'text/html' });
        res.write(data);
        res.end();
    })
}).listen(80);