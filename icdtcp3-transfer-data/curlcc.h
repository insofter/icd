#include <string>
#include <list>
#include <curl/curl.h>

#ifndef __CURLCC_H__
#define __CURLCC_H__ 

namespace curlcc
{
  class base_option
  {
    friend class easy;

    protected:
      virtual void setopt(CURL *handle) const = 0;
  };

  template<class T, CURLoption OPT>
  class option : public base_option
  {
    public:
      option(const T& value) : value(value) {}
      ~option() {}

    protected:
      T value;
      
      virtual void setopt(CURL *handle) const
      {
        curl_easy_setopt(handle, OPT, value); 
      }
  };

  template<CURLoption OPT>
  class option<std::string, OPT> : public base_option
  {
    public:
      option(const std::string& value) : value(value) {}
      ~option() {}

    protected:
      std::string value;
      
      virtual void setopt(CURL *handle) const
      {
        curl_easy_setopt(handle, OPT, value.c_str()); 
      }
  };

  template<CURLoption OPT>
  class option<std::list<std::string>, OPT> : public base_option
  {
    public:
      option(const std::list<std::string>& value) : value(value) {}
      ~option() {}

    protected:
      std::list<std::string> value;
      
      virtual void setopt(CURL *handle) const
      {
        struct curl_slist *headers = NULL;
        std::list<std::string>::const_iterator i;
        for (i = value.begin(); i != value.end(); i++)
          headers = curl_slist_append(headers, (*i).c_str());
        
        curl_easy_setopt(handle, OPT, headers);
      }
  };
  
  typedef size_t (*writefunction_t)(char *ptr, size_t size, size_t nmemb, void *userdata);

  typedef class option<long, CURLOPT_VERBOSE> option_verbose;
  typedef class option<std::string, CURLOPT_URL> option_url;
  typedef class option<writefunction_t, CURLOPT_WRITEFUNCTION> option_writefunction;
  typedef class option<void *, CURLOPT_WRITEDATA> option_writedata;
  typedef class option<std::string, CURLOPT_POSTFIELDS> option_postfields;
  typedef class option<std::string, CURLOPT_COPYPOSTFIELDS> option_copypostfields;
  typedef class option<std::list<std::string>, CURLOPT_HTTPHEADER> option_httpheader;
  typedef class option<std::string, CURLOPT_USERAGENT> option_useragent;
  typedef class option<std::string, CURLOPT_COOKIEFILE> option_cookiefile;

  class easy
  {
    public:
      easy() : handle(NULL) {}

      ~easy()
      {
        if (handle != NULL)
          cleanup();
      }
      
      void init(void)
      {
        handle = curl_easy_init();
        //if (handle == NULL)
          //throw ex;
      }

      void cleanup(void)
      {
        if (handle == NULL)
          return;

        curl_easy_cleanup(handle);
        handle = NULL;
      }

      void setopt(const base_option& opt)
      {
        opt.setopt(handle);
      }

      void perform(void)
      {
        /*CURLcode code =*/ curl_easy_perform(handle);
        //if (code != CURLE_OK)
        //  throw ex;
      }

    private:
      CURL *handle;
  };
}

#endif // __CURLCC_H__

