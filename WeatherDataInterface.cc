#include "WeatherDataInterface.hh"

std::vector<WeatherDataInterface::TemporalMeasurement<double>>
WeatherDataInterface::getForecastedMinimumDailyDewpoints(
      const boost::posix_time::time_period   & period,
      const boost::posix_time::time_duration & timeout)
{
   const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

   if(period.is_null())
      throw std::invalid_argument("time_period is not well formed");

   if( period.begin().is_special() )
      throw std::invalid_argument("beginTime is a special_value");
   if( period.last().is_special() )
      throw std::invalid_argument("endTime is a special_value");

   if( period.begin() < now )
      throw std::invalid_argument("beginTime is before current time");
   if( period.last() < now )
      throw std::invalid_argument("endTime is before current time");

   if( period.begin() > now + boost::gregorian::days(10) )
      throw std::invalid_argument("beginTime is greater than 10 days in the future");
   if( period.last() > now + boost::gregorian::days(10) )
      throw std::invalid_argument("endTime is greater than 10 days in the future");

   if( timeout.is_not_a_date_time() || timeout.is_neg_infinity() )
      throw std::invalid_argument("timeout can only have special_value pos_infin");

   return(doGetForecastedMinimumDailyDewpoints(period, timeout));
}
