#include <iostream>
#include <string>
#include <curl/curl.h>

std::string execute_request(const std::string &url, const std::string &access_token)
{
    CURL *curl;
    CURLcode res;
    std::string response_data;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](void *ptr, size_t size, size_t nmemb, void *data) -> size_t
                         {
                             ((std::string *)data)->append((char *)ptr, size * nmemb);
                             return size * nmemb; });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return response_data;
}

std::string place_order(const std::string &access_token, const std::string &instrument_name, int amount, double price)
{
    std::string url = "https://test.deribit.com/api/v2/private/buy?instrument_name=" + instrument_name +
                      "&amount=" + std::to_string(amount) + "&price=" + std::to_string(price);

    return execute_request(url, access_token);
}

std::string get_order(std::string &access_token)
{
    std::string url = "https://test.deribit.com/api/v2/private/get_open_orders";
    return execute_request(url, access_token);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <action> [args...]\n";
        std::cerr << "Actions:\n";
        std::cerr << "  place <instrument_name> <amount> <price>\n";
        std::cerr << "  cancel <order_id>\n";
        return 1;
    }
    std::string action = argv[1];
    std::string access_token = "1737032586071.1f9188O_.7lOXevGVVqT2Z_zJCdrB0TN44bYlsUeJrHxdVeLJkHb02mtIlMawRTRdBkn-5WfW_mA0cVhFZdeelIM9m75sxI6kL54DG3x3zx_3yAcPJcb-jAX9wwRpH6gykp6EUc2vYVOArIDef_Y27I2VU7JfVWjt8rjeG7pxhX7-wQKG7rEYu55Yzh3FMwTx-JV-xmPJtr5n4-bgHI6pD6E89JjEiFhoGzew-eLgPKjYls_GrUxY3AUSy6fm9e3gCEuPN6qUAYA3oScPsl0v3NlH4L9DnO6L_VlL_1ibn9Y4HDfSdqS3_x5wmcEo6uWcVkUsqqDc_PjXPMW3RzvP31dF-Z6UawrIKVUityfTPHCQlygqF6eJrGZWxC7G";

    if (action == "place")
    {
        if (argc != 5)
        {
            std::cerr << "Usage: " << argv[0] << " place <instrument_name> <amount> <price>\n";
            return 1;
        }

        std::string instrument_name = argv[2];
        int amount = std::stoi(argv[3]);
        double price = std::stod(argv[4]);

        std::string response = place_order(access_token, instrument_name, amount, price);
        std::cout << "Order Response: " << response << std::endl;
    }
    if (action == "get_order")
    {
        std::string response = get_order(access_token);
        std::cout << "Action Response: " << response << std::endl;
    }
    return 0;
}
