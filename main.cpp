#include "Config.hh"
#include "InsecureSMTPClient.hh"
#include "WUndergroundInterface.hh"

#include <cstdlib>
#include <sstream>

#include <boost/date_time/posix_time/posix_time_io.hpp>


int main(int argc, char **argv) {

   if( argc < 2 )
   {
      std::cerr << "Usage: " << argv[0] << " <XML configuration file>" << std::endl;
      return EXIT_FAILURE;
   }

   Config config(argv[1]);

   WUndergroundInterface weatherDataInterface(
      config.getWUndergroundAPIKey(), config.getWUndergroundLocation());

   std::vector<WeatherDataInterface::TemporalMeasurement<double>> minimums =
      weatherDataInterface.getForecastedMinimumDailyDewpoints(
         boost::posix_time::time_period(
            boost::posix_time::second_clock::local_time(),
            boost::posix_time::second_clock::local_time() +
               boost::gregorian::days(config.getForecastDuration())));

   std::stringstream sstr;
   for(const auto &e : minimums)
      if( e.measurement <= config.getDewpointThreshold() )
         sstr << e.time << " " << e.measurement << std::endl;

   if(!sstr.str().empty())
   {
      InsecureSMTPClient::sendEmail(
         config.getEmailSender(),
         config.getEmailRecipients(),
         config.getEmailSubject(),
         sstr.str());
   }

   return EXIT_SUCCESS;
}
