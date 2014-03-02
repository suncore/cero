
#include "stdincs.h"
#include "cero2cpp.h"
#include "platform.h"
#include "build.h"



static string cflags="-Wall -O3 -std=c++11 -c";
static string lflags="";
static string exe="a.out";
static string cc="g++";
static string objs = " ";
static string path_env = ".";
static vector<string> paths;
struct package
{
    string name;
    bool rebuilt;
};
static vector<package> packages;


static string open_ce_file(ifstream& f, string name)
{
    for (auto p: paths)
    {
        string n = p + "/" + name;
        f.open(n);
        if (f.good()) return p + "/" + name;
        if (f.is_open()) f.close();
    }
    cout << "Can not open input file " << name << ". Search path : " + path_env + "\n";
    bail();
    return "";
}


static void open_outfile(ofstream& f, string name)
{
    f.open(name, ios::trunc);
    if (!f.good())
    {
        cout << "Can not open output file " << name << "\n";
        bail();
    }
}

// returns true if rebuild happened (or one of the children rebuilt)
static bool compile(string name, bool build_all) 
{
    bool dep_rebuilt = false;
    bool rebuilt = false;
    bool found = false;
    package me;
    me.name = name;
    me.rebuilt = false;
    for (auto p : packages)
        if (p.name == name)
            return p.rebuilt;
    packages.push_back(me);
    
    //cout << "compile " << name + "\n";

    int i;
    ifstream infile;
    
    auto r = name.rfind(".");
    if (r != string::npos)
        name.erase(r, string::npos);
    string outfilename_cpp = name + ".cpp";
    string outfilename_h = name + ".h";

    string infilename = open_ce_file(infile, name+".ce");

    vector<string> pub;
    vector<string> priv;
    vector<string>* input;

    input = &priv; // default
    // read file
    bool pub_open = false;
    bool priv_open = false;
    while (1)
    {
        string s;
        bool x = getline(infile, s);
        if (!x) 
            break;
        else if (s == "+++ public")
        {
            input = &pub;
            pub.push_back(string(""));
            priv.push_back(string(""));
        }
        else if (s == "+++ private")
        {
            input = &priv;
            pub.push_back(string(""));
            priv.push_back(string(""));
        }
        else 
        {
            auto r = s.find("import ");
            if (r == 0)
            {
                string dep = s.substr(strlen("import "));
                if (dep != name && build_all)
                    dep_rebuilt |= compile(dep, build_all);
                s = "#include \"" + dep + ".h\"";
            }

            if (input == &priv)
            {
                priv_open = true;
                pub.push_back(string(""));
            }
            else if (input == &pub)
            {
                pub_open = true;
                priv.push_back(string(""));
            }
            input->push_back(s);
        }
    }

    infile.close();
    
    vector<string> decl_priv;
    vector<string> decl_pub;

    if (priv_open && !found)
        objs += name + ".o ";
    
    if (priv_open && file_is_newer(infilename, "build/" + outfilename_cpp))
    {
        for (string& s : priv)
        {
            if (s[0] == '+' && s != "+decl")
            {
                s.erase(0,1);
                decl_pub.push_back(s);
            }
            else if (s[0] == '-' && s != "-decl")
            {
                s.erase(0,1);
                s = "static " + s;
                decl_priv.push_back(s);
            }
        }
        for (string& s : priv)
        {
            if (s == "-decl")
            {
                s = "#include \"" + outfilename_cpp + ".-decl\"";
                cero2cpp(decl_priv);
                ofstream outfile;
                open_outfile(outfile, "build/" + outfilename_cpp + ".-decl");
                for (auto s2 : decl_priv)
                    outfile << s2 + "\n";
                cout << "created build/" + outfilename_cpp + ".-decl\n";
                outfile.close();
                rebuilt = true;
                break;
            }
        }
        priv.push_back(";"); // Insert last line to aid finalizing placement of curly                 if (import.size() > 0)
        cero2cpp(priv);
        ofstream outfile;
        open_outfile(outfile, "build/" + outfilename_cpp);
        for (i = 0; i < (int)priv.size()-1; i++)
        {
            string s = priv[i];
            outfile << s + "\n";
        }
        cout << "created build/" + outfilename_cpp + "\n";
        outfile.close();
        rebuilt = true;
    }

    if (pub_open && file_is_newer(infilename, "build/" + outfilename_h))
    {
        for (string& s : pub)
        {
            if (s == "+decl")
            {
                s = "#include \"" + outfilename_h + ".+decl\"";
                cero2cpp(decl_pub);
                ofstream outfile;
                open_outfile(outfile, "build/" + outfilename_h + ".+decl");
                for (auto s2 : decl_pub)
                    outfile << s2 + "\n";
                cout << "created build/" + outfilename_h + ".-decl\n";
                outfile.close();
                rebuilt = true;
                break;
            }
        }
        pub.push_back(";"); // Insert last line to aid finalizing placement of curly         
        cero2cpp(pub);
        ofstream outfile;
        open_outfile(outfile, "build/" + outfilename_h);
        for (i = 0; i < (int)pub.size()-1; i++)
        {
            string s = pub[i];
            outfile << s + "\n";
        }
        cout << "created build/" + outfilename_h + "\n";
        outfile.close();
        rebuilt = true;
    }

    if (priv_open && (dep_rebuilt || file_is_newer("build/" + outfilename_cpp, "build/" + name+".o")))
    {
        int r2 = syscall("build", cc + " " + cflags + " -c " + name + ".cpp");
        if (r2 != 0)
            exit(1);
    }

    rebuilt |= dep_rebuilt;
    //cout << "compile " + name + " " << rebuilt << "\n";
    for (auto& p : packages)
        if (p.name == name)
            p.rebuilt = rebuilt;
    return rebuilt;
}

void build(string infilename, bool build_all)
{
    ifstream infile;
    string basename = infilename;
    string s;
    
    auto r = basename.rfind(".");
    if (r != string::npos)
        basename.erase(r, string::npos);
    exe = basename;

    char* c;
    c = getenv("CFLAGS");
    if (c) cflags = string(c);
    c = getenv("LFLAGS");
    if (c) lflags = string(c);
    c = getenv("CERO_EXE");
    if (c) exe = string(c);
    c = getenv("CC");
    if (c) cc = string(c);
    c = getenv("CERO_PATH");
    if (c) path_env = string(c);
    
    string t = "";
    for (auto p : path_env)
    {
        if (p == ':')
        {
            paths.push_back(t);
            t = "";
        }
        else
            t += p;
    }
    if (t != "")
        paths.push_back(t);
    
    r = syscall(".", "mkdir -p build");
    compile(infilename, build_all);
    if (build_all)
    {
        r = syscall("build", cc + " -o " + exe + objs + lflags);
        if (r != 0)
            exit(1);
    }
}
