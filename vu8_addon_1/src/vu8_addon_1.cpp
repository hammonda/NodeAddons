#include <vu8/Class.hpp>

#include <stdint.h>
#include <string.h>

#include "boost/date_time/gregorian/gregorian.hpp"

namespace greg = boost::gregorian;

// Add convertibility specializations for the boost:greg:date types.
namespace vu8 {
VU8_TO_V8_CONV_TYPE_TRAIT_SPEC(T,T,greg::date::day_of_year_type);
}

typedef vu8::Class<greg::date, vu8::Factory<int32_t,int32_t,int32_t> > DateClass;

void Init(v8::Handle<v8::Object> target) {
  // Define the date class constructor function.
  DateClass dateClass;

  // Add the member methods.
  dateClass
  .Set<greg::date::date_int_type(), &greg::date::julian_day>("julian_day")
  .Set<greg::date::day_of_year_type(), &greg::date::day_of_year>("day_of_year")
  .Set<greg::date::date_int_type(), &greg::date::modjulian_day>("modjulian_day")
  .Set<int32_t(), &greg::date::week_number>("week_number")
  .Set<greg::date::date_int_type(), &greg::date::day_number>("day_number")
  .Set<DateClass(), &greg::date::end_of_month>("end_of_month");

  // Add the constructor to the node export object for the module.
  target->Set(v8::String::New("Date"), 
      dateClass.JSFunctionTemplate()->GetFunction());
}

#define BUILDING_NODE_EXTENSION
#include <node.h>
NODE_MODULE(vu8_addon_1, Init)
