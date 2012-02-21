#include <iostream>
#include <list>

#include "transfer-agent.h"

extern "C" {
#include <roxml.h>
}

namespace idctcp3
{
  size_t transfer_agent::data_arrived_cbf(char *ptr, size_t size, size_t nmemb, void *userdata)
  {
    std::string *data = reinterpret_cast<std::string*>(userdata);
    *data += std::string(ptr, size * nmemb);
    return size * nmemb;
  }

  void transfer_agent::init_request(void)
  {
    request.init();

    request.setopt(curlcc::option_verbose(1));

    // Put empty accept header to remove it from the list
    std::list<std::string> post_header;
    post_header.push_back("Accept:");
    request.setopt(curlcc::option_httpheader(post_header));

    // Add user agent  header
    request.setopt(curlcc::option_useragent("idctcp3"));

    // Enable cookie support (to maintain session id)
    request.setopt(curlcc::option_cookiefile(""));
  }

  std::string transfer_agent::post_request(const std::string& url, const std::string& fields)
  {
    std::string data;

    request.setopt(curlcc::option_url(base_url + url));
    request.setopt(curlcc::option_copypostfields(fields));
    request.setopt(curlcc::option_writefunction(data_arrived_cbf));
    request.setopt(curlcc::option_writedata(&data));

    request.perform();

    return data;
  }

  void transfer_agent::login()
  {
    std::cerr << "transfer_agent::login_device" << std::endl;

    init_request();

    std::string url("icdtcpwebservice/Service1.asmx/LoginDevice");
    std::string fields("idd=g&name=abc&DevInfo=");

    std::string response = post_request(url, fields);

    std::cout << "+-+-+[" << response << "]+-+-+" << std::endl;

  
/*    std::cmatch m;
    std::regex e("(\\<\\?xml[^?>]*\\?\\>\\s*)(\\<string[^>]*\\>)([^<]*)(\\</string\\>\\s*)");
    if (std::regex_match(response.c_str(), m, e))
      std::cout << m[0] << std::endl;
    else
      std::cout << "std::regex returned false" << std::endl;
*/    

    node_t *root = roxml_load_buf((char*)response.c_str());

    int len = 0;
    char *s = roxml_get_content(root, NULL, 0, &len);

    std::cout << "+-+-+[" << s << "]+-+-+" << std::endl;
     
    roxml_release(s);
    roxml_close(root);
  }

  void transfer_agent::get_time(void)
  {
    std::cerr << "transfer_agent get_time" << std::endl;
    
    std::string url("icdtcpwebservice/Service1.asmx/GetTime");
    std::string response = post_request(url, std::string());

    std::cout << "+-+-+[" << response << "]+-+-+" << std::endl;
  }

  void transfer_agent::send_data(void)
  {
    std::cerr << "transfer_agent::send_data" << std::endl;
    
    std::string url("icdtcpwebservice/Service1.asmx/SendData");
    //dataczas;FotoNo;cnt;tick;tickDark;fototest;
    std::string response = post_request(url, "data=2011-11-06iT15:45:00;0;65;90000;900000;0;");

    std::cout << "+-+-+[" << response << "]+-+-+" << std::endl;
  }

  void transfer_agent::logout(void)
  {
    std::cerr << "transfer_agent""logout_device" << std::endl;
    
    std::string url("icdtcpwebservice/Service1.asmx/OutDevice");
    std::string fields("ErrorNo=0");
    std::string response = post_request(url, fields);

    std::cout << "+-+-+[" << response << "]+-+-+" << std::endl;

    request.cleanup();
  }

  void transfer_agent::test_session(void)
  {
    std::cerr << "transfer_agent::test_session" << std::endl;
    
    std::string url("icdtcpwebservice/Service1.asmx/TestSession");
    std::string response = post_request(url, std::string());

    std::cout << "+-+-+[" << response << "]+-+-+" << std::endl;
  }
}

