# Audio-Book-Files-Renamer
Craw file list from websites, and format files with customized formater.

# Modification for Personal Use
  
  1. Craft the `baseURL` at line 145, as well as customized `url` at line 154 to your needs:
     ```cpp
     // Base URL at line 145
     std::string baseURL = "https://www.example.com/";

     ...

     // Construct the URL at line 154
     std::string url = baseURL + "p" + std::to_string(i) + "/";
     ```
  
  2. Modify the match pattern to your needs
     ```cpp
     xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST "//span[@class='title Mi_']", xpathCtx);
     ```
  
  3. Modify the format rule function body
     ```cpp
     std::list<std::string> chapterTitlesFormatted(std::list<std::string>& chapterTitleList) {
       ...
     }
     ```
  
  4. Formated file

      `chapterTitles.csv` is the formated file, which is sitting inside your `cmake-build-debug` folder.
# Run
  
  1. `mkdir cmake-build-debug` in repo root
  
  2. run `cmake ..`
  
  3. run `make`
  
  4. `./Audio_Book_Files_Renamer`
