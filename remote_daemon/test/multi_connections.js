var net = require('net');

var counter = 0;

var send = function(c, count) {
	var payload = {
		protocol_version : "1.0",
		message_type : "command",
		meta : {
			reply : false
		},
		data : {
			command : "input_event",
			parameters : {
				type : count,
				code : count,
				value : count
			}
		}
	};
	var payload_str = JSON.stringify(payload);
	var payload_len = payload_str.length;
	var message = payload_len + ":" + payload_str;
	console.log("send: ", message);
	c.write(message);
	c.end();
}

var daemon = function(count) {
	var client = net.createConnection(12306, '127.0.0.1');
	client.on('data', function(s) {
				console.log(s.toString())
			});

	client.on('connect', function(s) {
				console.log('connect to RemoteDaemon!!!');
				send(client, count);
			});
}

setInterval(function() {
			counter += 1;
			daemon(counter);
		}, 50);
