#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cmath>

using namespace std;

class ComputerClub
{
	struct Event // ñòðóêòóðà ñîáûòèÿ
	{
		string time; // âðåìÿ ñîáûòèÿ
		int id; //id ñèòóàöèè
		string name; // èìÿ êëèåíòà
	};

	struct Table
	{
		string entryTime = ""; // âðåìÿ ïîñàäêè çà ñòîë
		int entryHour = 0;
		int entryMinute = 0;
		string exitTime = ""; // âðåìÿ âûñàäêè èç-çà ñòîëà
		int exitHour = 0;
		int exitMinute = 0;
		int entryTotalMinute = 0; 
		int exitTotalMinute = 0;
	};

	unordered_map <int, int> moneyTables; // âûðó÷êà ñòîëîâ
	unordered_map <int, int> timeTables; // âðåìÿ çà ñòîëîì

	int tableNumber; // òåêóùèé íîìåð ñòîëà
	int numTables; // êîëè÷åñòâî ñòîëîâ
	string startTime; // âðåìÿ íà÷àëà ðàáîòû
	string endTime; // âðåìÿ êîíöà ðàáîòû
	int price; // ñòîèìîñòü ñòîëà â ÷àñ
	ifstream file; // ðàáî÷èé ôàéë ñ ñîáûòèÿìè
	unordered_set <string> names; // ñïèñîê íàõîäÿùèõñÿ â êëóáå êëèåíòîâ
	unordered_map <int, bool> tables; // ñïèñîê ñòîëîâ
	unordered_map <string, int> namesTables; // ñïèñîê ãîñòåé çà ñòîëàìè
	queue <string> queClients; // î÷åðåäü èç êëèåíòîâ

public:
	ComputerClub(){} // êîíñòðóêòîð ïî óìîë÷àíèþ

	void convertTimeFormat(const string& time, int& hours, int& minutes); // ôóíêöèÿ ïðåîáðàçîâàíèÿ ôîðìàòà âðåìåíè ÕÕ:ÕÕ â ÷àñû è ìèíóòû
	void displayEventInfo(const Event& event);
	void GetValue(const string& filename);	
	bool isValidTimeFormat(const string& time);

};

bool ComputerClub::isValidTimeFormat(const string& time)
{
	// Ôîðìàò âðåìåíè ××:ÌÌ èìååò äëèíó 5 ñèìâîëîâ è ðàçäåëåí äâîåòî÷èåì.
	if (time.length() != 5 || time[2] != ':')
		return false;

	// Ïðîâåðÿåì, ÷òî âñå ñèìâîëû äî è ïîñëå äâîåòî÷èÿ ÿâëÿþòñÿ öèôðàìè.
	for (int i = 0; i < 5; ++i)
	{
		if (i == 2)
			continue; // Ïðîïóñêàåì ñèìâîë äâîåòî÷èÿ
		if (!isdigit(time[i]))
			return false;
	}

	// Ïðîâåðÿåì, ÷òî çíà÷åíèÿ ÷àñîâ è ìèíóò íàõîäÿòñÿ â ðàçóìíûõ ïðåäåëàõ.
	int hours = stoi(time.substr(0, 2));
	int minutes = stoi(time.substr(3, 2));
	if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59)
		return false;

	return true;
}

void ComputerClub:: GetValue(const string& filename)
{
	file.open(filename);
	if (!file.is_open())  // ïðîâåðêà îòêðûòèÿ ôàéëà
	{
		cout << "Error: Cannot open file " << filename << endl;
		return;
	}

	file >> numTables >> startTime >> endTime >> price; // çàíîñèì íà÷àëüíûå äàííûå â ïåðåìåííûå
	cout << startTime << endl; // âûâîä âðåìåíè íà÷àëà ðàáîòû

	// çàïîëíÿåì map êîëè÷åñòâîì ñòîëîâ (èçíà÷àëüíî âñå ñâîáîäíû)
	for (int i = 0; i < numTables; ++i)
	{
		tables.insert({ i,false });
		moneyTables.insert({ i,0 });
		timeTables.insert({ i,0 });
	}

	int startHour, startMinute, endHour, endMinute;
	// ïåðåâîäèì ÷àñû è ìèíóòû â óäîáíûé ôîðìàò
	convertTimeFormat(startTime, startHour, startMinute);
	convertTimeFormat(endTime, endHour, endMinute);
	
	Event event;
	Table * table = new Table[numTables];
	int currentHour, currentMinute;
	string prevTime = startTime; 
	bool firstClientBeforeOpening = false;

	while (file >> event.time >> event.id >> event.name)
	{
		displayEventInfo(event);
		convertTimeFormat(event.time, currentHour, currentMinute);

		auto nm = names.find(event.name);

		switch (event.id)
		{
		case 1:
			// íàõîäèòñÿ ëè êëèåíò â êëóáå
			if (nm != names.end())
			{
				cout << endl << "13 YouShallNotPass";
			}
			// îòêðûò ëè êëóá
			else if (currentHour < startHour || currentHour >= endHour)
			{
				firstClientBeforeOpening = true;
				cout << endl << event.time << " 13 NotOpenYet";
			}
			// çàíîñèì êëèåíòà â ñïèñîê ïîñåòèòåëåé
			else
			{
				firstClientBeforeOpening = false;
				names.insert(event.name);
			}
			break;
		case 2:
			file >> tableNumber;
			cout << " " << tableNumber;
			// íàõîäèòñÿ ëè êëèåíò â êëóáå
			if (nm == names.end())
			{
				cout << endl << " 13 ClientUnknown";
			}
			// çàíÿò ëè ñòîë ïî íîìåðó tableNumber
			else if (tables[tableNumber])
			{
				cout << endl << event.time << " 13 PlaceIsBusy";
				queClients.push(event.name);
			}
			// åñëè ñòîë ñâîáîäåí 
			else
			{
				// îòìå÷àåì ñòîë êàê çàíÿòûé
				tables[tableNumber] = true;
				namesTables.insert({ event.name,tableNumber });

				// âíîñèì âðåìÿ íà÷àëà áðîíè ñòîëà 
				table[tableNumber - 1].entryTime = event.time;
				convertTimeFormat(table[tableNumber - 1].entryTime, table[tableNumber - 1].entryHour, table[tableNumber - 1].entryMinute);
				table[tableNumber - 1].entryTotalMinute = table[tableNumber - 1].entryHour * 60 + table[tableNumber - 1].entryMinute;
			}
			break;
		case 3:
			for (int i = 0; i < numTables; ++i)
			{
				if (!tables[tableNumber])
				{
					cout << endl << event.time << " 13 ICanWaitNoLonger!";
					break;
				}
			}
			if (queClients.size() > numTables)
			{
				cout << endl << event.time << " 11 " << event.name;
				names.erase(event.name);
				queClients.pop();
			}
			break;
		case 4:
			// íàõîäèòñÿ ëè êëèåíò â êëóáå
			if (nm == names.end())
			{
				cout << endl << " 13 ClientUnknown";
			}
			else
			{
				// îñâîáîæäåíèå ñòîëà, çà êîòîðûì ñèäåë êëèåíò	
				tableNumber = namesTables[event.name];
				tables[tableNumber] = false;
				
				// âíîñèì âðåìÿ êîíöà áðîíè ñòîëà
				table[tableNumber-1].exitTime = event.time;
				convertTimeFormat(table[tableNumber - 1].exitTime, table[tableNumber - 1].exitHour, table[tableNumber - 1].exitMinute);
				table[tableNumber - 1].exitTotalMinute = table[tableNumber - 1].exitHour * 60 + table[tableNumber - 1].exitMinute;
				// ïîäñ÷åò âûðó÷êè è âðåìåíè 
				moneyTables[tableNumber] += (ceil(static_cast<double>(table[tableNumber - 1].exitTotalMinute - table[tableNumber - 1].entryTotalMinute) / 60.0)) * price;
				timeTables[tableNumber] += table[tableNumber - 1].exitTotalMinute - table[tableNumber - 1].entryTotalMinute;

				// óäàëåíèå êëèåíòà èç ñïèñêà ïîñåòèòåëåé
				names.erase(event.name);
				namesTables.erase(event.name);
			}		
			if (!queClients.empty())
			{
				cout << endl << event.time << " 12 " << queClients.front() << " " << tableNumber;
				namesTables.insert({ queClients.front(),tableNumber });
				queClients.pop();
				tables[tableNumber] = true;		
				table[tableNumber - 1].entryTime = event.time;
				convertTimeFormat(table[tableNumber - 1].entryTime, table[tableNumber - 1].entryHour, table[tableNumber - 1].entryMinute);
				table[tableNumber - 1].entryTotalMinute = table[tableNumber - 1].entryHour * 60 + table[tableNumber - 1].entryMinute;
			}
			break;
		}

		// ïðîâåðêà ôîðìàòà âðåìåíè
		if (!isValidTimeFormat(event.time))
		{
			file.close();
			return;
		}

		// ïðîâåðêà ôîðìàòà ïîñëåäîâàòåëüíîñòè ñîáûòèé
		if (event.time < prevTime && !firstClientBeforeOpening)
		{
			file.close();
			return;
		}

		// ïðîâåðêà ôîðìàòà èìåíè êëèåíòà
		for (char ch : event.name)
		{
			if (!isalnum(ch) && ch != '_' && ch != '-')
			{
				file.close();
				return;
			}
		}

		cout << endl;
		prevTime = event.time;
	}

	while (!names.empty())
	{
		auto it = names.begin();
		cout << endTime << " 11 " << *it << endl;
		tableNumber = namesTables[*it];
		table[tableNumber - 1].exitTime = endTime;
		convertTimeFormat(table[tableNumber - 1].exitTime, table[tableNumber - 1].exitHour, table[tableNumber - 1].exitMinute);
		table[tableNumber - 1].exitTotalMinute = table[tableNumber - 1].exitHour * 60 + table[tableNumber - 1].exitMinute;
		moneyTables[tableNumber] += (ceil(static_cast<double>(table[tableNumber - 1].exitTotalMinute - table[tableNumber - 1].entryTotalMinute) / 60.0)) * price;
		timeTables[tableNumber] += table[tableNumber - 1].exitTotalMinute - table[tableNumber - 1].entryTotalMinute;
		names.erase(it);	
	}

	cout << endTime << endl; // âûâîä âðåìåíè êîíöà ðàáîòû

	// âûâîä âûðó÷êè
	for (int i = 0; i < numTables; ++i)
	{
		int hours = timeTables[i + 1] / 60;
		int minutes = timeTables[i + 1] % 60;
		cout << i + 1 << " " << moneyTables[i + 1] << " " << setfill('0') << setw(2) << hours << ":" << setw(2) << minutes << endl;
	}
	

	file.close();
}

void ComputerClub::convertTimeFormat(const string& time, int& hours, int& minutes) 
{
	hours = stoi(time.substr(0, 2));
	minutes = stoi(time.substr(3, 2));
}

void ComputerClub:: displayEventInfo(const Event& event) 
{
	cout << event.time << " " << event.id << " " << event.name;
}


int main(int argc, char* argv[])
{

	ComputerClub A;
	A.GetValue(argv[1]);

	return 0;
}
