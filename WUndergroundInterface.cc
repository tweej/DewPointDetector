#include "WUndergroundInterface.hh"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/trivial.hpp>

// STL
#include <map>
#include <vector>

// POCO
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NetException.h>
// #include <Poco/StreamCopier.h>

// JsonCPP
#include <json/json.h>

const std::string WUndergroundInterface::WU_API_URL = "http://api.wunderground.com/api/";


/* API Request Limits: TODO
 * Calls per day: 500
 * Calls per minute: 10
 */
std::string WUndergroundInterface::obtainJSON(
   const Poco::URI                         & uri,
   const boost::posix_time::time_duration  & timeout) const
{
   // prepare path
   std::string path(uri.getPathAndQuery());
   if (path.empty()) path = "/";

   // prepare request and response
   Poco::Net::HTTPRequest req(
      Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);

   try
   {
      // prepare session
      Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
      if( !timeout.is_pos_infinity() )
      {
         session.setTimeout(Poco::Timespan(timeout.total_seconds(), 0));
      }

      session.sendRequest(req);

      // Get reponse
      Poco::Net::HTTPResponse res;
      if( res.getStatus() != Poco::Net::HTTPResponse::HTTP_OK )
      {
         BOOST_LOG_TRIVIAL(trace) << __func__ << " - "
            << res.getStatus() << " " << res.getReason();

         throw WeatherDataInterface::NoData();
      }

      // Poco::StreamCopier::copyToString(session.receiveResponse(res), payload);
      // This is uglier than above, but it appears to be about 40% faster
      std::istreambuf_iterator<char> eos;
      return(std::string(std::istreambuf_iterator<char>(session.receiveResponse(res)), eos));
   }
   catch(const Poco::TimeoutException &e)
   {
      BOOST_LOG_TRIVIAL(warning) << __func__ << " - "
         << "Poco::TimeoutException - " << e.displayText();

      throw WeatherDataInterface::TimeoutException();
   }
   catch(const Poco::Net::NoMessageException &e)
   {
      BOOST_LOG_TRIVIAL(warning) << __func__ << " - "
         << "Poco::NoMessageException - " << e.displayText();

      throw WeatherDataInterface::NoData();
   }
}


std::vector<WeatherDataInterface::TemporalMeasurement<double>>
WUndergroundInterface::doGetForecastedMinimumDailyDewpoints(
   const boost::posix_time::time_period   & period,
   const boost::posix_time::time_duration & timeout)
{
   static const Poco::URI uri(WU_API_URL + apiKey + "/hourly10day/q/" + location);

   BOOST_LOG_TRIVIAL(trace) << __func__ << " - " << "About to make HTTP request";

   const std::string json = obtainJSON(uri, timeout);

   BOOST_LOG_TRIVIAL(trace) << __func__ << " - " << "Got HTTP response";

   BOOST_LOG_TRIVIAL(trace) << __func__ << " - "
      << "About to parse URI (" << period.begin() << "," << period.last() << "): "
      << uri.toString();

   Json::Value  root;
   Json::Reader reader;

   bool parsingSuccessful = reader.parse(json, root);
   if( !parsingSuccessful )
   {
      // report the location of the failure in the document to the user
      BOOST_LOG_TRIVIAL(warning) << __func__ << " - "
         << "Failed to parse configuration\n" << reader.getFormattedErrorMessages();

      throw WeatherDataInterface::NoData("Failed to parse JSON data");
   }


   std::vector<WeatherDataInterface::TemporalMeasurement<double>> results;

   const Json::Value array = root["hourly_forecast"];
   for(unsigned int i=0; i<array.size(); ++i)
   {
      // The JSON fields provided by Weather Underground (annoyingly) must be interpreted as strings
      const boost::gregorian::date day(
         std::stoul(array[i]["FCTTIME"]["year"].asString()),
         std::stoul(array[i]["FCTTIME"]["mon"].asString()),
         std::stoul(array[i]["FCTTIME"]["mday"].asString()));

      const boost::posix_time::time_duration time(
         std::stoul(array[i]["FCTTIME"]["hour"].asString()),
         std::stoul(array[i]["FCTTIME"]["min"].asString()),
         std::stoul(array[i]["FCTTIME"]["sec"].asString()),
         0);

      const boost::posix_time::ptime dataElementTime(day, time);

      // data elements are in increasing chronological order
      if( dataElementTime < period.begin() )
         continue;
      else if( dataElementTime > period.last() )
         break;

      const double dewpoint = std::stod(array[i]["dewpoint"]["english"].asString());
      if( results.empty() || results.back().time.date() != day )
      {
         results.emplace_back(
            WeatherDataInterface::TemporalMeasurement<double>{dataElementTime, dewpoint});
      }
      else if( dewpoint < results.back().measurement )
      {
         results.back().time        = dataElementTime;
         results.back().measurement = dewpoint;
      }
   }

   BOOST_LOG_TRIVIAL(trace) << __func__ << " - "
         << "Analyzed whole response at " << boost::posix_time::microsec_clock::local_time();

   return(results);
}


