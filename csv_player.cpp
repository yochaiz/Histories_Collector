#include "csv_player.h"

CsvPlayer::CsvPlayer() :
		name(""), clubName("") {
}

CsvPlayer::CsvPlayer(string name1, string name3) :
		name(name1), clubName(name3) {
}

string CsvPlayer::getName() const {
	return name;
}

string CsvPlayer::getClubName() const {
	return clubName;
}

CsvPlayer::operator string() const {
	return name;
}

bool operator<(const CsvPlayer& item1, const CsvPlayer& item2) {
	if (item1.name == item2.name)
		return (item1.clubName < item2.clubName);
	return (item1.name < item2.name);
}

bool operator==(const CsvPlayer& item1, const CsvPlayer& item2) {
	return ((item1.name == item2.name) && (item1.clubName == item2.clubName));
}

ostream& operator<<(ostream& os, const CsvPlayer& item) {
	return os << item.name << " (" << item.clubName << ")";
}
