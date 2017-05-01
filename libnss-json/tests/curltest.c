#include <curl/curl.h>

int main()
{
  CURL *curl;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
//  curl_easy_setopt(curl, CURLOPT_URL, "https://raw.githubusercontent.com/Aklakan/libnss-json/master/yoda.json");
  curl_easy_setopt(curl, CURLOPT_URL, "file:///home/raven/Projects/Eclipse/libnss-json/yoda.json");
  curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L); //0 disable messages

//  curl_easy_setopt(curl, CURLOPT_POST, 1);
//  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "foo=bar&foz=baz");
  curl_easy_perform(curl);
  curl_easy_cleanup(curl);
}


