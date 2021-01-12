var http = require('http');

var options = {
  host: '192.168.1.189',
  path: '/EPD',
  method: 'POST',
  port: 80,
  timeout: 20000,
  protocol: 'http:'
};

callback = function(response) {
  var str = ''
  response.on('data', function (chunk) {
    str += chunk;
  });

  response.on('end', function () {
    console.log(str);
  });
}

var req = http.request(options, callback);
//This is the data we are posting, it needs to be a string or a buffer
req.write('ib');
req.end();