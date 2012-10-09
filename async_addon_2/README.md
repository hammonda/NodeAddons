Example of how to build a node addon to call a C++ class member function asynchronously using the libeio thread pool.

## Build Instructions

Use `node-gyp`, the cross-platform command-line tool written in Node.js for compiling native addon modules for Node.js

      node-gyp configure
      node-gyp build

## Run Instructions

      node example.js

## Example Code

      var addon = require('./build/Release/async_addon_2');

      var obj = new addon.NthPrime();

      obj.getNthPrime(100000, function (n, answer, threadId) {
        console.log(n, answer, threadId);
      });

## Example Output

      $ node example.js 
      100000 1299709 16689984

