// ConsoleApplication1.cpp: определяет точку входа для консольного приложения.
//

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <io.h>
#include "windows.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#define time 1000
using namespace std;
using namespace boost;
namespace logging = boost::log;
bool FileExists(const char *fname);
///////КЛАССЫ И СТРУКТУРЫ///////
struct Config
{
public:
	string path_to_extractor = "";
	string path_to_game = "";
	string path_to_output = "";
	vector<string> gamedatas_names;
	void read()
	{
		string flag;
		string temp;
		if(FileExists("Config.txt"))
		{
			BOOST_LOG_TRIVIAL(info) << "The Config.txt is found";
			ifstream Config_file("Config.txt");
			getline(Config_file, path_to_extractor);
			getline(Config_file, path_to_game);
			getline(Config_file, path_to_output);
			getline(Config_file, flag);
			BOOST_LOG_TRIVIAL(info) << "Files avalible: ";
			if (flag == "NO")
			{
				BOOST_LOG_TRIVIAL(info) << "The flag in config = NO";
				get_filenames();
			}
			if (flag != "NO" && !Config_file.eof())
			{
				BOOST_LOG_TRIVIAL(info) << "The flag in config = YES";
				while (true)
				{
					if (!Config_file.eof())
					{
						getline(Config_file, temp);
						if (temp[0] != '/')
						{
							BOOST_LOG_TRIVIAL(info) << temp;
							gamedatas_names.push_back(temp);
						}
					}
					else break;
				}
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "The Config.txt is not found!";
		}
	}
private:
	void get_filenames()
	{
		WIN32_FIND_DATA FindFileData;
		HANDLE hf;
		hf = FindFirstFile(path_to_game.c_str(), &FindFileData);
		if (hf != INVALID_HANDLE_VALUE) {
			do {
				if (name_contains(FindFileData.cFileName))
				{
					BOOST_LOG_TRIVIAL(info) << FindFileData.cFileName;
					gamedatas_names.push_back(FindFileData.cFileName);
				}
			} while (FindNextFile(hf, &FindFileData) != 0);
			FindClose(hf);
		}
	}
	bool name_contains(string str)
	{
		if (str.size() == 12)
		{
			typedef vector< string > split_vector_type;
			split_vector_type SplitVec;
			boost::split(SplitVec, str, boost::is_any_of("."), token_compress_on);
			if (SplitVec[0] == "gamedata")
				return true;
			else
				return false;
		}
		return false;
	}
};
///////МЕТОДЫ///////
void init()
{
	logging::core::get()->set_filter
		(
			logging::trivial::severity >= logging::trivial::info
			);
}
void automatic(Config &config)
{
	if (FileExists(config.path_to_extractor.c_str()))
	{
		BOOST_LOG_TRIVIAL(info) << "Распаковываем в: " << config.path_to_output;
		for (size_t i = 0; i < config.gamedatas_names.size(); i++)
		{
			STARTUPINFO cif;
			ZeroMemory(&cif, sizeof(STARTUPINFO));
			PROCESS_INFORMATION pi;
			BOOST_LOG_TRIVIAL(info) << "Распаковываем : " << config.gamedatas_names[i];
			if (CreateProcess(config.path_to_extractor.c_str(), NULL,NULL, NULL, FALSE, NULL, NULL, NULL, &cif, &pi) == TRUE)
			{
				Sleep(time);
				//хендлы
				string path_to_db = config.path_to_game + config.gamedatas_names[i];
				HWND hwnd = FindWindow(NULL, "Select file to unpack...");
				HWND hbnd = FindWindowEx(hwnd, NULL, "Button", "&Открыть");
				HWND hсnd = FindWindowEx(hwnd, NULL, "ComboBoxEx32", "");
				hсnd = FindWindowEx(hсnd, NULL, "ComboBox", NULL);
				hсnd = FindWindowEx(hсnd, NULL, "Edit", NULL);
				#ifdef _DEBUG 
				BOOST_LOG_TRIVIAL(info) << "FindWindow(NULL, 'Select file to unpack...') " << hwnd;
				BOOST_LOG_TRIVIAL(info) << "FindWindow(NULL, 'Button', '&Открыть') " << hbnd;
				BOOST_LOG_TRIVIAL(info) << "FindWindow(NULL, 'Edit', '') " << hсnd;
				#endif
				if ((hwnd != NULL && hbnd != NULL ) && hсnd != NULL)
				{
					//устанавливаем текст
					SendMessage(hсnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR(path_to_db.c_str())));
					//кликаем
					SendMessage(hbnd, WM_LBUTTONDOWN, 0, 0);
					SendMessage(hbnd, WM_LBUTTONUP, 0, 0);
					#ifdef _DEBUG
					BOOST_LOG_TRIVIAL(info) << "Sended";
					#endif
				}
				else
				{
					if (hwnd != NULL)
						BOOST_LOG_TRIVIAL(fatal) << "Window handle is null";
					if (hbnd != NULL)
						BOOST_LOG_TRIVIAL(fatal) << "Button handle is null";
					if (hсnd != NULL)
						BOOST_LOG_TRIVIAL(fatal) << "Combox handle is null";
					break;
				}
				//работа со 2м окном
				Sleep(time);
				hwnd = FindWindow(NULL, "Обзор папок");
				hbnd = FindWindowEx(hwnd, NULL, "Button", "ОК");
				hсnd = FindWindowEx(hwnd, NULL, "Edit", NULL);
				if ((hwnd != NULL && hbnd != NULL) && hсnd != NULL)
				{
					//устанавливаем текст
					SendMessage(hсnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR(config.path_to_output.c_str())));
					//кликаем
					SendMessage(hbnd, WM_LBUTTONDOWN, 0, 0);
					SendMessage(hbnd, WM_LBUTTONUP, 0, 0);
					#ifdef _DEBUG
					BOOST_LOG_TRIVIAL(info) << "Sended";
					#endif
				}
				else
				{
					if (hwnd != NULL)
						BOOST_LOG_TRIVIAL(fatal) << "Window handle is null";
					if (hbnd != NULL)
						BOOST_LOG_TRIVIAL(fatal) << "Button handle is null";
					if (hсnd != NULL)
						BOOST_LOG_TRIVIAL(fatal) << "Combox handle is null";
					break;
				}
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
			else
			{
				BOOST_LOG_TRIVIAL(fatal) << "The extractor process is not started!";
			}
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "The " << config.path_to_extractor << " is not found!";
	}
}
bool FileExists(const char *fname)
{
	return access(fname, 0) != -1;
}
///////ПРЕМЕННЫЕ///////

int main()
{
	init();
	setlocale(LC_ALL, "Rus");
	Config config;
	config.read();
	automatic(config);
	system("pause");
    return 0;
}

