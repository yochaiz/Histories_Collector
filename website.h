#ifndef WEBSITE_H_
#define WEBSITE_H_

#include <fstream> //ofstream#include <string>#include <vector>using std::string;using std::vector;using std::ofstream;namespace WebSiteError {
class NOT_REQUIRED_SEASON: public std::exception {
};
class TAG_NOT_FOUND: public std::exception {
	virtual const char* what() const throw () {
		return "Tag not found exception";
	}
};
}

#define NUM_OF_MONTHS 13

class WebSite {
protected:
	string outputFileName;
	ofstream outputFile;
	string leaguePageStr;
	string clubPageStr;
	string playerPageStr;
	string requiredSeason;
	void (WebSite::*readPlayerPageFunc)();
	void readPage(const string& url, string& pageStr, string pageType,
			void (WebSite::*func)());
	string convertMonth(string months[], string& month);
	virtual string readName(string& str, int& pos, string tag1, string tag2,
			string tag3);
	virtual string readSeason(int& pos)=0;
	virtual void formatDOB(string& dob)=0;
	void formatSeasonType1(string& season);
	string saveSourcePage(const char* url, const char* path);
	void normalizeOutputFile();
	void detectPageType(string url, string playerPageIdentifier,
			string leaguePageIdentifier);
public:
	WebSite(const char* leaguePagePath, const char* outFileName,
			const string url, int* specificSeason);
	WebSite(const WebSite& x); //copy c'tor
	WebSite& operator=(const WebSite& x);
	virtual void readLeaguePage()=0;
	virtual void readClubPage()=0;
	virtual void readPlayerPosition()=0;
	virtual void readPlayerPageAllSeasons()=0;
	virtual void readPlayerPageSpecificSeason()=0;
	virtual void extractHistory()=0;
	virtual void extractPosition(int val)=0;
	virtual ~WebSite();
};

#endif /* WEBSITE_H_ */
