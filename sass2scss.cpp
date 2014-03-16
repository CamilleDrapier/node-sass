// include library
#include <iostream>

// using std::string
using namespace std;

// pretty print option
// set by command line
int pretty = 0;

// whitespace buffer
string whites;

// concatenate comma lists
bool comma = false;

// comment context
string comment = "";

// indent level
int level = 0;

// indent strings
// length should be dynamic
// but my c++ is very rusty
string indents[255];

// helper function
static string closer ()
{
	return pretty > 0 ? "\n" + indents[level] + "}" : " }";
}

// helper function
static string opener ()
{
	return pretty > 1 ? "\n" + indents[level] + "{" : " {";
}

// flush whitespace and
// print additional text
// but only print additional
// chars and buffer whitespace
// ***************************************************************************************
void flush (ostream& os, string& str)
{
	// print whitespace
	os << whites; whites = "";
	// remove newlines
	int pos = str.find_last_not_of("\n\r");
	string lfs = str.substr(pos + 1);
	str = str.substr(0, pos + 1);
	// getline discharged newline
	whites = whites + lfs + "\n";
	// print text
	os << str;
}

// process a line of the sass text
void process (ostream& os, string& str)
{

	// get postion of first meaningfull char
	int pos_left = str.find_first_not_of(" \t\n\v\f\r");

	// has only whitespace
	if (pos_left == string::npos)
	{
		// add whitespace
		whites += str;
	}
	// have meaningfull first char
	else
	{

		// store indentation string
		string indent = str.substr(0, pos_left);

		// is a one-line comment
		if (comment == "//")
		{
				// close comment
				os << " */";
				// unset flase
				comment = "";
		}
		// special case for multiline comment, when the next
		// line is on the same indentation as the actual comment
		// I assume that this means we should close the comment node
		else if (comment == "/*" && indent.length() == indents[level].length())
		{
			// close comment
			os << " */";
			// unset flag
			comment = "";
		}
		// current line has less or same indentation
		else if (indent.length() <= indents[level].length())
		{
			// add semicolon if not in concat mode
			if (comment == "" && !comma) os << ";";
		}

		// make sure we close every "higher" block
		while (indent.length() < indents[level].length())
		{
			// close block (lower level)
			indents[level] = ""; level --;
			// print closer
			if (comment == "")
			{ os << " */"; }
			else { os << closer(); }
			// reset comment
			comment = "";
		}

		// current line has more indentation
		if (indent.length() > indents[level].length())
		{
			// not in comment mode
			if (comment == "")
			{
				// print block opener
				os << opener();
				// open new block
				level ++;
				// store block indentation
				indents[level] = indent;
			}
			// open new block if comment is opening
			// be smart and only require the same indentation
			// level as the comment node itself, plus one char
			if (comment == "/*")
			{
				// open new block
				level ++;
				// only increase indentation minimaly
				// this will accept everything that is more
				// indented than the opening comment line
				indents[level] = indents[level - 1] + ' ';
			}
			// set comment to current indent
			// multiline comments must be indented
			// indicates multiline if not eq "*"
			if (comment != "") comment = indent;
		}

		// check if current line starts a comment
		string opener = str.substr(pos_left, 2);
		if (opener == "/*" || opener == "//")
		{
			// force single line comments
			// into a correct css comment
			str[pos_left + 1] = '*';
			// close previous comment
			if (comment != "") os << " */";;
			// remove indentation from previous comment
			if (comment == "//")
			{
				// reset string
				indents[level] == "";
				// close block
				level --;
			}
			// set comment flag
			comment = opener;
		}

		// flush line
		flush(os, str);

		// get postion of last meaningfull char
		int pos_right = str.find_last_not_of(" \t\n\v\f\r");

		// check for invalid result
		if (pos_right != string::npos)
		{

			// get the last meaningfull char
			string closer = str.substr(pos_right, 1);

			// check if next line should be concatenated
			if (closer == ",") comma = true;

			// check if we have more than
			// one meaningfull char
			if (pos_right > 0)
			{

				// get the last two chars from string
				string closer = str.substr(pos_right - 1, 2);
				// is comment node closed expicitly
				if (closer == "*/")
				{
					// close implicit comment
					comment = "";
				// reset string
				indents[level] == "";
				// close block
				level --;
				}
			}
			// EO have meaningfull chars from end

		}

	}
	// EO have meaningfull chars from start

}

int main (int argc, char** argv)
{

	// process command line arguments
	for (int i = 0; i < argc; ++i)
	{
		// only handle prettyfying option
		if ("--pretty" == string(argv[i])) pretty ++;
	}

	// declare variable
	string line;

	// init variables
	whites = "";
	indents[0] = "";

	// read from stdin
	while (cin)
	{
		// read a line
		getline(cin, line);
		// process the line
		process(cout, line);
		// break if at end of file
		if (cin.eof()) break;
	};

	// flush buffer
	string end = "";
	flush(cout, end);

}
