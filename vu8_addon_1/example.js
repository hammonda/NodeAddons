var addon = require('./build/Release/vu8_addon_1');

var date = new addon.Date(2012, 10, 21);

console.log('julian_day:', date.julian_day());
console.log('day_of_year:', date.day_of_year());
console.log('modjulian_day:', date.modjulian_day());
console.log('week_number:', date.week_number());
console.log('day_number:', date.day_number());

var eom = date.end_of_month();
console.log('end_of_month:', eom.day_number());
