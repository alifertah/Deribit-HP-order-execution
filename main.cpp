#include <iostream>
#include <string>
#include <curl/curl.h>

/**
 * @brief Executes an HTTP request to the given URL with the provided access token.
 *
 * @param url
 * @param access_token
 * @return std::string
 */
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

/**
 * @brief this function sets the necessary params for the url req to place an order
 *
 * @param access_token
 * @param instrument_name
 * @param amount
 * @param price
 * @return std::string
 */
std::string place_order(const std::string &access_token, const std::string &instrument_name, int amount, double price)
{
    std::string url = "https://test.deribit.com/api/v2/private/buy?instrument_name=" + instrument_name +
                      "&amount=" + std::to_string(amount) + "&price=" + std::to_string(price);

    return execute_request(url, access_token);
}

/**
 * @brief Get the order object
 *
 * @param access_token
 * @return std::string
 */
std::string get_order(std::string &access_token)
{
    std::string url = "https://test.deribit.com/api/v2/private/get_open_orders";
    return execute_request(url, access_token);
}

/**
 * @brief this function cancels an order by the order_id provided in the args
 *
 * @param access_token
 * @param order_id
 * @return std::string
 */
std::string cancel_order(std::string &access_token, std::string &order_id)
{
    std::cout << "Order ID: " << order_id << std::endl;
    std::string url = "https://test.deribit.com/api/v2/private/cancel?order_id=" + order_id;
    return execute_request(url, access_token);
}

std::string get_order_by_id(std::string &access_token, std::string &order_id)
{
    std::string url = "https://test.deribit.com/api/v2/private/get_order_state?order_id=" + order_id;
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
    std::string access_token = "1737145024106.1XgG9uVj.-AgvhGEtrpl9dMgzbWJpvFmSl6InwobH_n0uv28NM0Yp5CcB10yHnBuogb_UT-1ebgQDca7Dg-1gSy75WyB0S8n2U_gHmGR8sxLTLVxqslaVsfEeGlcjJZKssinS7W-5OjlhrLi-WofvtfERC4eqK_2-BOBY364wB8fBpSsMfWD0Pf5iDxi9arz2Cmdo0eRONHJYAuTBX2GeOXKTxFZnUAHJxpvS2VeopmMnCiqyHBkWrcHqLWULmsKqzttR_2upMW65F62jtZ2u8IEpOSCXt6AheE9GL9ViMsE4daVlRzJR0nqEjsGmgkOOgCyLu8FS3-jw-JaTuY86tKFIpXF2gXb4GvEeSN_3om0KIM4hrFEsvLs28uHi";

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
    if (action == "cancel_order")
    {
        std::string order_id = argv[2];
        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << " cancel_order <order_id>\n";
            return 1;
        }

        std::string response = cancel_order(access_token, order_id);
        std::cout << "Action Response: " << response << std::endl;
    }

    if (action == "get_order_by_id")
    {
        std::string order_id = argv[2];
        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << " get_order_by_id <order_id>\n";
            return 1;
        }

        std::string response = get_order_by_id(access_token, order_id);
        std::cout << "Action Response: " << response << std::endl;
    }

    return 0;
}
