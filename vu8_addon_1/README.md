Example of how to use vu8 to wrap a C++ class for use in nodejs.

## Build Instructions

Use `node-gyp`, the cross-platform command-line tool written in Node.js for compiling native addon modules for Node.js

	vu8_addon_1$ git clone https://github.com/hammonda/vu8.git deps/vu8
	vu8_addon_1$ node-gyp configure
	vu8_addon_1$ node-gyp build

## Run Instructions

	vu8_addon_1$ node example.js

## Example Code

	var addon = require('./build/Release/vu8_addon_1');

	var date = new addon.Date(2012, 10, 21);

	console.log('julian_day:', date.julian_day());
	console.log('day_of_year:', date.day_of_year());
	console.log('modjulian_day:', date.modjulian_day());
	console.log('week_number:', date.week_number());
	console.log('day_number:', date.day_number());

	var eom = date.end_of_month();
	console.log('end_of_month:', eom.day_number());

## Example Output

	julian_day: 2456222
	day_of_year: 295
	modjulian_day: 56221
	week_number: 42
	day_number: 2456222
	end_of_month: 2456232

