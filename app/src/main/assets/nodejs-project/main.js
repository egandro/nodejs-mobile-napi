var http = require('http');
var leftPad = require('left-pad');
var versions_server = http.createServer( (request, response) => {
  //response.end('Versions: ' + JSON.stringify(process.versions) + ' left-pad: ' + leftPad(42, 5, '0'));
  response.end('NAPI - helloWorld: ' + NativeBridge.helloWorld());
});
versions_server.listen(3000);
console.log('The node project has started.');

const NativeBridge = process._linkedBinding('sbgck_bridge');
console.log("NAPI - helloWorld", NativeBridge.helloWorld());
console.log("NAPI - addInts(2,5)", NativeBridge.addInts(2,5));
