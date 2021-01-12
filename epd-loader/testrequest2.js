const http = require('http');

var httpOptions = {
	hostname: '192.168.1.189',
	path: '/EPD',
	method: 'POST',
	headers: {
		'Content-Type': 'text/plain',
		'Content-Length': 2,
	}
};


const req = http.request(httpOptions, res => {
	console.log(`statusCode: ${res.statusCode}`);

	res.on('data', d => {
		process.stdout.write(d);
	});
});

req.on('error', error => {
	console.error(error);
});

req.write('ib');
req.end();
