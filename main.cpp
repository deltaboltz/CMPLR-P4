/*
 * By: Connor Schultz
 * Date Made: 04/15/2021
 * Files Needed: any file with extension .sp21 & makefile to compile all files
 * Made For: CS-4280 (Program Translation Project) @ UMSL
 * Last Update : 05/05/2021 - Changed readme and target file added
 * TODO : FINISHED
 */

 //NOTE : Test4 had an error to try to call

#include <iostream>
#include <string>
#include <fstream>
#include "parser.h"
#include "node.h"

using namespace std;

int main(int argc, char* argv[])
{

  std::string base;
  node<std::string> root;

  if(argc == 1)
  {
    root = parser(cin);
    base = "kb";
  }
  else if(argc == 2) //check to see if the file has the proper extension
  {
    string fext, filename(""), filearg(argv[1]);
    int length = filearg.length();

    if(length > 4)
    {
      fext = filearg.substr(length-5, length);

      if(fext == ".sp21")
      {
        filename = filearg;
      }
    }

    if(filename == "")
    {
      filename = filearg + ".sp21";
    }

    ifstream fs(filename.c_str());

    if(fs)
    {
      root = parser(fs);
      base = filename.substr(0, filename.length()-5);
    }
    else
    {
      cout << filename << " - CANNOT be opened.";
      exit(1);
    }
  }
  else
  {
    cout << "Wrong number of arguments were given";
    exit(2);
  }

  root.statChecker();

  std::ofstream ofs;
  ofs.open(base + ".asm");
  root.genASM(ofs);
  ofs.close();

  cout << "finished processing: ASM is in: " << base << ".asm" << endl;
  return 0;
}
