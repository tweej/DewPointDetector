#ifndef WUNDERGROUNDINTERFACE_H
#define WUNDERGROUNDINTERFACE_H

#include "WeatherDataInterface.hh"

#include <string>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Poco/URI.h>

class WUndergroundInterface : public WeatherDataInterface
{
public:
   explicit WUndergroundInterface(const std::string & apiKey, const std::string & location) :
      WeatherDataInterface(apiKey, location) {}

private:
   std::vector<WeatherDataInterface::TemporalMeasurement<double>>
   doGetForecastedMinimumDailyDewpoints(
         const boost::posix_time::time_period   & period,
         const boost::posix_time::time_duration & timeout) override;

   std::string obtainJSON(
      const Poco::URI                        & uri,
      const boost::posix_time::time_duration & timeout) const;

private:
   static const std::string WU_API_URL;
};

#endif /* WUNDERGROUNDINTERFACE_HH_ */
