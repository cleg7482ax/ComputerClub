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
	struct Event // ��������� �������
	{
		string time; // ����� �������
		int id; //id ��������
		string name; // ��� �������
	};

	struct Table
	{
		string entryTime = ""; // ����� ������� �� ����
		int entryHour = 0;
		int entryMinute = 0;
		string exitTime = ""; // ����� ������� ��-�� �����
		int exitHour = 0;
		int exitMinute = 0;
		int entryTotalMinute = 0; 
		int exitTotalMinute = 0;
	};

	unordered_map <int, int> moneyTables; // ������� ������
	unordered_map <int, int> timeTables; // ����� �� ������

	int tableNumber; // ������� ����� �����
	int numTables; // ���������� ������
	string startTime; // ����� ������ ������
	string endTime; // ����� ����� ������
	int price; // ��������� ����� � ���
	ifstream file; // ������� ���� � ���������
	unordered_set <string> names; // ������ ����������� � ����� ��������
	unordered_map <int, bool> tables; // ������ ������
	unordered_map <string, int> namesTables; // ������ ������ �� �������
	queue <string> queClients; // ������� �� ��������

public:
	ComputerClub(){} // ����������� �� ���������

	void convertTimeFormat(const string& time, int& hours, int& minutes); // ������� �������������� ������� ������� ��:�� � ���� � ������
	void displayEventInfo(const Event& event);
	void GetValue(const string& filename);	
	bool isValidTimeFormat(const string& time);

};

bool ComputerClub::isValidTimeFormat(const string& time)
{
	// ������ ������� ��:�� ����� ����� 5 �������� � �������� ����������.
	if (time.length() != 5 || time[2] != ':')
		return false;

	// ���������, ��� ��� ������� �� � ����� ��������� �������� �������.
	for (int i = 0; i < 5; ++i)
	{
		if (i == 2)
			continue; // ���������� ������ ���������
		if (!isdigit(time[i]))
			return false;
	}

	// ���������, ��� �������� ����� � ����� ��������� � �������� ��������.
	int hours = stoi(time.substr(0, 2));
	int minutes = stoi(time.substr(3, 2));
	if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59)
		return false;

	return true;
}

void ComputerClub:: GetValue(const string& filename)
{
	file.open(filename);
	if (!file.is_open())  // �������� �������� �����
	{
		cout << "Error: Cannot open file " << filename << endl;
		return;
	}

	file >> numTables >> startTime >> endTime >> price; // ������� ��������� ������ � ����������
	cout << startTime << endl; // ����� ������� ������ ������

	// ��������� map ����������� ������ (���������� ��� ��������)
	for (int i = 0; i < numTables; ++i)
	{
		tables.insert({ i,false });
		moneyTables.insert({ i,0 });
		timeTables.insert({ i,0 });
	}

	int startHour, startMinute, endHour, endMinute;
	// ��������� ���� � ������ � ������� ������
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
			// ��������� �� ������ � �����
			if (nm != names.end())
			{
				cout << endl << "13 YouShallNotPass";
			}
			// ������ �� ����
			else if (currentHour < startHour || currentHour >= endHour)
			{
				firstClientBeforeOpening = true;
				cout << endl << event.time << " 13 NotOpenYet";
			}
			// ������� ������� � ������ �����������
			else
			{
				firstClientBeforeOpening = false;
				names.insert(event.name);
			}
			break;
		case 2:
			file >> tableNumber;
			cout << " " << tableNumber;
			// ��������� �� ������ � �����
			if (nm == names.end())
			{
				cout << endl << " 13 ClientUnknown";
			}
			// ����� �� ���� �� ������ tableNumber
			else if (tables[tableNumber])
			{
				cout << endl << event.time << " 13 PlaceIsBusy";
				queClients.push(event.name);
			}
			// ���� ���� �������� 
			else
			{
				// �������� ���� ��� �������
				tables[tableNumber] = true;
				namesTables.insert({ event.name,tableNumber });

				// ������ ����� ������ ����� ����� 
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
			// ��������� �� ������ � �����
			if (nm == names.end())
			{
				cout << endl << " 13 ClientUnknown";
			}
			else
			{
				// ������������ �����, �� ������� ����� ������	
				tableNumber = namesTables[event.name];
				tables[tableNumber] = false;
				
				// ������ ����� ����� ����� �����
				table[tableNumber-1].exitTime = event.time;
				convertTimeFormat(table[tableNumber - 1].exitTime, table[tableNumber - 1].exitHour, table[tableNumber - 1].exitMinute);
				table[tableNumber - 1].exitTotalMinute = table[tableNumber - 1].exitHour * 60 + table[tableNumber - 1].exitMinute;
				// ������� ������� � ������� 
				moneyTables[tableNumber] += (ceil(static_cast<double>(table[tableNumber - 1].exitTotalMinute - table[tableNumber - 1].entryTotalMinute) / 60.0)) * price;
				timeTables[tableNumber] += table[tableNumber - 1].exitTotalMinute - table[tableNumber - 1].entryTotalMinute;

				// �������� ������� �� ������ �����������
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

		// �������� ������� �������
		if (!isValidTimeFormat(event.time))
		{
			file.close();
			return;
		}

		// �������� ������� ������������������ �������
		if (event.time < prevTime && !firstClientBeforeOpening)
		{
			file.close();
			return;
		}

		// �������� ������� ����� �������
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

	cout << endTime << endl; // ����� ������� ����� ������

	// ����� �������
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
