var function_addon_1 = require('./build/Release/function_addon_1');

var EventEmitter = require('events').EventEmitter;
var ee = new EventEmitter;
ee.on('work', work);

function callback (results) {
  process.nextTick(emitWork);
}

function work () {
  function_addon_1.getPrimes(10, 10, callback);
}

function emitWork () {
  ee.emit('work');
}

emitWork();
