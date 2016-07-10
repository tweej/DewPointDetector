#ifndef WEATHERDATAINTERFACE_H
#define WEATHERDATAINTERFACE_H

#include <stdexcept>
#include <string>
#include <vector>

#include <boost/date_time/posix_time/posix_time_types.hpp>


class WeatherDataInterface
{
public:

   explicit WeatherDataInterface() = delete;
   explicit WeatherDataInterface(const std::string &apiKey, const std::string &location) :
      apiKey(apiKey), location(location) {}

   /* Weather data providers do not all implement the same API, data formats, or data fields for the
    *  data that they provide. So instead of simply returning a container of data, this class
    *  declares functions which evaluate specific tasks that may be carried out different ways by
    *  derived classes for each different weather data provider.
    */


   template<typename Measurement>
   struct TemporalMeasurement
   {
      boost::posix_time::ptime  time;
      Measurement               measurement;
   };


   /* Get the minimum daily dewpoints from the forecast
    *
    * An attempt will be made to obtain the forecast for the period provided. The minimum dew point
    * for each day within this period will be returned from the function.
    *
    * Exception Information:
    *   If [timeout] is exceeded while waiting for data from the weather data source the function
    *    will throw a TimeoutException.
    *   If complete, valid data cannot be obtained and parsed, the function will throw a NoData
    *    exception.
    *   If a request would exceed the API limits of the data source, the function will throw
    *    RateLimitException. // TODO API Limits
    *   If weather data cannot be interpreted, the function will throw a NoData exception.
    *   If [period] is not "well formed", the function will throw std::invalid_argument.
    *   If either [beginTime] or [endTime] is before the current local time, the function will throw
    *    std::invalid_argument.
    *   If either [beginTime] or [endTime] is in the future by more than 10 days, the function will
    *    throw std::invalid_argument.
    *
    * Parameters:
    *   period  - The period of (future) time during which forecast data will be analyzed.
    *   timeout - The maximum amount of time to wait for data from the weather data source.
    */
   std::vector<TemporalMeasurement<double>> getForecastedMinimumDailyDewpoints(
      const boost::posix_time::time_period   & period,
      const boost::posix_time::time_duration & timeout = boost::posix_time::seconds(10));





   class TimeoutException : public std::runtime_error {
   public:
      explicit TimeoutException()                       : std::runtime_error("Timeout Exception") {}
      explicit TimeoutException(const std::string &arg) : std::runtime_error(arg)                 {}
   };

   class NoData : public std::runtime_error {
   public:
      explicit NoData() :
         std::runtime_error("Unable to obtain weather data") {};
      explicit NoData(const std::string &arg) : std::runtime_error(arg) {}
   };

   class RateLimitException : public std::runtime_error {
   public:
      explicit RateLimitException() :
         std::runtime_error("Rate limit exception") {}
      explicit RateLimitException(const std::string &arg) : std::runtime_error(arg) {}
   };

protected:
   // Prevent the deletion of an instance through a base class pointer
   virtual ~WeatherDataInterface() = default;

   // Derived classes need to know the API key and location to make weather data requests
   const std::string apiKey;
   const std::string location;

private:

   virtual std::vector<TemporalMeasurement<double>> doGetForecastedMinimumDailyDewpoints(
      const boost::posix_time::time_period   & period,
      const boost::posix_time::time_duration & timeout) = 0;
};

#endif /* WEATHERDATAINTERFACE_H */
