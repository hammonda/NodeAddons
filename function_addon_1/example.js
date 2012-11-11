var function_addon_1 = require('./build/Release/function_addon_1');

function callback (results) { 
  console.log(JSON.stringify(results, null, ' '));
}

function_addon_1.getPrimes(100000, 100005, callback);
function_addon_1.getPrimes(1000, 1005, callback);
function_addon_1.getPrimes(100, 105, callback);

console.log('Finished calls');
