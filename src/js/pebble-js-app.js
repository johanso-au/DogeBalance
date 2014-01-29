var WalletAddress = 'DMry4Ch2FDFGud7WCn92cXgP1M7qeiMGJm';

var maxTriesForSendingAppMessage = 3;
var timeoutForAppMessageRetry = 3000;
var timeoutForAPIRequest = 12000;

function sendAppMessage(message, numTries, transactionId) {
	numTries = numTries || 0;
	if (numTries < maxTriesForSendingAppMessage) {
		numTries++;
		console.log('Sending AppMessage to Pebble: ' + JSON.stringify(message));
		Pebble.sendAppMessage(
			message, function() {}, function(e) {
				console.log('Failed sending AppMessage for transactionId:' + e.data.transactionId + '. Error: ' + e.data.error.message);
				setTimeout(function() {
					sendAppMessage(message, numTries, e.data.transactionId);
				}, timeoutForAppMessageRetry);
			}
		);
	} else {
		console.log('Failed sending AppMessage for transactionId:' + transactionId + '. Bailing. ' + JSON.stringify(message));
	}
}

function makeRequest() {
	var xhr = new XMLHttpRequest();
	xhr.open('GET', 'http://dogechain.info/chain/Dogecoin/q/addressbalance/' + WalletAddress, true);
	xhr.timeout = timeoutForAPIRequest;
	xhr.onload = function(e) {
		if (xhr.readyState == 4) {
			if (xhr.status == 200) {
				var res = parseInt(xhr.responseText).toString();
				console.log('Res now equals: ' + res);
				sendAppMessage({
						'balance': res,
				});
			} else {
				console.log('Request returned error code ' + xhr.status.toString());
				sendAppMessage({'item_name': 'Error: ' + xhr.statusText});
			}
		}
	}
	xhr.ontimeout = function() {
		console.log('Error: request timed out!');
		sendAppMessage({'item_name': 'Error: Request timed out!'});
	};
	xhr.onerror = function(e) {
		console.log(JSON.stringify(e));
		sendAppMessage({'item_name': 'Error: Failed to connect!'});
	};
	xhr.send(null);
}

Pebble.addEventListener('ready', function(e) {});

Pebble.addEventListener('appmessage', function(e) {
	console.log('AppMessage received from Pebble: ' + JSON.stringify(e.payload));

	makeRequest();
});