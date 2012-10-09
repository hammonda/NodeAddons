Example of how to build a node addon to call a C++ function asynchronously using the libeio thread pool.

## Build Instructions

Use `node-gyp`, the cross-platform command-line tool written in Node.js for compiling native addon modules for Node.js

      node-gyp configure
      node-gyp build

## Run Instructions

      node example.js

## Example Code

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

## Example Output

      $ node example.js 
      Finished async.nthPrime calls
      100000th Prime = 1299709 calculated by thread 1383232
      100003th Prime = 1299763 calculated by thread 2947904
      100001th Prime = 1299721 calculated by thread 3984192
      100002th Prime = 1299743 calculated by thread 1649472
      100004th Prime = 1299791 calculated by thread 1383232

