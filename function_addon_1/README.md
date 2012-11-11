Example of how to build a node addon to call a C++ function asynchronously using the libeio thread pool and return a local JavaScript object.

## Build Instructions

Use `node-gyp`, the cross-platform command-line tool written in Node.js for compiling native addon modules for Node.js

      node-gyp configure
      node-gyp build

## Run Instructions

      node example.js

## Example Code

      var function_addon_1 = require('./build/Release/function_addon_1');

      function callback (results) { 
        console.log(JSON.stringify(results, null, ' '));
      }

      function_addon_1.getPrimes(100000, 100005, callback);
      function_addon_1.getPrimes(1000, 1005, callback);
      function_addon_1.getPrimes(100, 105, callback);

      console.log('Finished calls');

## Example Output

      $ node example.js 
      Finished calls
      {
       "start": 100,
       "size": 6,
       "primes": [
        541,
        547,
        557,
        563,
        569,
        571
       ]
      }
      {
       "start": 1000,
       "size": 6,
       "primes": [
        7919,
        7927,
        7933,
        7937,
        7949,
        7951
       ]
      }
      {
       "start": 100000,
       "size": 6,
       "primes": [
        1299709,
        1299721,
        1299743,
        1299763,
        1299791,
        1299811
       ]
      }
