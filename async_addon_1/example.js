var async = require('./build/Release/async_addon_1');

function callback (n, answer, threadId) { 
  console.log(n+'th Prime =', answer, 'calculated by thread', threadId); 
}

for (var i = 0; i < 200; i++) {
  if (!async.nthPrime(100000 + i, callback)) {
    throw "async.nthPrime error";
  }
}

console.log('Finished async.nthPrime calls');
