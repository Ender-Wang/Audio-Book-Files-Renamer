#include <iostream>
#include <string>
#include <list>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>
#include <fstream>

/**
 *@brief A Renamer for crawling file names from Ximalaya, and format them to a standard format such as "[Book Title] 001 [Chapter Title].mp3"
 *
 * @return int
 */

size_t curlWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* str = (std::string*)userdata;
    str->append(ptr, size * nmemb);
    return size * nmemb;
}

/**
 *@brief Extract the chapter title from the HTML content
 *
 * @param htmlContent
 * @return std::list<std::string>
 */
std::list<std::string> extractChapterTitle(std::string& htmlContent) {
    std::list<std::string> chapterTitleList;

    // Create an HTML parser context
    htmlDocPtr doc = htmlReadMemory(htmlContent.c_str(), htmlContent.size(), nullptr, nullptr, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);

    if (doc != nullptr) {
        // Create an XPath context
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
        xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST "//span[@class='title Mi_']", xpathCtx);

        if (xpathObj != nullptr) {
            xmlNodeSetPtr nodeset = xpathObj->nodesetval;

            if (nodeset != nullptr) {
                for (int i = 0; i < nodeset->nodeNr; i++) {
                    xmlChar* title = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
                    if (title != nullptr) {
                        chapterTitleList.push_back(reinterpret_cast<char*>(title));
                        xmlFree(title);
                    }
                }
            }
            xmlXPathFreeObject(xpathObj);
        }

        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);
    }

    return chapterTitleList;
}

/**
 *@brief Make a curl request to the URL
 *
 * @param url
 * @return std::list<std::string>
 */

std::list<std::string> curlRequest(const char* url) {
    std::list<std::string> chapterTitleList{};

    CURL* curl{};
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    // Set the URL to crawl
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Set the callback function
    std::string htmlContent;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlContent);

    // Perform the HTTP request
    res = curl_easy_perform(curl);

    // Check for errors
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }
    else {
        // // Receive the HTML content
        // std::cout << "htmlContent: " << htmlContent << std::endl;

        // Extract the chapter title
        chapterTitleList = extractChapterTitle(htmlContent);
    }
    return chapterTitleList;
}

/**
 *@brief Format the chapter titles
 *
 * @param chapterTitles The chapter title Lists to be formatted
 * @return std::list<std::string>
 */
std::list<std::string> chapterTitlesFormatted(std::list<std::string>& chapterTitleList) {
    std::list<std::string> chapterTitlesFormatted{};
    for (auto& chapterTitle : chapterTitleList) {
        std::string formattedTitle{};

        if (chapterTitle.find("（") != std::string::npos) {
            // Find "(" and remove the content after it (including the "(")
            formattedTitle = chapterTitle.substr(0, chapterTitle.find("（"));

        }
        if (chapterTitle.find("+") != std::string::npos) {
            // Split the string by "+", push both parts to the list
            std::string part1 = chapterTitle.substr(0, chapterTitle.find("+"));
            std::string part2 = chapterTitle.substr(chapterTitle.find("+") + 1, chapterTitle.size() - chapterTitle.find("+") - 1);
            chapterTitlesFormatted.push_back(part1);
            formattedTitle = part2;
        }

        // TODO: Add more rules to format the chapter title
        // Find by ”》“ is not working, weird, either by ”》“ or any chinese characters such as "第"

        //Default string
        if (formattedTitle.empty()) {
            formattedTitle = chapterTitle;
        }

        chapterTitlesFormatted.push_back(formattedTitle);
    }

    return chapterTitlesFormatted;
}

int main() {
    // Initialize the chapter title list
    std::list<std::string> chapterTitles{};

    // Base URL
    std::string baseURL = "https://www.ximalaya.com/youshengshu/12642314/";

    // Initialize the curl
    chapterTitles = curlRequest(baseURL.c_str());
    std::cout << "Page 1 is done." << std::endl;

    // For loop to call the curl request multiple times
    for (int i = 2; i <= 28; i++) {
        // Construct the URL
        std::string url = baseURL + "p" + std::to_string(i) + "/";

        // Call the curl request
        std::list<std::string> chapterTitlesTemp = curlRequest(url.c_str());

        // Append the chapter titles to the chapter title list
        chapterTitles.insert(chapterTitles.end(), chapterTitlesTemp.begin(), chapterTitlesTemp.end());

        std::cout << "Page " << i << " is done." << std::endl;
    }

    // // Print the chapter title
    // std::cout << "Chapter Title List:" << std::endl;
    // for (auto& chapterTitle : chapterTitles) {
    //     std::cout << chapterTitle << std::endl;
    // }

    // Format the chapter titles
    chapterTitles = chapterTitlesFormatted(chapterTitles);

    // Write the chapter title to a csv file
    std::ofstream csvFile;
    csvFile.open("chapterTitles.csv");
    // clear the file
    csvFile << "";
    for (auto& chapterTitle : chapterTitles) {
        csvFile << chapterTitle << std::endl;
    }

    return 0;
}
