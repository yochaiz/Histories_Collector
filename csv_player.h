#ifndef CSVPLAYER_H_
#define CSVPLAYER_H_

#include <string>
#include <ostream>
using std::string;
using std::ostream;

class CsvPlayer {
protected:
	string name;
	string clubName;
public:
	CsvPlayer();
	CsvPlayer(string name1, string name3);
	string getName() const;
	string getClubName() const;
	operator string() const;
	friend bool operator<(const CsvPlayer& item1, const CsvPlayer& item2);
	friend bool operator==(const CsvPlayer& item1, const CsvPlayer& item2);
	friend ostream& operator<<(ostream& os, const CsvPlayer& item);
};

#endif /* CSVPLAYER_H_ */
