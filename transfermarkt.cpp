#include "transfermarkt.h"
#include <iostream> //for cerrusing std::cout;using std::cerr;using std::endl;static string* initMonths() {	string* months = new string[NUM_OF_MONTHS];	months[1] = "Jan";	months[2] = "Feb";	months[3] = "Mar";	months[4] = "Apr";	months[5] = "May";	months[6] = "Jun";	months[7] = "Jul";	months[8] = "Aug";	months[9] = "Sep";	months[10] = "Oct";	months[11] = "Nov";	months[12] = "Dec";	return months;}static map<string, int> initValues() {	map<string, int> tmp;	tmp["Mill"] = 1000000;	tmp["Th"] = 1000;	return tmp;}static map<string, Position> initPositions() {	map<string, Position> tmp;	tmp["Keeper"] = Position(0);	tmp["Sweeper"] = Position(1, 2);	tmp["Centre Back"] = Position(2, 2);	tmp["Right-Back"] = Position(2, 7, 0);	tmp["Left-Back"] = Position(2, 7, 1);	tmp["Defensive Midfield"] = Position(3, 4, 2);	tmp["Central Midfield"] = Position(4, 2);	tmp["Left Midfield"] = Position(4, 1);	tmp["Right Midfield"] = Position(4, 0);	tmp["Attacking Midfield"] = Position(4, 5, 2);	tmp["Left Wing"] = Position(4, 5, 1);	tmp["Right Wing"] = Position(4, 5, 0);	tmp["Secondary Striker"] = Position(6, 8, 2);	tmp["Centre Forward"] = Position(6, 2);	tmp["Defence"] = Position(2);	tmp["Midfield"] = Position(4);	tmp["Striker"] = Position(6, 2);	return tmp;}string Transfermarkt::url = "transfermarkt.com";string* Transfermarkt::months = initMonths();map<string, int> Transfermarkt::valuesMap = initValues();map<string, Position> Transfermarkt::positionMap = initPositions();static void sortPlayerHistoryURL(string& path) {	int pos2 = path.find("/spieler/");	if (pos2 == (int) string::npos) //not a player pagereturn	;	int pos3 = path.rfind("/", pos2 - 1);	path = path.replace(pos3, (pos2 - pos3), "/detaillierteleistungsdaten");}static void sortPlayerPositionURL(string& path) {	int pos2 = path.find("/spieler/");	if (pos2 == (int) string::npos) //not a player pagereturn	;	int pos3 = path.rfind("/", pos2 - 1);	path = path.replace(pos3, (pos2 - pos3), "/profil");}void Transfermarkt::extractHistory() {	outputFile << "Name,DOB,Club,Year,Apps,Goals,Loan" << endl;	sortPlayerURL = &sortPlayerHistoryURL;
	if (requiredSeason != "")
		readPlayerPageFunc = &WebSite::readPlayerPageSpecificSeason;
	else
		readPlayerPageFunc = &WebSite::readPlayerPageAllSeasons;
	sortPlayerHistoryURL(playerPageStr);
	detectPageType(url, "/spieler/", "/wettbewerb/");
}

void Transfermarkt::extractPosition(int val) {
	outputFile
			<< "First Name,Last Name,Common Name,DOB,Nation1,Nation2,Contract start,Contract end,Value,Club,On loan from,GK,SW,D,DM,M,AM,ST,WB,FR,Right,Left,Center,Right Foot,Left Foot,Int Caps,Int Goals"
			<< endl;
	secondPosValue = val;
	sortPlayerURL = &sortPlayerPositionURL;
	readPlayerPageFunc = &WebSite::readPlayerPosition;
	sortPlayerPositionURL(playerPageStr);
	detectPageType(url, "/spieler/", "/wettbewerb/");
}
Transfermarkt::~Transfermarkt() {
}

Transfermarkt::Transfermarkt(const char* leaguePagePath,
		const char* outFileName, int* specificSeason) :
		WebSite(leaguePagePath, outFileName, url, specificSeason) {

}

Transfermarkt::Transfermarkt(const Transfermarkt& x) :
		WebSite(x), secondPosValue(x.secondPosValue), sortPlayerURL(
				x.sortPlayerURL) {
	cerr << "using Transfermarkt copy c'tor" << endl;
}
Transfermarkt& Transfermarkt::operator=(const Transfermarkt& x) {
	cerr << "using Transfermarkt operator=" << endl;
	return *this;
}
void Transfermarkt::formatDOB(string& dob) {
	int pos = dob.find(" ");
	if (pos < 0)
		return;
	string month = dob.substr(0, pos);
	month = convertMonth(months, month);
	pos++;
	int length = dob.find(",", pos) - pos;
	string day = dob.substr(pos, length);
	if (day.length() == 1)
		day.insert(0, "0");
	pos += length + 2;
	string year = dob.substr(pos, 4);
	dob = day + "." + month + "." + year;
}

void Transfermarkt::formatValue(string& value) {
	string::size_type pos = value.find(",");
	if (pos != string::npos)
		value = value.replace(pos, 1, ".");
	pos = value.find(" ");
	string tmp = value.substr(0, pos);
	double dVal = stringToNumber<double>(tmp);
	tmp = value.substr(pos + 1, value.length() - 1);
	int intVal = dVal * (valuesMap[tmp]);
	value = intToString(intVal);
}

void Transfermarkt::convertPosition(int* positions, int* sides,
		string& sitePosName, int value) {
	Position p = positionMap[sitePosName];
	p.updatePositions(positions, value);
	p.updateSides(sides, value);
}

static void initArrays(int* a, int* b) {
	for (int i = 0; i < NUM_OF_POSITIONS; i++)
		a[i] = 1;

	for (int i = 0; i < NUM_OF_SIDES; i++)
		b[i] = 1;
}

void Transfermarkt::printPositions(int* positions, int* sides) {
	for (int i = 0; i < NUM_OF_POSITIONS; i++)
		outputFile << "," << positions[i];

	for (int i = 0; i < NUM_OF_SIDES; i++)
		outputFile << "," << sides[i];
}

void Transfermarkt::convertFoot(string& foot, int& right, int& left,
		int* sides) {
	if (foot == "-")
		return;

	if (foot == "right") {
		right = 20;
		if (sides[1] >= secondPosValue)
			left = secondPosValue;
	} else if (foot == "left") {
		left = 20;
		if (sides[0] >= secondPosValue)
			right = secondPosValue;
	} else {
		right = left = 18;
		if (sides[0] == 20)
			right = 20;
		if (sides[1] == 20)
			left = 20;
		if ((right == left) && (right < 20))
			right = 20;
	}
}

string Transfermarkt::readField(int& pos, string tag1, string tag2,
		string tag3) {
	string str = "-";
	int newPos = pos;
	try {
		str = readName(playerPageStr, newPos, tag1, tag2, tag3);
		pos = newPos;
	} catch (WebSiteError::TAG_NOT_FOUND& e) {
	}
	return str;
}

void Transfermarkt::clearExtraTags(string& name, string& fname, string& sname,
		string& cname, bool searchFullName) {
	const string tag1 = "<b>";
	const string tag2 = "</b>";
	int pos = name.find(tag1);

	if (pos == 0) {
		int startPos = pos + tag1.length();
		int endPos = name.find(tag2);
		name = name.substr(startPos, endPos - startPos);
		cname = name;

		if (searchFullName) {
			string fullName = readField(pos, ">Complete name:<", "<td>", "<");
			if (fullName != "-") {
				pos = fullName.find(cname);
				if (pos == 0) {
					fname = fullName.substr(0, cname.length());
					startPos = cname.length() + 1;
					int len = fullName.length() - startPos;
					sname = fullName.substr(startPos, len);
				} else if (pos > 0) {
					fname = fullName.substr(0, pos - 1);
					sname = fullName.substr(pos, fullName.length() - pos);
				} else { // cname isn't part of full name
					pos = fullName.find(" ");
					if (pos > 0) {
						pos++;
						fname = fullName.substr(0, pos - 1);
						sname = fullName.substr(pos, fullName.length() - pos);
					} else {
						fname = fullName;
					}
				}
			}
		}
	} else if (pos > 0) {
		int endPos = name.find(tag2);
		if (endPos >= 0) {
			int startPos = pos + tag1.length();
			sname = name.substr(startPos, endPos - startPos);
			if (pos > 0) {
				fname = name.substr(0, pos - 1); // -1 for space
				name = fname + " " + sname;
			} else {
				name = sname;
			}
		}
	}
}

static void clearCommas(string& str) {
	int pos = str.find(",");
	for (; pos >= 0; pos = str.find(",")) {
		str = str.erase(pos, 1);
	}
}

void Transfermarkt::readPlayerPosition() {
	int pos = 0;
	string name = readName(playerPageStr, pos, "<h1 itemprop", "name\">",
			"</h1>");
	if (name == "-")
		return;

	string fname = "", sname = "", cname = "";
	clearExtraTags(name, fname, sname, cname, true);
	cout << name << endl;
	int positions[NUM_OF_POSITIONS], sides[NUM_OF_SIDES];
	initArrays(positions, sides);
	string mainPosition = readField(pos, ">Main position:<", "/>", "\t");
	convertPosition(positions, sides, mainPosition, 20);
	string secondPosition = readField(pos, ">Side position:<", "/>", "\t");
	convertPosition(positions, sides, secondPosition, secondPosValue);
	int endPos = playerPageStr.find("</div", pos);
	pos = playerPageStr.find("/>", pos);
	while (pos < endPos) {
		secondPosition = readField(pos, "/>", "/>", "\t");
		convertPosition(positions, sides, secondPosition, secondPosValue);
		pos = playerPageStr.find("\">", pos);
	}
	if ((mainPosition == "-") && (secondPosition == "-")) {
		pos = 0;
		mainPosition = readField(pos, ">Position:<",
				"td>\r\n\t\t\t\t\t\t\t\t\t", "\t\t\t\t\t\t\t\t<");
		convertPosition(positions, sides, mainPosition, 20);
	}
	string value = readField(pos, "Current market value:", "\">\r\n", ". ");
	formatValue(value);
	string DOB = readName(playerPageStr, pos, "/datum/", ">", "<");
	formatDOB(DOB);
	string nationality1 = readField(pos, "<th>Nationality:<", "title=\"", "\"");
	clearCommas(nationality1);
	endPos = playerPageStr.find("</td>", pos);
	string nationality2 = readField(pos, "title=", "\"", "\"");
	clearCommas(nationality2);
	if (pos > endPos) { // title= isn't a nationality
		nationality2 = "-";
		pos = endPos;
	}
	string foot = readField(pos, ">Foot:<", "<td>", "<");
	int right = 1, left = 1;
	convertFoot(foot, right, left, sides);
	pos = playerPageStr.find("Current club:", pos);
	string club = readField(pos, "alt=", "\"", "\"");
	string contractStart = readField(pos, ">In the team since:<",
			"td>\r\n\t\t\t\t\t\t\t\t\t\t", "\t\t\t\t\t\t\t\t\t<");
	formatDOB(contractStart);
	string contractEnd = readField(pos, "Contract until:", "<td>", "</td>");
//	pos = playerPageStr.find(">Nationality:<", pos);
	string loan = "-";
	int tmpPos = playerPageStr.find(">on loan from:<", pos);
	if (tmpPos != (int) string::npos) {
		pos = tmpPos;
		loan = readField(pos, "<a class", ">", "<");
	}
	string intCaps = "0";
	string intGoals = "0";
	tmpPos = playerPageStr.find("zentriert bg_", pos);
	if (tmpPos != (int) string::npos) {
		string nation = readName(playerPageStr, tmpPos, "title=", "\"", "\"");
		int tmpPos2 = nation.rfind(" U");
		if (tmpPos2 == (int) string::npos) {
			tmpPos = playerPageStr.find("class=\"zentriert\"", tmpPos);
			if (tmpPos != (int) string::npos) {
				tmpPos = playerPageStr.find("<td class=", tmpPos);
				pos = tmpPos;
				intCaps = readField(pos, "<a href=", ">", "<");
				if (intCaps == "-")
					intCaps = "0";
				intGoals = readField(pos, "<a href=", ">", "<");
				if (intGoals == "-")
					intGoals = "0";
			}
		}
	}
	outputFile << fname << "," << sname << "," << cname << "," << DOB << ","
			<< nationality1 << "," << nationality2 << "," << contractStart
			<< "," << contractEnd << "," << value << "," << club << "," << loan;
	printPositions(positions, sides);
	outputFile << "," << right << "," << left << "," << intCaps << ","
			<< intGoals;
	outputFile << endl;
}

string Transfermarkt::readSeason(int& pos) {
	string season = readName(playerPageStr, pos, "<", ">", "<");
	formatSeasonType1(season);
	return season;
}

void Transfermarkt::readLoanData(vector<HistoryEntry>& historyVec) {

}

/*Reads Transfermarkt player page */
void Transfermarkt::readPlayerPageAllSeasons() {
	int pos = 0;
	string name = readName(playerPageStr, pos, "<h1 itemprop", "name\">",
			"</h1>");
	string fname = "", sname = "", cname = "";
	clearExtraTags(name, fname, sname, cname, false);
	cout << name << endl;
	string DOB = readName(playerPageStr, pos, "birthDate", ">\r\n", "(");
	formatDOB(DOB);
	pos = playerPageStr.find(">National leagues<");
	pos = playerPageStr.find("<tr class", pos) + 1;
	if (pos == 0) //player page is empty, no entries at all
		return;
	int startPos = pos - 1;
	int endPos = playerPageStr.find("</table>", pos);
	//vector<HistoryEntry> historyVec;
	for (; ((pos > startPos) && (pos < endPos));
			pos = playerPageStr.find("<tr class", pos) + 1) {
		string season = readSeason(pos);
		pos = playerPageStr.find("<td", pos) + 1;
		pos = playerPageStr.find("<td", pos) + 1;
		pos = playerPageStr.find("<td", pos) + 1;
		string clubName = readName(playerPageStr, pos, "alt=", "\"", "\"");
		pos = playerPageStr.find("<td", pos) + 1;
		string apps = readName(playerPageStr, pos, "<", ">", "<");
		if (apps == "-")
			apps = "0";
		string goals = readName(playerPageStr, pos, "<td", ">", "<");
		if (goals == "-")
			goals = "0";
		outputFile << name << "," << DOB << "," << clubName << "," << season
				<< "," << apps << "," << goals << ",0" << endl;
		//historyVec.push_back(HistoryEntry(season, clubName, apps, goals));
	}
//readLoanData(historyVec);
}

void Transfermarkt::readPlayerPageSpecificSeason() {
	int pos = 0;
	string name = readName(playerPageStr, pos, "<h1 itemprop", "name\">",
			"</h1>");
	string fname = "", sname = "", cname = "";
	clearExtraTags(name, fname, sname, cname, false);
	cout << name << endl;
	string DOB = readName(playerPageStr, pos, "birthDate", ">\r\n", "(");
	formatDOB(DOB);
	pos = playerPageStr.find(">National leagues<");
	pos = playerPageStr.find("<tr class", pos) + 1;
	if (pos == 0) //player page is empty, no entries at all
		return;
	int startPos = pos - 1;
	int endPos = playerPageStr.find("</table>", pos);
	//vector<HistoryEntry> historyVec;
	for (; ((pos > startPos) && (pos < endPos));
			pos = playerPageStr.find("<tr class", pos) + 1) {
		string season = readSeason(pos);
		if (season != requiredSeason)
			continue;
		pos = playerPageStr.find("<td", pos) + 1;
		pos = playerPageStr.find("<td", pos) + 1;
		pos = playerPageStr.find("<td", pos) + 1;
		string clubName = readName(playerPageStr, pos, "alt=", "\"", "\"");
		pos = playerPageStr.find("<td", pos) + 1;
		string apps = readName(playerPageStr, pos, "<", ">", "<");
		if (apps == "-")
			apps = "0";
		string goals = readName(playerPageStr, pos, "<td", ">", "<");
		if (goals == "-")
			goals = "0";
		outputFile << name << "," << DOB << "," << clubName << "," << season
				<< "," << apps << "," << goals << ",0" << endl;
		//historyVec.push_back(HistoryEntry(season, clubName, apps, goals));
	}
//readLoanData(historyVec);
}

static string getLink(string& str, int& pos, string tag1, string tag2) {
	int pos2 = str.rfind(tag1, pos);
	pos2++;
	pos = str.find(tag2, pos);
	return str.substr(pos2, pos - pos2);
}

void Transfermarkt::readClubPage() {
	int pos = clubPageStr.find("/profil/spieler");
	int endPos = clubPageStr.find("</tbody>", pos);
	for (; pos < endPos; pos = clubPageStr.find("/profil/spieler", pos)) {
		string page = getLink(clubPageStr, pos, "\"", "\"");
		//sortPlayerURL(page);
		this->sortPlayerURL(page);
		playerPageStr = saveSourcePage(url.c_str(), page.c_str());
		try {
			(this->*readPlayerPageFunc)(); //readPlayerPage();
		} catch (WebSiteError::TAG_NOT_FOUND& e) {
			break;
		}
		pos = clubPageStr.find("/profil/spieler", pos); //each link appears twice
		pos++;
	}
}

void Transfermarkt::readLeaguePage() {
	int pos = leaguePageStr.find(">Clubs ");
	int endPos = leaguePageStr.find("</tbody>", pos);
	pos = leaguePageStr.find("<tr class", pos);
	for (; pos < endPos; pos = leaguePageStr.find("<tr class", pos)) {
		pos = leaguePageStr.find("/verein/", pos);
		string page = getLink(leaguePageStr, pos, "\"", "\"");
		clubPageStr = saveSourcePage(url.c_str(), page.c_str());
		readClubPage();
	}
}

