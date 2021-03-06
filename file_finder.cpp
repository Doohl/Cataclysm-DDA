#include "file_finder.h"
#include <cstring>  // for strcmp
#include <stack>    // for stack (obviously)

// FILE I/O
#include <sys/stat.h>
#ifdef _MSC_VER
#include "wdirent.h"
#include <direct.h>
#else
#include <dirent.h>
#endif

/**
    Searches the supplied root directory for the extension provided. Can recursively search downward through the directory tree
    to get all files. It will then return a vector of file paths that fulfill the search requirements.
    USAGE:
    extension should be preceeded by the '.' character to denote it as an extension, if it is not then it will return all files
    and folder names that contain the extension string.
    root_path can point to any folder relative to the execution directory.
        Searching from the execution directory -- Supply "." as the root_path
        Searching from the parent directory of the execution directory -- Supply ".." as the root_path
        Searching child directories within the execution directory -- Supply "<directory path>" or "./<directory path>", both
            will provide correct results.
    recursive_search will:
        if true, continue searching all child directories from the supplied root_path
        if false, search only the supplied root_path, and stop when the directory contents have been worked through.
*/
std::vector<std::string> file_finder::get_files_from_path(std::string extension, std::string root_path, bool recursive_search)
{
    std::vector<std::string> files;

    // test for empty root path
    if (root_path.empty())
    {
        root_path = ".";
    }

    std::stack<std::string> directories;
    directories.push(root_path);
    std::string path = "";

    while (!directories.empty())
    {
        path = directories.top();
        directories.pop();

        DIR *root = opendir(path.c_str());

        if (root)
        {
            struct dirent *root_file;
            struct stat _buff;
            DIR *subdir;

            while ((root_file = readdir(root)))
            {
                // check to see if it is a folder!
                if (stat(root_file->d_name, &_buff) != 0x4)
                {
                    // ignore '.' and '..' folder names, which are current and parent folder relative paths
                    if ((strcmp(root_file->d_name, ".") != 0) && (strcmp(root_file->d_name, "..") != 0))
                    {
                        std::string subpath = path + "/" + root_file->d_name;

                        if (recursive_search)
                        {
                            subdir = opendir(subpath.c_str());
                            if (subdir)
                            {
                                directories.push(subpath);
                            }
                            closedir(subdir);
                        }
                    }
                }
                // check to see if it is a file with the appropriate extension
                std::string tmp = root_file->d_name;
                if (tmp.find(extension.c_str()) != std::string::npos)
                {
                    // file with extension found! add to files list with full path
                    std::string fullpath = path + "/" + tmp;
                    files.push_back(fullpath);
                }
            }
        }
        closedir(root);
    }
    return files;
}
