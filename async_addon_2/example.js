var addon = require('./build/Release/async_addon_2');

var obj = new addon.NthPrime();

obj.getNthPrime(100000, function (n, answer, threadId) {
  console.log(n, answer, threadId);
});
