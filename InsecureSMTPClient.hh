#ifndef INSECURESMTPCLIENT_H
#define INSECURESMTPCLIENT_H

#include <iostream>
#include <string>
#include <vector>

#include <Poco/Net/MailMessage.h>
#include <Poco/Net/MailRecipient.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/SMTPClientSession.h>

namespace InsecureSMTPClient
{
   void sendEmail(
      const std::string               &sender,
      const std::vector<std::string>  &recipients,
      const std::string               &subject,
      const std::string               &content)
   {

      Poco::Net::MailMessage message;
      message.setSender(sender);
      for(const std::string &recipient: recipients)
      {
         message.addRecipient(
            Poco::Net::MailRecipient(
               Poco::Net::MailRecipient::PRIMARY_RECIPIENT, recipient));
      }
      message.setSubject(subject);
      message.setContentType("text/plain; charset=UTF-8");
      message.setContent(content, Poco::Net::MailMessage::ENCODING_8BIT);

      try {
         // https://support.google.com/a/answer/176600?hl=en
         // Not that it is needed for this innocuous data, but SecureSMTPClientSession is supported
         // only by versions of POCO greater than what is available in the Ubuntu Xenial repository.
         Poco::Net::SMTPClientSession session("aspmx.l.google.com", 25);
         session.login();
         try {
               session.sendMessage(message);
               session.close();
         } catch (Poco::Net::SMTPException &e) {
            std::cerr << e.displayText() << std::endl;
            session.close();
         }
      } catch (Poco::Net::NetException &e) {
         std::cerr << e.displayText() << std::endl;
      }
   }
}

#endif /* INSECURESMTPCLIENT_H */