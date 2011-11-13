#include "curlcc.h"

namespace idctcp3
{

  class device_info
  {
  };

  class flow_entry
  {
  };

  class transfer_agent
  {
  public:
    transfer_agent(const std::string& base_url) : base_url(base_url) {}
    ~transfer_agent() {}

    // Login the device to the service
    void login(void);

    void get_time(void);

    void send_data(void);

    //HasUpdateBin - Sprawdza czy jest dostępna nowa wersja oprogramowania dla ICDTCP2
    //GetUpdateDataBin - Pobiera binaria dla updateu oporgramowania dla licznika ICDTCP2

    // Logout the device from the service
    void logout(void);

    //HelloWorld - testowa funkcja HelloWorld

    void test_session(void);

  protected:
    curlcc::easy request;
    std::string base_url;

    void init_request(void);
    std::string post_request(const std::string& url, const std::string& fields);

    static size_t data_arrived_cbf(char *ptr, size_t size, size_t nmemb, void *userdata);
  };
}

