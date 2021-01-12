// npm install mqtt --save

const verbose = false;

var mqtt = require('mqtt');
var http = require('http');

var httpOptions = {
    host: '192.168.1.189',
    ahost: 'web42.com',
    path: '/',
    port: '80',
    method: 'POST',
    headers: {
        'Content-Type': 'text/plain'
    }
};

var client = mqtt.connect('mqtt://samson.web42.com');

var pxInd = 0, stInd = 0;
var rqMsg = 0;
var prvPx = 0, prvSt = 0;
var epdInd = 24;

var a;

var updating = false;

function ldPrv(response) { if (response.statusCode != 200) { pxInd = prvPx; stInd = prvSt; } }

var httpCallback = function (response) {
    var str = '';
    response.on('data', function (chunk) {
        str += chunk;
    });

    response.on('end', function () {
        if (verbose) {
            console.log(response.statusCode + ' ' + response.statusMessage);
            console.log('>>>\n' + str + '<<<\n');
        }
        ldPrv(response);

        if (stInd == 0) return u_dataA(a, (epdInd == 1) || (epdInd == 12) ? -1 : 0, 0, 50);
        if (stInd == 1) return u_next();
        if (stInd == 2) return u_dataA(a, 3, 50, 50);
        if (stInd == 3) return u_done();
        updating = false;
    });
};

function svPrv() { prvPx = pxInd; prvSt = stInd; }
function byteToStr(v) { return String.fromCharCode((v & 0xF) + 97, ((v >> 4) & 0xF) + 97); }
function wordToStr(v) { return byteToStr(v & 0xFF) + byteToStr((v >> 8) & 0xFF); }

function sendPostRequest(path, data) {
    httpOptions.path = path;
    httpOptions.headers['Content-Length'] = data.length;

    /*
    const req = http.request(httpOptions, res => {
        console.log(`statusCode: ${res.statusCode}`)
    
        res.on('data', d => {
            process.stdout.write(d)
        })
    })
    */

    var req = http.request(httpOptions, httpCallback);
    req.on('error', function (e) {
        console.log('HTTP Error: ' + e);
        updating = false;
    });
    req.on('timeout', function (e) {
        console.log('HTTP Timout: ' + e);
        updating = false;
    });
    req.on('uncaughtException', function (e) {
        console.log('HTTP UncaughtException: ' + e.toString);
        updating = false;
    });

    req.write(data);
    req.end();
}

function u_send(cmd, next) {
    if (verbose) console.log('send()');
    sendPostRequest('/' + cmd, '');
    if (next) stInd++;
    return 0;
}

function u_next() {
    if (verbose) console.log('u_next()');
    pxInd = 0;
    u_send('NEXT', true);
}
function u_done() {
    if (verbose) console.log('u_done()');
    if (verbose) console.log('Complete!');
    updating = false;
    return u_send('SHOW', true);
}
function u_loadA(a, k1, k2) {
    if (verbose) console.log('u_loadA()');
    var x = '' + (k1 + k2 * pxInd / a.length);
    if (x.length > 5) x = x.substring(0, 5);
    if (verbose) console.log('Progress: ' + x + '%');

    sendPostRequest('/LOAD', rqMsg + wordToStr(rqMsg.length) + 'LOAD');
    if (pxInd >= a.length) stInd++;
    return 0;
}

function u_dataA(a, c, k1, k2) {
    if (verbose) console.log('u_dataA()');
    rqMsg = '';
    svPrv();
    while ((pxInd < a.length) && (rqMsg.length < 1500)) {
        var v = 0;
        for (var i = 0; i < 8; i++) {
            if ((pxInd < a.length) && (a[pxInd] != c)) v |= (128 >> i);
            pxInd++;
        }
        rqMsg += byteToStr(v);
    }
    return u_loadA(a, k1, k2);
}

function uploadImage() {
    console.log('uploadImage()');
    updating = true;
    pxInd = 0;
    stInd = 0;

    var epdInd = 24;

    sendPostRequest('/EPD', byteToStr(epdInd));
    return 0;
}



client.on('connect', function () {
    client.subscribe('/display/epd-f3d111/all', function (err) {
        if (err) {
            console.log('Error subscribing: ' + err.toString);
        }
    });
});

client.on('message', function (topic, message) {
    // message is Buffer
    console.log(topic + ', ' + message.length.toString() + ' bytes');
    if (!updating) {
        a = Array.from(message);
        uploadImage();
        //		uploadImage(880, 528);
    }
//    client.end();
});