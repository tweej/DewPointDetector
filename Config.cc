#include "Config.hh"

#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>

Config::Config(const std::string &path) : _path(path)
{
   using namespace boost::property_tree;
   ptree pt;

   xml_parser::read_xml(path, pt); // will throw on badness

   _dewpointThreshold    = pt.get<double>("config.DewPointThreshold");
   _emailSender          = pt.get<std::string>("config.EmailSender");
   _emailSubject         = pt.get<std::string>("config.EmailSubject");
   _forecastDuration     = pt.get<unsigned short>("config.ForecastDuration");
   _wUndergroundAPIKey   = pt.get<std::string>("config.WUndergroundAPIKey");
   _wUndergroundLocation = pt.get<std::string>("config.WUndergroundLocation");

   BOOST_FOREACH( ptree::value_type const& v, pt.get_child("config.EmailRecipients") )
      _emailRecipients.push_back(v.second.data());
}
