//Charlie Ang
//CSC 3350 Spring 2016
//April 25, 2016
//Lab 3 MYDIR
//This program produces a directory listing (one filename per line) of all files 
//matching a specified filename pattern.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <WinBase.h>

int main (int argc, char *argv[])
{
	//Filetime variables 
	FILETIME ft;
	FILETIME localft;
	SYSTEMTIME systime;

	//Path variables
	WIN32_FIND_DATA found;					//search buffer
	HANDLE			hFind;					//search handle
	BOOL			fullPath = FALSE;		//full path requested 
	char			path[MAX_PATH] = "";	//specified path
	char			pattern[MAX_PATH];		//search pattern
	char			orgpath[MAX_PATH];

	BOOL			isFile = FALSE;			//if it is a file
	BOOL			isExtension = FALSE;	//if it is an extension


	int i;
	for (i = 1; i < argc; i++)	//Find first filename on line.....
	{
		if (_stricmp(argv[i], "/?") == 0 || _stricmp(argv[i], "-?") == 0 || _stricmp(argv[i], "--help") == 0 ||
			_stricmp(argv[i], "--HELP") == 0)	//Help message request switches 
		{
			printf("\nThis program produces a directory listing of all files matching a specified filename pattern.\n");
			exit(1);	//exit code to terminate
		}
	}

	for (i = 1; i < argc; i++)
	{
		if (_stricmp(argv[i], "-L") == 0 || _stricmp(argv[i], "-l") == 0)	//if user enters l or L for long listing 
		{
			fullPath = TRUE;	//full directory listing
		}
		else
		{
			strcpy_s(path, MAX_PATH, argv[i]);	//save path specified
		}
	}
	
	//Check to see if user-specified path exists
	GetCurrentDirectory(MAX_PATH, orgpath);

	if ((strcmp(path, "") != 0) && !SetCurrentDirectory(path))
	{
		isFile = TRUE;	//if path exists, it is file
		//printf("[Usage] Invalid <subdirectory path> specified\n");
		//exit(1);
	}

	//Obtain full path if requested
	if (fullPath)
	{
		GetCurrentDirectory(MAX_PATH, path);
	}

	SetCurrentDirectory(orgpath);	//restore original path

	//Add \ to end of path, if necessary
	if (strcmp(path, "") != 0 &&			//path specified
		path[strlen(path) - 1] != ':' &&	//not just drive letter
		path[strlen(path) - 1] != '\\')		//doesn't end in "\"
	{
		if (!isFile)	//if not a file...concatenate
		{
			strcat_s(path, MAX_PATH, "\\");	//add \ at end 
		}	
	}

	if (isFile)	//if it is a file
	{
		strcpy_s(pattern, MAX_PATH, orgpath);	//copy path into pattern 

		strcat_s(pattern, MAX_PATH, "\\");	//search for all files

		for (int i = 0; i < strlen(path); i++)
		{
			if (path[i] == '.')
			{
				isExtension = TRUE;	//there is an extension
			}
		}
		
		if (!isExtension)
		{
			strcat_s(path, MAX_PATH, "*.*");
		}

		strcpy_s(pattern, MAX_PATH, path);	//copy path into pattern 
		strcat_s(path, MAX_PATH, "*.*");	//search for all files..........changed
	}
	else
	{
		strcpy_s(pattern, MAX_PATH, path);	//copy path into pattern 
		strcat_s(pattern, MAX_PATH, "*.*");	//search for all files
	}


	hFind = FindFirstFile(pattern, &found);
	if (hFind != INVALID_HANDLE_VALUE)	//found a first file
	{
		do
		{
			if ((strcmp(found.cFileName, ".") != 0) &&
				(strcmp(found.cFileName, "..") != 0))
			{
				if (fullPath == TRUE)	//print out times for long listing 
				{
					ft = found.ftLastWriteTime;
					FileTimeToLocalFileTime(&ft, &localft);

					FileTimeToSystemTime(&localft, &systime);
					printf("%d-%d-%d ", systime.wYear, systime.wMonth, systime.wDay);
					printf("%d:%d		", systime.wHour, systime.wMinute);

					//file size
					int fileSizeHigh;
					int fileSizeLow;
					DWORD64 fileSize;
					fileSizeHigh = found.nFileSizeHigh;
					fileSizeLow = found.nFileSizeLow;
						fileSize = fileSizeHigh + fileSizeLow;
					
					printf("%d	", fileSize);

					//Print out attributes.......
					DWORD attr;
					char attributes[5];	//ASHR + null

					//attr = GetFileAttributes(&found);	//Fetch existing attributes
					attr = found.dwFileAttributes;

					for (int i = 0; i < 5; i++)
					{
						attributes[i] = NULL;
					}
						if (attr & FILE_ATTRIBUTE_READONLY)	//check specific bit
						{
							attributes[3] = 'R';	//set read only bit in code 
						}

						if (attr & FILE_ATTRIBUTE_HIDDEN)
						{
							attributes[2] = 'H';
						}

						if (attr & FILE_ATTRIBUTE_SYSTEM)
						{
							attributes[1] = 'S';
						}

						if (attr & FILE_ATTRIBUTE_ARCHIVE)
						{
							attributes[0] = 'A';
						}
						attributes[4] = 0;	//append null

						printf("[%c%c%c%c]	", attributes[0], attributes[1], attributes[2], attributes[3]);	//print out ASHR 
				}
				printf("%s\n", found.cFileName);
				//printf("%s%s\n", path, found.cFileName);
			} 
		}
		while (FindNextFile(hFind, &found));
		FindClose(hFind);
	}
	return(0);
}