// npm install mqtt --save
// npm install follow-redirects --save

const { exec } = require('child_process');
var mqtt = require('mqtt');
var http = require('follow-redirects').http;
var client = mqtt.connect('mqtt://samson.web42.com');

var updateFrequency = 5 * 60;

client.on('connect', function () {
    client.subscribe('/config/display/update-frequency', function (err) {
        if (err) {
            console.log('Error subscribing: ' + err.toString);
        }
    });
});

client.on('message', function (topic, message) {
    // message is Buffer
    console.log(topic + ', ' + message.length.toString() + ' bytes');
    var newUF = parseInt(message);
    if (newUF > 0) {
        console.log("Update frequency now set to " + newUF);
        updateFrequency = newUF;
    }
});

http.createServer(function (req, res) {
    res.writeHead(200, { 'Content-Type': 'text/html' });
    var data = getImagePPM();
    //        res.write('Forcing update');
    res.write("Retrieving...");
    res.end();
}).listen(80);

var httpOptions = {
    hostname: 'samson.web42.com',
    port: 7000,
    path: '/?link=http://samson.web42.com:2000&extension=ppm&wh=880,528',
    method: 'GET',
    headers: {
        'Content-Type': 'text/plain',
        'Content-Length': 0,
    }
};

var data = '';

function getImagePPM() {
    console.log("Retrieving up-to-date image");

    /*
    exec('curl -L -o - "http://samson.web42.com:7000/?link=http://samson.web42.com:2000&extension=ppm&wh=880,528"',
        { maxBuffer: 1024 * 500 }, (error, stdout, stderr) => {
            if (error) {
                console.error(`exec error: ${error}`);
                return;
            }
            console.log(`stdout: ${stdout}`);
            console.error(`stderr: ${stderr}`);
        });
    */
    const req = http.request(httpOptions, res => {
        console.log(`statusCode: ${res.statusCode}`);
        res.on('data', d => {
            data += d;
        });
        res.on('end', () => {
            console.log("received:>>>" + data.length + "<<<");
            processImagePPM();
        });
    });

    req.on('error', error => {
        console.error(error);
    });

    req.end();

}

function processImagePPM() {
    const width = 880;
    const height = 528;

    var inputData = data;
    var outputA = Buffer.alloc(inputData.length / 3);

    console.log("image processing");

    for (var i = 0; i < inputData.length; i += 3) {
        var r = inputData[i] / 255.0;
        var g = inputData[i + 1] / 255.0;
        var b = inputData[i + 2] / 255.0;

        var cmax = Math.max(r, Math.max(g, b));
        var cmin = Math.min(r, Math.min(g, b));
        var diff = cmax - cmin;
        var h = -1;
        var s = -1;

        if (cmax == cmin) {
            h = 0;
        } else if (cmax == r) {
            h = (60 * ((g - b) / diff) + 360) % 360;
        } else if (cmax == g) {
            h = (60 * ((b - r) / diff) + 120) % 360;
        } else if (cmax == b) {
            h = (60 * ((r - g) / diff) + 240) % 360;
        }

        if (cmax == 0) {
            s = 0;
        } else {
            s = (diff / cmax) * 100;
        }

        v = cmax * 100;

        //    msg.inputHSV[i] = h;
        //    msg.inputHSV[i + 1] = s;
        //    msg.inputHSV[i + 2] = v;


        if (v < 20) {
            // Black = HSV(0, 0%, 0%)
            //        msg.outputB[i / 3 / 8] |= 2**(i % (3 * 8));
            outputA[i / 3] = 0;
        } else if (s > 80) {
            // Red = HSV(0, 100%, 100%)
            //        msg.outputR[i / 3 / 8] |= 2**(i % (3 * 8));
            outputA[i / 3] = 3;
        } else {
            // White = HSV(0, 0%, 100%)
            outputA[i / 3] = 1;
        }
    }
    var options={
        retain:true,
        qos:1};

    if (client.connected == true) {
        console.log("mqtt publish /display/epd-f3d111/all");
        client.publish('/display/epd-f3d111/all', outputA, options);
    }
}