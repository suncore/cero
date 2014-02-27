
#include "stdincs.h"

#define LINUX
#ifdef LINUX

#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
        

        

int syscall(string dir, string s)
{
    int r;
    if (dir != ".")
        s = "( cd " + dir + "; " + s + " )";
    cout << "syscall: " << s << "\n";
    r = system(s.c_str());
    if (r == -1)
    {
        perror("Could not execute");
        exit(1);
    }
    return r;
}

static time_t file_mtime(const char* p, bool& ok)
{
    struct stat attrib;                     // create a file attribute structure
    int r = stat(p, &attrib);
    ok = false;
    if (r == 0) ok = true;
    return attrib.st_mtime;
}


bool file_is_newer(string newfile, string oldfile)
{
    bool ok1, ok2;
    auto t1 = file_mtime(newfile.c_str(), ok1);
    auto t2 = file_mtime(oldfile.c_str(), ok2);
    if (!(ok1 && ok2)) return true;
    //cout << "file_is_newer: " + newfile + " " + oldfile + ": " << (t1 > t2) << "\n";
    return t1 > t2;
}

#endif

#ifdef WINDOWS

#endif
