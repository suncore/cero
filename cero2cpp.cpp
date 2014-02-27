

#include "stdincs.h"

#define LINELEN 2048

//const int nParenKeys = 4;
static const string parenKeys[] = { " for ", " while ", " else if ", " if ", " switch " };

enum lineType
{
    lt_normal,
    lt_whitespace,
    lt_preprocessor,
    lt_label,
    lt_separator,
    lt_cont
};

struct line
{
    string s;
    int indent;
    lineType lt;
};


static bool
isCharWhite(char c)
{
    return c == ' ' || c == '\t' || c == '\n';
}

static void
removeWhites(string& s)
{
    string s2 = "";
    for (auto c : s)
    {
        if (!isCharWhite(c))
            s2 += c;
            
    }
    s = s2;
}

static bool
isWhiteSpaceOnly(string s)
{
    int len = s.length();
    int i;
    int nonWhites = 0;
    
    for (i = 0; i < len; i++)
        if (!isCharWhite(s[i]))
            nonWhites++;
    return nonWhites == 0;
}


static bool
isLabel(string s)
{
    int len = s.length();
    int i;
    
    for (i = len-1; i >= 0; i--)
        if (s[i] == ':')
            return true;
        else if (!isCharWhite(s[i]))
            return false;
    return false;
}


static bool
isKey(string s)
{
    int b, i;

    for (auto key : parenKeys)
    {
        auto r = s.find(key);
        if (r != string::npos)
        {
            b = 1;
            for (i = 0; i < (int)r; i++)
                b &= isCharWhite(s[i]);
            if (b)
                return true;
        }
    }
    return false;
}

static bool
toBeContinued(string s)
{
    int len = s.length();
    int i;
    bool cont = false;

    if (isKey(s))
        return false;
    
    for (i = 0; i < len; i++)
    {
        if (s[i] == ',' || s[i] == '&' || s[i] == '|' || s[i] == '=' || s[i] == '>' || s[i] == '<' || s[i] == '*' || s[i] == '/')
            cont = true;
        else if (s[i] == '+' && i > 0 && s[i-1] != '+')
            cont = true;
        else if (s[i] == '-' && i > 0 && s[i-1] != '-')
            cont = true;
        else if (!isCharWhite(s[i]))
            cont = false;
    }
    return cont;
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

void cero2cpp(vector<string>& inv)
{
    int i,j;
    int tabSize = 4;
    vector<line> input;


    // Insert last line to aid finalizing placement of curly braces. This line will be removed before output.
    inv.push_back(";");

    for (auto s : inv)
    {
        line l;
        l.s = s;
        l.indent = 0;
        l.lt = lt_normal;
        input.push_back(l);
    }

    {
        // Remove all comments
        int ltot = (int)input.size();
        for (i = 0; i < ltot; i++)
        {
            line* l = &input[i];        
            auto r1 = l->s.find("//");
            if (r1 != string::npos)
                l->s.erase(r1, string::npos);
            r1 = l->s.find("/*");
            if (r1 != string::npos)
            {
                auto r2 = l->s.find("*/", r1);
                if (r2 == string::npos)
                {
                    for (; i < ltot; i++)
                    {
                        line* l = &input[i];        
                        r2 = l->s.find("*/", r1);
                        if (r2 == string::npos)
                        {
                            l->s.erase(r1, r2-r1);
                            r1 = 0;
                        }
                        else
                        {
                            l->s.erase(r1, r2+2-r1);
                            break;
                        }
                    }
                }
                else
                    l->s.erase(r1, r2-r1+2);
            }
        }
    }
    
    {
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
            else if (isLabel(l->s))
                l->lt = lt_label;
            else if (isWhiteSpaceOnly(l->s))
                l->lt = lt_whitespace;
            else if (toBeContinued(l->s))
            {
                l->lt = lt_cont;
                if (indent == -1)
                    indent = getIndent(l->s);
                l->indent = indent;
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
    }
    
    {
        // insert parentheses around for, while, if, ... testcase
        for (i = 0; i < (int)input.size(); i++)
        {
            line& l = input[i];
            if (l.lt != lt_normal)
                continue;

            int b;

            for (auto key : parenKeys)
            {
                auto r1 = l.s.find(key);
                if (r1 != string::npos)
                {
                    b = 1;
                    for (int j = 0; j < (int)r1; j++)
                        b &= isCharWhite(l.s[j]);
                    if (b)
                    {
                        if (key == " for " && l.s.find("..") != string::npos)
                        {
                            string s = l.s.substr(r1 + key.length()-1);
                            removeWhites(s);
                            bool isAuto = false;
                            string var, start, end;
                            var = start = end = "";
                            int state = 0;
                            // expand shorthand "for i=0..5" or "for i := 0..5" to "for i = 0; i < 5; i++"
                            for (int j = 0; j < (int)s.size(); j++)
                            {
                                if (state == 0) // waiting for end of variable
                                {
                                    if (s[j] == ':' && s[j+1] == '=')
                                    {
                                        j++; // eat '='
                                        isAuto = true;
                                        state = 1;
                                    }
                                    else if (s[j] == '=')
                                    {
                                        state = 1;
                                    }
                                    else
                                    {
                                        var += s[j];
                                    }
                                }
                                else if (state == 1) // waiting for end of start expr
                                {
                                    if (s[j] == '.' && s[j+1] == '.')
                                    {
                                        j++;
                                        state = 2; // skip directly to start of end expr
                                    }
                                    else
                                        start += s[j];
                                }
                                else if (state == 2) // waiting for end of end expr
                                {
                                    end += s[j];
                                }
                            }
                            l.s.erase(r1 + key.length());
                            l.s += (isAuto ? "auto " : "") + var + "=" + start + ";" + var + "<" + end + ";" + var + "++";
                        }
                        l.s.insert(r1 + key.length(), "(");
                        l.s += ")";
                    }
                }
            }
        }
    }
    
    {
        // insert semicolons
        for (i = 0; i < (int)input.size(); i++)
        {
            line& l = input[i];
            
            if (l.lt == lt_normal)
            {
                line* n = NULL;
                // search for next normal line
                for (j = i+1; j < (int)input.size(); j++)
                {
                    n = &input[j];
                    if (n->lt == lt_normal)
                        break;
                }
                if (n && (n->indent <= l.indent))
                    l.s += ";";
            }
        }
    }
    

    
    {
        // change := to auto
        for (i = 0; i < (int)input.size(); i++)
        {
            line& l = input[i];
            auto r1 = l.s.find(":=");
            if (r1 != string::npos)
            {
                l.s.erase(r1, 1);
                l.s = " auto " + l.s;
            }
        }
    }
    
    
    {
        // insert curly braces
        int semiColonNeeded = false;
        string semis[] = { "struct ", "class ", "enum ", "union " };
        list<bool> semiStack;
        for (i = 0; i < (int)input.size(); i++)
        {
            line& l = input[i];

            // Curly brace insertion rules
            //     If follwing line is indented to right, place { immediately before following line
            //     If follwing line is indented to left, place } immediately after line

            semiColonNeeded = false;
            if (l.lt == lt_normal || l.lt == lt_cont)
            {
                for (auto f : semis)
                {
                    if (l.s.find(f) != string::npos)
                    {
                        semiColonNeeded = true;
                    }
                }
            }

            if (l.lt == lt_normal)
            {
                line* n = NULL; 
                // search for next normal line
                for (j = i+1; j < (int)input.size(); j++)
                {
                    n = &input[j];
                    if (n->lt == lt_normal || n->lt == lt_cont)
                        break;
                }
                if (n && (n->indent > l.indent))
                {
                    if (semiColonNeeded)
                        semiStack.push_front(true);
                    else
                        semiStack.push_front(false);
                    l.s = l.s + "{";
                    //cout << "open:" + l.s + "\n";
                }
                if (n && (n->indent < l.indent))
                {
                    int steps = (l.indent - n->indent) / tabSize;
                    for (int step = 0; step < steps; step++)
                    {
                        l.s = l.s + "}";
                        //cout << "close:" + l.s + "\n";
                        // ending curly sometimes needs a semicolon
                        if (semiStack.size() == 0)
                        {
                            printf("Indentation error at or before line %d : %s\n", i, l.s.c_str());
                            printf("Possibly this is due to an unsupported line break in a statement at or before the above line. See guidelines on which line breaks inside statements are allowed in cero.\n");
                            exit(1);
                        }
                        bool& r = semiStack.front();
                        semiStack.pop_front();
                        if (r)
                            l.s = l.s + ";";
                    }
                }
            }
        }
    }

    if (input.size() > 0)
        input.pop_back(); // remove semicolon
    inv.clear();    
    for (auto l : input)
    {
        inv.push_back(l.s);
    }
}

