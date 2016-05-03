#include "lab7.h"

int main(int argc, char **argv)
{
    //argument parsing
    string command;
    vector<string> args;
    getline(cin, command);
    split(args, command, is_any_of(" "));
    while(!command.empty())
    {
        if(command == "import")
        {
            cout << "importing file " <<  command << " lfs file\n";
            //import(arg1, arg2);                                                 
        }
        getline(cin, command, ' ');
    }
    return 0;
}