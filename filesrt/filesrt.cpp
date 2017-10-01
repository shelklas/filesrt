#include <iostream>
#include <regex>
#include <filesystem>
#include <vector>
#include <fstream>

using namespace std;
using namespace std::experimental::filesystem;

class SearchArg
{
public:
	string m_name;
	regex m_regex;
	vector<path> m_path;
	SearchArg(string name, regex regex) : m_name(name), m_regex(regex) {}
};
bool notWhiteListedFile(directory_iterator & d, vector<string>& whitelist)
{
	for (vector<string>::iterator itr = whitelist.begin(); itr != whitelist.end(); ++itr) // Don't add our sorted folder
		if (d->path().filename().string() == *itr)
			return false;
	return true;
}
void scan(path const& f, vector<SearchArg>& srch,vector<string>& whitelist)
{
	directory_iterator d(f), e;
	for (; d != e; ++d)
		for (vector<SearchArg>::iterator it = srch.begin(); it != srch.end(); ++it)
			if (regex_match(d->path().extension().string(), it->m_regex))
			{
				if(notWhiteListedFile(d,whitelist))
					it->m_path.push_back(d->path());
				break;
			}
}
void fileSort(vector<SearchArg>& srch,path& currentPath)
{
	if (!is_directory(path(currentPath.string() + ("\\Sorted"))))
		create_directory(path(currentPath.string() + ("\\Sorted")));
	for (vector<SearchArg>::iterator it = srch.begin(); it != srch.end(); ++it)
		if (it->m_path.size() != 0)
		{
			if (!is_directory(path(currentPath.string() + ("\\Sorted\\") + it->m_name)))
				create_directory(path(currentPath.string() +("\\Sorted\\") + it->m_name));
			for (vector<path>::iterator itr = it->m_path.begin(); itr != it->m_path.end(); ++itr)
			{
				path changePath = path(currentPath.string() + ("\\Sorted\\" + it->m_name + "\\" + itr->filename().string()));
				if (exists(changePath))
				{
					int count = 0;
					while (exists(path(currentPath.string() + "\\Sorted\\" + it->m_name + "\\" + changePath.stem().string() + "_" + to_string(count) + changePath.extension().string()))) // adding int to end to make unique
						count++;
					changePath = path(currentPath.string() + "\\Sorted\\" + it->m_name + "\\" + changePath.stem().string() + "_" + to_string(count) + changePath.extension().string());
				}
				try
				{
					rename(*itr, changePath);
				}
				catch (filesystem_error& e)
				{
					cout <<changePath.string() << endl;
					cout << e.what() << endl;
				}
			}
		}
}
int main()
{
	regex photoExt("(.png|.raw|.jpg|.tif)",regex::icase);
	regex databaseExt("(.accdb)", regex::icase);
	regex documentExt("(.txt|.doc|.docx|.rtf|.pdf|.ppt)", regex::icase);
	regex videoExt("(.mkv|.mov|.mp4|.gif)", regex::icase);
	regex applicationExt("(.exe|.jar)", regex::icase);
	regex codingExt("(.cpp|.c|.java|.html|.css)", regex::icase);
	regex otherExt("(|.*$)");
	path currPath = current_path();
	vector<SearchArg> search;
	vector<string> whitelist;
	string hold;
	fstream whitlistFile("fileSrtWhitelist.txt");
	while (whitlistFile >> hold)
		whitelist.push_back(hold);
	whitlistFile.close();
	search.push_back(SearchArg("Photos", photoExt));
	search.push_back(SearchArg("Databases", databaseExt));
	search.push_back(SearchArg("Documents", documentExt));
	search.push_back(SearchArg("Videos", videoExt));
	search.push_back(SearchArg("Applications", applicationExt));
	search.push_back(SearchArg("Coding", codingExt));
	search.push_back(SearchArg("Other", otherExt));

	scan(currPath, search,whitelist);

	fileSort(search,currPath);
}