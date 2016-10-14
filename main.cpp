#include "trie.h"
#include "soccerway.h"
#include "nft.h"
#include "transfermarkt.h"
#include "club_analysis.h"
#include "player_analysis.h"
using std::cin;
using std::cerr;
using std::endl;

#define SOCCERWAY_IDENTIFIER ".soccerway."
#define TRANSFERMARKT_IDENTIFIER ".transfermarkt."

void printProlog() {
	cout << "Welcome to the Histories Collector" << endl << endl;
	cout << "It is recommended to run the program within the DATA folder"
			<< endl << endl;
	cout << "Please select an action:" << endl;
	cout << "1 - Extract histories from webpage" << endl;
	cout << "2 - Extract histories from webpage + correct mismatches" << endl;
	cout << "3 - Correct mismatches in existing CSV" << endl;
	cout << "4 - Extract positions from webpage" << endl;
	cout << endl;
}

int* selectSpecificYear(string& input) {
	if (input == "4")
		return NULL;

	cout << endl << "Do you want to read specific season entries? (y/n)"
			<< endl;
	string seasonInput;
	cin >> seasonInput;
	while ((seasonInput != "n") && (seasonInput != "y"))
		cin >> seasonInput;

	if (seasonInput == "n")
		return NULL;

	Trie<int> seasonTrie;

	for (int i = 1980; i < 2100; i++) {
		Node<int>* leaf = seasonTrie.addWord(intToString(i));
		int* data = leaf->getData();
		*data = i;
	}

	cout << "Please enter season (single year):" << endl;
	cin >> seasonInput;
	Node<int>* leaf = seasonTrie.searchWord(seasonInput);
	while (!leaf) {
		cout << "Please enter season (single year):" << endl;
		cin >> seasonInput;
		leaf = seasonTrie.searchWord(seasonInput);
	}
	return leaf->getData();
}

int* selectSecondaryPosValue() {
	Trie<int> valuesTrie;
	for (int i = 2; i <= 20; i++) {
		Node<int>* leaf = valuesTrie.addWord(intToString(i));
		int* data = leaf->getData();
		*data = i;
	}

	string valueInput;
	cout << "Please select secondary positions value (2-20):" << endl;
	cin >> valueInput;
	Node<int>* leaf = valuesTrie.searchWord(valueInput);
	while (!leaf) {
		cout << "Please select secondary positions value (2-20):" << endl;
		cin >> valueInput;
		leaf = valuesTrie.searchWord(valueInput);
	}

	return leaf->getData();
}

void collectHistoriesActions(string& input, string& outputFileName) {
	cout << "Please enter webpage URL" << endl;
	string leaguePageURL;
	cin >> leaguePageURL;

	cout << endl << "Please enter destination file name" << endl;
	cout << "(if the file already exist it will be overwritten)" << endl;
	cin >> outputFileName;

	int* specificSeason = selectSpecificYear(input);

	WebSite* site = NULL;
	if (leaguePageURL.find(SOCCERWAY_IDENTIFIER) != string::npos)
		site = new Soccerway(leaguePageURL.c_str(), outputFileName.c_str(),
				specificSeason);
	else if (leaguePageURL.find(TRANSFERMARKT_IDENTIFIER) != string::npos)
		site = new Transfermarkt(leaguePageURL.c_str(), outputFileName.c_str(),
				specificSeason);
	else
		site = new NFT(leaguePageURL.c_str(), outputFileName.c_str(),
				specificSeason);

	if (input != "4")
		site->extractHistory();
	else {
		int* val = selectSecondaryPosValue();
		site->extractPosition(*val);
	}

	delete site;
}

void correctMistakesActions(string& input, string& outputFileName) {
	Trie<int> clubDatShortNameTrie;
	ClubAnalysis* clubAnalysis = NULL;
	if (input == "3") {
		cout
				<< "Please make sure the CSV is in the same folder as the Histories Collector"
				<< endl << endl;
		cout << "Please enter the file name (case sensitive)" << endl;
		cin >> outputFileName;
	}
	cout << endl << "Do you want to analyze CSV clubs? (y/n)" << endl;
	cin >> input;
	while ((input != "n") && (input != "y"))
		cin >> input;

	bool fileConvertedToLower = false;
	if (input == "y") {
		cout << endl
				<< "Analyzing CSV clubs (you can quit any time by pressing q+Enter)"
				<< endl << endl;
		fileToLower(outputFileName);
		fileConvertedToLower = true;
		clubAnalysis = new ClubAnalysis(clubDatShortNameTrie, outputFileName);
		clubAnalysis->sortCsvClubNames();
	}

	cout << endl << "Do you want to analyze CSV players? (y/n)" << endl;
	cin >> input;
	while ((input != "n") && (input != "y"))
		cin >> input;
	if (input == "y") {
		try {
			cout
					<< "Analyzing CSV players (you can quit any time by pressing q+Enter)"
					<< endl;
			if (!fileConvertedToLower)
				fileToLower(outputFileName);
			if (!clubAnalysis)
				clubAnalysis = new ClubAnalysis(clubDatShortNameTrie,
						outputFileName);
			if (clubDatShortNameTrie.isEmpty())
				clubAnalysis->buildClubDatTries();
			PlayerAnalysis playerAnalysis(clubDatShortNameTrie, outputFileName);
			playerAnalysis.sortCsvPlayerNames();
		} catch (std::exception& e) {
			delete clubAnalysis;
			clubAnalysis = NULL;
			throw e;
		}
	}
	delete clubAnalysis;
	clubAnalysis = NULL;
}

int main(void) {
	printProlog();

	string input;
	cin >> input;
	while ((input != "1") && (input != "2") && (input != "3") && (input != "4"))
		cin >> input;

	string outputFileName = "";
	try {
		if (input != "3") {
			collectHistoriesActions(input, outputFileName);
		}

		if ((input != "1") && (input != "4")) {
			correctMistakesActions(input, outputFileName);
		}
	} catch (std::exception& e) {
		cerr << e.what() << endl;
	}
	cout << endl << "Session ended, thank you and goodbye." << endl << endl;
	system("pause");
	return 0;
}

