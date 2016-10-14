#include "website.h"
#include "functions.h"
#include <cstdlib>
#include <windows.h>
#include <wininet.h>
using std::cerr;
using std::endl;

WebSite::WebSite(const char* leaguePagePath, const char* outFileName,
		const string url, int* specificSeason) :
		outputFileName(outFileName), requiredSeason(string("")), readPlayerPageFunc(
				&WebSite::readPlayerPageAllSeasons) {
	playerPageStr = leaguePagePath;
	int pos = playerPageStr.find(url);
	if (pos != (int) string::npos)
		playerPageStr.erase(0, url.length() + pos);
	outputFile.open(outFileName);
	if (!outputFile.is_open())
		throw generalError::OPEN_FILE_ERROR();
	if (specificSeason) {
		requiredSeason = intToString(*specificSeason);
		readPlayerPageFunc = &WebSite::readPlayerPageSpecificSeason;
	}
}

WebSite::~WebSite() {
	outputFile.close();
}

WebSite::WebSite(const WebSite& x) :
		requiredSeason(x.requiredSeason), readPlayerPageFunc(
				x.readPlayerPageFunc) {
	cerr << "using WebSite copy c'tor" << endl;
}

WebSite& WebSite::operator=(const WebSite& x) {
	cerr << "using WebSite operator=" << endl;
	return *this;
}

void WebSite::readPage(const string& url, string& pageStr, string pageType,
		void (WebSite::*func)()) {
	pageStr = saveSourcePage(url.c_str(), playerPageStr.c_str());
	cout << endl << "Reading " << pageType << " data from site" << endl;
	(this->*func)();
	normalizeOutputFile();
}

void WebSite::detectPageType(string url, string playerPageIdentifier,
		string leaguePageIdentifier) {
	if (playerPageStr.find(playerPageIdentifier) != string::npos)
		readPage(url, playerPageStr, "player", readPlayerPageFunc);
	else if (playerPageStr.find(leaguePageIdentifier) != string::npos)
		readPage(url, leaguePageStr, "league", &WebSite::readLeaguePage);
	else
		readPage(url, clubPageStr, "club", &WebSite::readClubPage);

}

static void removeExtraSpaces(string& str) {
	while ((str.length() > 0) && (str[0] == ' '))
		str.erase(0, 1);
	while ((str.length() > 0) && (str[str.length() - 1] == ' '))
		str.erase(str.length() - 1, 1);
}

/*Returns name in pos in website page */
string WebSite::readName(string& str, int& pos, string tag1, string tag2,
		string tag3) {
	pos = str.find(tag1, pos);
	if (pos == (int) string::npos)
		throw WebSiteError::TAG_NOT_FOUND();
	pos = str.find(tag2, pos);
	if (pos == (int) string::npos)
		throw WebSiteError::TAG_NOT_FOUND();
	pos += tag2.length();
	int length = str.find(tag3, pos) - pos;
	if (length < 0)
		throw WebSiteError::TAG_NOT_FOUND();
	string name = str.substr(pos, length);
	removeExtraSpaces(name);
	return name;
}

/*Converts site month (string) to CSV month (2 digits) */
string WebSite::convertMonth(string months[], string& month) {
	for (int i = 1; i < 13; i++) {
		if (months[i] == month) {
			string res = intToString(i);
			if (i < 10)
				res.insert(0, "0");
			return res;
		}
	}
	return month;
}

void WebSite::formatSeasonType1(string& season) {
	if (season.length() > 4) {
		season = season.substr(3, 2);
		if (atoi(season.c_str()) < 50)
			season = "20" + season;
		else
			season = "19" + season;
	}
}

string WebSite::saveSourcePage(const char* url, const char* path) {
	HINTERNET hInternet = InternetOpenA("InetURL/1.0",
	INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	HINTERNET hConnection = InternetConnectA(hInternet, url, 80, " ", " ",
	INTERNET_SERVICE_HTTP, 0, 0);

	HINTERNET hData = HttpOpenRequestA(hConnection, "GET", path, NULL, NULL,
	NULL, INTERNET_FLAG_KEEP_CONNECTION, 0);

	string pageStr = "";
	char buf[2048];
	HttpSendRequestA(hData, NULL, 0, NULL, 0);

	DWORD bytesRead = 0;
	// http://msdn.microsoft.com/en-us/library/aa385103(VS.85).aspx
	// To ensure all data is retrieved, an application must continue to call the
	// InternetReadFile function until the function returns TRUE and the
	// lpdwNumberOfBytesRead parameter equals zero.
	while (InternetReadFile(hData, buf, 2000, &bytesRead) && bytesRead != 0) {
		buf[bytesRead] = 0; // insert the null terminator.
		pageStr += buf;
	}
	//pFile.close();
	InternetCloseHandle(hData);
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hInternet);
	return pageStr;
}

void WebSite::normalizeOutputFile() {
	outputFile.close();
	leaguePageStr = ifstreamToString(outputFileName.c_str());
	removeUTF(leaguePageStr);
	normalizeIconv("ASCII//TRANSLIT", "UTF-8", leaguePageStr);
	overwriteFileData(outputFileName.c_str(), leaguePageStr);
}
