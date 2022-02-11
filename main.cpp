#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Date;
class Database;

Date ParseDate(const string&); //Обработка даты из входного потокаx
bool operator<(const Date&, const Date&);//Перезагрузка оператора сравнения для сравнения дат
ostream& operator<<(ostream&, const Date&); //Перезагрузка оператора вывода для даты
ofstream& operator<<(ofstream&, const Database&); //Перезагрузка для чтения БД из файла


// Класс для хранения даты и обработки основных логических ошибок (количество месяцев в году и дней в месяце)
class Date {
public:
    Date(int new_year, int new_month, int new_day) {

        if (new_month > 12 || new_month < 1) {
            throw logic_error("Month value is invalid: " + to_string(new_month));
        }
        if (new_day > 31 || new_day < 1) {
            throw logic_error("Day value is invalid: " + to_string(new_day));
        }

        year = new_year;
        month = new_month;
        day = new_day;
    }

    int GetYear() const {
        return year;
    }
    int GetMonth() const {
        return month;
    }
    int GetDay() const {
        return day;
    }

private:
    int year;
    int month;
    int day;
};


//Класс для организации и управления простой БД
class Database {
public:

    //Добавление нового элемента в БД
    void AddEvent(stringstream& str) {

        string date_str, event;
        str >> date_str;
        const Date date = ParseDate(date_str);

        while (!str.eof()) {
            string n;
            str >> n;
            event += n + " ";
        }

        storage[date].insert(event);
    }

    //Удаление только события из БД, если такое имеется
    //Возврат True при удалении
    bool DeleteEvent(const Date& date, const string& event) {

        if (storage.count(date) > 0 && storage[date].count(event) > 0) {
            storage[date].erase(event);
            return true;
        }
        return false;
    }

    //Удаление даты вместе со всеми её событиями
    //Возврат количества удаленных событий
    int DeleteDate(const Date& date) {

        if (storage.count(date) == 0) {
            return 0;
        } else {
            const int event_count = storage[date].size();
            storage.erase(date);
            return event_count;
        }
    }

    //Поиск событий по дате
    set<string> Find(const Date& date) const {

        if (storage.count(date) > 0) {
            return storage.at(date);
        } else {
            return {};
        }
    }

    //Печать всей БД
    void Print() const {

        for (const auto& item : storage) {
            for (const string& event : item.second) {
                cout << item.first << " " << event << endl;
            }
        }

        if (storage.empty()) {
            cout << "Database is empty" << endl;
        }

    }

    //Удалить всё из БД
    void DeleteAll() {
        storage.clear();
    }

    friend ofstream& operator<<(ofstream&, const Database&);

private:
    map<Date, set<string>> storage;
};


int main() {

    Database db;

    cout << "Please, enter a command: Add, Del, Find, Print, Save, Read, Clean, End" << endl <<
         "Save, Read, Clear - these are commands for working with a file" << endl <<
         "Special format for input:" << endl <<
         "Add 0000-01-01 Event" << endl <<
         "Del all, Del 0000-01-01 or Del 0000-01-01 Event" << endl <<
         "Find 0000-01-01" << endl;

    string command_line;
    while (getline(cin, command_line)) {

        try {

            stringstream ss(command_line);
            string command;
            ss >> command;

            if (command == "Add") { //Добавление события

                db.AddEvent(ss);

            } else if (command == "Del") { //Удаление события

                string date_str;
                ss >> date_str;
                if (date_str == "all") {
                    db.DeleteAll();
                    cout << "The database has been delete" << endl;
                    continue;
                }

                string event;
                if (!ss.eof()) { //Проверка на наличие события
                    ss >> event;
                }

                const Date date = ParseDate(date_str);

                if (event.empty()) { //Удаляем все события, если указана только дата
                    const int count = db.DeleteDate(date);
                    cout << "Deleted " << count << " events" << endl;
                } else {
                    if (db.DeleteEvent(date, event)) {
                        cout << "Deleted successfully" << endl;
                    } else {
                        cout << "Event not found" << endl;
                    }
                }

            } else if (command == "Find") { //Поиск событий по дате

                string date_str;
                ss >> date_str;
                const Date date = ParseDate(date_str);
                for (const string &event: db.Find(date)) {
                    cout << event << endl;
                }

            } else if (command == "Print") { //Вывод БД

                db.Print();

            } else if (command == "Save") { //Сохранение БД на ЖД

                ofstream fout("save_bd.txt", ios::app);
                if(!fout) {
                    throw logic_error("The file if not open");
                }
                fout << db;
                fout.close();
                cout << "Saving completed" << endl;

            } else if (command == "Read") { //Считывание сохраненных ранее событий

                ifstream inf("save_bd.txt");
                if(!inf) {
                    throw logic_error("The file if not open");
                }

                string line;
                while(getline(inf, line)) {
                    stringstream str(line);
                    cout << line << endl;
                    db.AddEvent(str);
                }
                inf.close();

            } else if (command == "Clear"){ //Очистка файла с БД

                ofstream fout("save_bd.txt");
                if(!fout) {
                    throw logic_error("The file if not open");
                }
                fout.close();
                cout << "The file is cleared" << endl;

            } else if (command == "End") { //Завершение программы

                    cout << "Goodbye!";
                    break;

            } else if (!command.empty()) {

                throw logic_error("Unknown command: " + command);

            }
        }
        catch (const exception &e) {
            cout << e.what() << endl;
        }
    }

    return 0;
}

//Обработка даты из входного потока
//Вывод ошибки при некорректном вводе
Date ParseDate(const string& date) {
    istringstream date_stream(date);
    bool ok = true;

    int year;
    ok = ok && (date_stream >> year);
    ok = ok && (date_stream.peek() == '-');
    date_stream.ignore(1);

    int month;
    ok = ok && (date_stream >> month);
    ok = ok && (date_stream.peek() == '-');
    date_stream.ignore(1);

    int day;
    ok = ok && (date_stream >> day);
    ok = ok && date_stream.eof();

    if (!ok) {
        throw logic_error("Wrong date format: " + date);
    }
    return Date(year, month, day);
}

//Перезагрузка оператора сравнения для сравнения дат
bool operator<(const Date& lhs, const Date& rhs) {
    return vector<int>{lhs.GetYear(), lhs.GetMonth(), lhs.GetDay()} <
           vector<int>{rhs.GetYear(), rhs.GetMonth(), rhs.GetDay()};
}

//Перезагрузка оператора вывода для даты
ostream& operator<<(ostream& stream, const Date& date) {
    stream << setw(4) << setfill('0') << date.GetYear() << "-"
            << setw(2) << setfill('0') << date.GetMonth() << "-"
            << setw(2) << setfill('0') << date.GetDay();
    return stream;
}
//Перезагрузка для чтения БД из файла
ofstream& operator<<(ofstream& stream, const Database& db)  {
    for (const auto& item : db.storage) {
        for (const string& event : item.second) {
            stream << item.first << " " << event << endl;
        }
    }
    return stream;
}