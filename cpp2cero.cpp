

#include "stdincs.h"

#define LINELEN 2048

//const int nParenKeys = 4;
static const string parenKeys[] = { " for ", " while ", " else if ", " if ", " switch " };

enum lineType
{
    lt_whitespace,
    lt_preprocessor,
    lt_normal,
    lt_cont
};

struct line
{
    string s;
    int indent;
    lineType lt;
};

static vector<line> input;

static bool
isWhiteSpaceOnly(string s)
{
    int len = s.length();
    int i;
    int nonWhites = 0;
    
    for (i = 0; i < len; i++)
        if (!(s[i] == ' ' || s[i] == '\t' || s[i] == '\n'))
            nonWhites++;
    return nonWhites == 0;
}

static bool
toBeContinued(string s)
{
    int len = s.length();
    int i;
    bool comma = false;
    
    for (i = 0; i < len; i++)
    {
        if (s[i] == ',' || s[i] == '&' || s[i] == '|' || s[i] == '=')
            comma = true;
        else if (!(s[i] == ' ' || s[i] == '\t' || s[i] == '\n'))
            comma = false;
    }
    return comma;
}

static int
getIndent(string s)
{
    int i;
    int whites = 0;

    for (i = 0; i < (int)s.length(); i++)
    {
        if (s[i] == ' ')
            whites++;
        else if (s[i] == '\t')
            whites += 8; // TODO configurable
        else
            return whites;
    }
    return whites;
}

static bool
isDeletableLine(string s)
{
    int i;
    int curlies = 0;
    int nonWhites = 0;
    
    for (i = 0; i < (int)s.length(); i++)
        if (s[i] == '{' || s[i] == '}' || s[i] == ';')
            curlies++;
        else if (!(s[i] == ' ' || s[i] == '\t' || s[i] == '\n'))
            nonWhites++;
    
    return curlies >= 1 && nonWhites == 0;
}

static void
removeSemicolon(string& s)
{
    s32 len = s.length();
    s32 i;

    for (i = len-1; i >= 0; i--)
    {
        if (s[i] == ')')
            return;

        if (s[i] == ';')
        {
            s.erase(i, 1);
            return;
        }
    }
}




void cpp2cero(string infilename, string outfilename)
{
    int i;
    //string s;
    ifstream infile;
    ofstream outfile;
    
    infile.open(infilename);
    if (!infile.good())
    {
        perror("Can not open input file\n");
        bail();
    }
    
    outfile.open(outfilename, ios::trunc);
    if (!outfile.good())
    {
        perror("Can not open output file\n");
        bail();
    }

    // read file
    while (1)
    {
        line l;
        string s;
        bool x = getline(infile, s);
        if (!x)
            break;
        l.s = s;
        input.push_back(l);
    }

    // calculate indents and line types
    int lineNum, indent;
    lineNum = 0;
    indent = -1; // multi line indent
    for (i = 0; i < (int)input.size(); i++)
    {
        line* l = &input[i];
        l->lt = lt_normal;
        if (l->s[0] == '#')  // TODO can be preceded by whitespace
            l->lt = lt_preprocessor;
        else if (isWhiteSpaceOnly(l->s))
            l->lt = lt_whitespace;
        else if (toBeContinued(l->s))
        {
            l->lt = lt_cont;
            if (indent == -1)
                indent = getIndent(l->s);
        }
        else
        {
            if (indent != -1)
                l->indent = indent;
            else
                l->indent = getIndent(l->s);
            indent = -1;
        }
        lineNum++;
    }

    for (i = 0; i < (int)input.size(); i++)
    {
        line& l = input[i];
        removeSemicolon(l.s);

        // clean up parentheses
        if (l.lt == lt_normal)
            for (auto key : parenKeys)
            {
                //printf("hej2 %s, %s\n", l.s.c_str(), key.c_str());
                auto r1 = l.s.find(key); // TODO check only whitespace precedes
                if (r1 != string::npos)
                {
                    //printf("hej %s\n", l.s.c_str());
                    auto r2 = l.s.find("(");
                    auto r3 = l.s.rfind(")");
                    if (r2 == string::npos || r3 == string::npos) continue;
                    l.s.erase(r2,1);
                    l.s.erase(r3-1,1);
                }
            }
    }
    
    for (auto l2 : input)
    {
        if (!isDeletableLine(l2.s))
            outfile << l2.s + "\n";
    }

    outfile.close();
    infile.close();
    
}        
