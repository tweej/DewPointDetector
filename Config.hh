#ifndef CONFIG_H
#define CONFIG_H

// STL
#include <string>
#include <vector>

class Config
{
public:
   explicit Config() = delete;
   explicit Config(const std::string &path);

   double                   getDewpointThreshold() const    { return _dewpointThreshold; }
   std::vector<std::string> getEmailRecipients() const      { return _emailRecipients; }
   std::string              getEmailSender() const          { return _emailSender; }
   std::string              getEmailSubject() const         { return _emailSubject; }
   unsigned short           getForecastDuration() const     { return _forecastDuration; }
   std::string              getWUndergroundAPIKey() const   { return _wUndergroundAPIKey; }
   std::string              getWUndergroundLocation() const { return _wUndergroundLocation; }

private:
   const std::string _path;

   double                    _dewpointThreshold;
   unsigned short            _forecastDuration;
   std::vector<std::string>  _emailRecipients;
   std::string               _emailSender;
   std::string               _emailSubject;
   std::string               _wUndergroundAPIKey;
   std::string               _wUndergroundLocation;
};

#endif // CONFIG_H
