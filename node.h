
#ifndef NODE_H
#define NODE_H
#include <set>
#include <vector>
#include "token.h"
#include "stack.h"

template <class T>
class node {
  private:
      std::vector<node<T>> children_;
      std::vector<token> tokens_;

      T key_;
      void preHelper(std::ostream& out, std::string indent);

      void statChecker(stack<std::string, int> &stat, int scope);

      void genASM(std::ostream& out, int scope, std::set<std::string>& varset, stack<std::string, int>& stackvars, std::string& labelctr);

      void genChildASM(std::ostream& out, int scope, std::set<std::string>& varset, stack<std::string, int>& stackvars, std::string& labelctr);

      void getNextLabelString(std::string& labelctr);

      void setR0Call(std::ostream& out, int scope, std::set<std::string>& varset, stack<std::string, int>& stackvars, std::string& labelctr, std::string& endLabel);

  public:
      // AST functionality
      void insert(node<T> child);
      void insert(token childToken);
      node<T> (T key);
      node<T> ();

      // traversal functionality
      void preOrder(std::ostream& out);
      void preOrder();
      std::string toString();

      //Enables Static Semantics
      void statChecker();

      void genASM(std::ostream& out);
};

template <class T>
node<T>::node()
{

}

template <class T>
node<T>::node(T key)
{
  key_ = key; // Initialize the node name
}

template <class T>
void node<T>::insert(node<T> child)
{
  children_.push_back(child);
}

template <class T>
void node<T>::insert(token childToken)
{
  tokens_.push_back(childToken);
}

template <class T>
std::string node<T>::toString()
{
if (tokens_.size() > 0)
  {
    std::string tokenStr("");
    int i = 0;
    while (i < tokens_.size()-1)
    {
      tokenStr += tokens_[i].toString() + ", ";
      i++;
    }
    tokenStr += tokens_[i].toString() + "\n";
    return key_ + "  Tokens: " + tokenStr;
  }
  return key_ + "\n";
}

template <class T>
void node<T>::preHelper(std::ostream& out, std::string indent)
{
  // helper for preOrder
  std::string newdent("  ");
  newdent += indent;
  out << indent << toString();
  for (int i = 0; i < children_.size(); i++)
  {
      children_[i].preHelper(out, newdent);
  }
}

// traversal methods with variable ostream
template <class T>
void node<T>::preOrder(std::ostream& out)
{
  preHelper(out, "");
}

// traversal methods with default std::cout stream
template <class T>
void node<T>::preOrder()
{
  preHelper(std::cout, "");
}

template <class T>
void node<T>::statChecker()
{
  stack<std::string, int> s;

  statChecker(s, 0);
}

template <class T>
void node<T>::statChecker(stack<std::string, int> &stat, int scope)
{
  //std::cout << "In statChecker for node: " << key_ << "\n"; //debug
  if(key_ == "<vars>" && tokens_.size())
  {
    //std::cout << "token value is: " << tokens_[1].instance << "\n"; //debug
    std::string k = tokens_[1].instance;
    //std::cout << "k value is: " << k << "\n"; //debug

    if(stat.containsKey(k, scope))
    {
      std::cout << "SEMANTICS ERR\n";
      std::cout << "Line " << tokens_[1].posString() << " \"";
      std::cout << tokens_[1].instance;
      std::cout << "\" has already been defined in this scope\n";

      exit(-3);
    }

    stat.push(k, scope);

  }
  else if(key_ == "<block>")
  {
    for(int i = 0; i < children_.size(); i++)
    {
      children_[i].statChecker(stat, scope + 1);
    }

    stat.popScope(scope + 1);
    return;
  }
  else if(key_ == "<R>" || key_ == "<in>" || key_ == "<assign>" || key_ == "<label>" || key_ == "<goto>")
  {
    for(int i = 0; i < tokens_.size(); i++)
    {
      if(tokens_[i].id == identifier)
      {
        std::string k = tokens_[i].instance;

        if(!stat.containsKey(k))
        {
          std::cout << "SEMANTICS ERR\n";
          std::cout << "Line " << tokens_[i].posString() << " \"";
          std::cout << tokens_[i].instance;
          std::cout << "\" has not been defined in this scope\n";
          std::cout << "tokens that are in vector are: " << "\n";
          exit(-3);
        }
      }
    }
  }

  for(int i = 0; i < children_.size(); i++)
  {
    children_[i].statChecker(stat, scope);
  }

}

template <class T>
void node<T>::genASM(std::ostream& out)
{
	std::set<std::string> varset;
	stack<std::string, int> stackvars;
	std::string labelctr("A");

	genASM(out, 0, varset, stackvars, labelctr);
}

template <class T>
void node<T>::genASM(std::ostream& out, int scope, std::set<std::string>& varset, stack<std::string, int>& stackvars, std::string& labelctr)
{
	bool verbose = false;

	if (key_ == "<program>")
  {         // vars block
		genChildASM(out, scope, varset, stackvars, labelctr);
		out << "STOP\n";
		std::set<std::string>::iterator itr;

		for (itr = varset.begin(); itr != varset.end(); ++itr)
    {
			out << *itr << " 0\n";
		}

    //these will be our temp variables
		out << "outvar 0\n";
		out << "mathvar 0\n";
	}
  else if (key_ == "<block>")
  {         // start vars stats stop
		genChildASM(out, scope+1, varset, stackvars, labelctr);
		// restore variables stacked within block
		while (stackvars.getLastScope() == scope+1)
    {
			out << "STACKR 0\n";
			out << "STORE " << stackvars.getLastKey() << "\n";
			out << "POP\n";
      //out << "THIS IS A TEST TO SEE A MISPRINT \n"; //debug
			stackvars.pop();
		}
	}
  else if (key_ == "<vars>")
  {         // empty | var id := num vars
		if (tokens_.size())
    {
			if (varset.find(tokens_[1].instance) != varset.end())
      {
				// variable needs to be pushed to the stack
				out << "PUSH\n";
				out << "LOAD " << tokens_[1].instance << "\n";
				out << "STACKW 0\n";
				stackvars.push(tokens_[1].instance, scope);
			}

      //the way that I read in a - I had to work around actually reading in a
      //  "-" so I multiply it by a -1 at the beginning before storing it
      if(tokens_[4].instance == "-")
      {
        out << "LOAD " << tokens_[5].instance << "\n";
        out << "MULT -1\n";
        out << "STORE " << tokens_[1].instance << "\n";
      }
      else
      {
        out << "LOAD " << tokens_[4].instance << "\n";
        out << "STORE " << tokens_[1].instance << "\n";
      }

			varset.insert(tokens_[1].instance);
		}

		genChildASM(out, scope, varset, stackvars, labelctr);
	}
  else if (key_ == "<expr>")
  {                         // N - expr | N
		genChildASM(out, scope, varset, stackvars, labelctr);

		if(tokens_.size())
    {
			out << "STACKR 0\n";
			out << "POP\n";
			out << "STORE mathvar\n";
			out << "STACKR 0\n";
			out << "SUB mathvar\n";
			out << "STACKW 0\n";
		}
	}
  else if (key_ == "<A>")
  {                         // M + A | M
		genChildASM(out, scope, varset, stackvars, labelctr);

		if(tokens_.size())
    {
			out << "STACKR 0\n";
			out << "POP\n";
			out << "STORE mathvar\n";
			out << "STACKR 0\n";
			out << "ADD mathvar\n";
			out << "STACKW 0\n";
		}
	}
  else if (key_ == "<N>")
  {                         // A / N | A * N | A
		genChildASM(out, scope, varset, stackvars, labelctr);

		if(tokens_.size() && tokens_[0].instance == "*")
    {
			out << "STACKR 0\n";
			out << "POP\n";
			out << "STORE mathvar\n";
			out << "STACKR 0\n";
			out << "MULT mathvar\n";
			out << "STACKW 0\n";
		}
    else if (tokens_.size() && tokens_[0].instance == "/")
    {
			out << "STACKR 0\n";
			out << "POP\n";
			out << "STORE mathvar\n";
			out << "STACKR 0\n";
			out << "DIV mathvar\n";
			out << "STACKW 0\n";
		}
	}
  else if (key_ == "<M>")
  {                         // * M | R
		genChildASM(out, scope, varset, stackvars, labelctr);
		if (tokens_.size() == 1)
    {
			out << "STACKR 0\n";
			out << "MULT -1\n";
			out << "STACKW 0\n";
		}
	}
  else if (key_ == "<R>")
  {                         // ( expr ) | idTK | numTK
		genChildASM(out, scope, varset, stackvars, labelctr);

		// ( expr ) is on stack
		if (tokens_[0].id == identifier || tokens_[0].id == integer)
    {
			out << "PUSH\n";
			out << "LOAD " << tokens_[0].instance << "\n";
			out << "STACKW 0\n";
		}
	}
  else if (key_ == "<stats>")
  {         // stat mstat
		genChildASM(out, scope, varset, stackvars, labelctr);
	}
  else if (key_ == "<mStat>")
  {         // empty | stat mstat
		genChildASM(out, scope, varset, stackvars, labelctr);
	}
  else if (key_ == "<stat>")
  {         // in ;| out ;| block ;| if ;| loop ;| assign ;| goto ;| label;
		genChildASM(out, scope, varset, stackvars, labelctr);
	}
  else if (key_ == "<in>")
  {         // getter idTK
		out << "READ " << tokens_[1].instance << "\n";
	}
  else if (key_ == "<out>")
  {         // outter expr
		genChildASM(out, scope, varset, stackvars, labelctr);
		out << "STACKR 0\n";
		out << "POP\n";
		out << "STORE outvar\n";
		out << "WRITE outvar\n";
	}
  else if (key_ == "<if>")
  {
    // if [ exprA RO exprB ] then stat
		// do not generate asm for stat yet.
		for (int i = 0; i < children_.size()-1; i++)
    {
			children_[i].genASM(out, scope, varset, stackvars, labelctr);
		}

		// exprB is at stack[0]
		// exprA is at stack[1]
		/*out << "STACKR 0\n";
		out << "POP\n";
		out << "STORE mathvar\n";
		out << "STACKR 0\n";
		out << "POP\n";
		out << "SUB mathvar\n";*/
		// ACC: a-b

    //genChildASM(out, scope, varset, stackvars, labelctr);

		/*if (tokens_.size())
    {

			if (tokens_[4].instance == "=<")
      {
				out << "BRZPOS ";
			}
      else if (tokens_[4].instance == "=>")
      {
				out << "BRZNEG ";
			}
      else if(tokens_[4].instance == "==")
      {
				out << "BRZERO ";
			}
      else if(tokens_[4].instance == "[")
      {
        out << "BRPOS " << labelctr << "\n";
        out << "BRNEG ";
      }
		}*/
		//out << labelctr << "\n";
		std::string oldLabel(labelctr.c_str());
		getNextLabelString(labelctr);

		// gen asm for stat
		children_[children_.size()-1].genASM(out, scope, varset, stackvars, labelctr);
		out << oldLabel << ": NOOP\n";
	}
  else if(key_ == "<R0>")
  {


    std::string startLabel(labelctr.c_str());
    getNextLabelString(labelctr);
    std::string endLabel(labelctr.c_str());
    getNextLabelString(labelctr);

    if (!tokens_[0].instance.compare("=<"))
    {
      //out << "THIS IS A TEST FOR =< !!!!\n"; //debug
      out << "SUB mathvar\n";
      out << "BRNEG " << endLabel << "\n";
    }
    else if (!tokens_[0].instance.compare("=>"))
    {
      //out << "THIS IS A TEST FOR => !!!!\n"; //debug
      out << "SUB mathvar\n";
      out << "BRPOS " << endLabel << "\n";
    }
    else if(!tokens_[0].instance.compare("=="))
    {
      //out << "THIS IS A TEST FOR == !!!!\n"; //debug
      out << "SUB mathvar\n";
      out << "BRPOS " << endLabel << "\n";
      out << "BRNEG " << endLabel << "\n";
    }

    else if(!tokens_[0].instance.compare("%"))
    {
      //out << "THIS IS A TEST FOR % !!!!\n"; //debug
      out << "MULT mathvar\n";
      out << "BRPOS " << endLabel << "\n";
    }

    else if(!tokens_[0].instance.compare("["))
    {
      out << "THIS IS A TEST FOR [ == ] !!!!\n"; //debug
      out << "SUB mathvar\n";
      out << "BRZERO " << endLabel << "\n";
    }

    out << "BR " << startLabel << "\n";
    out << endLabel << ": NOOP\n";
  }
  else if (key_ == "<loop>")
  {         // loop [ expr RO expr ] stat
		out << labelctr << ": NOOP\n";
		std::string startLabel(labelctr.c_str());
		getNextLabelString(labelctr);
		std::string endLabel(labelctr.c_str());
		getNextLabelString(labelctr);

		// is condition true?
		for (int i = 0; i < children_.size()-1; i++)
    {
			children_[i].genASM(out, scope, varset, stackvars, labelctr);
		}
		// expr:b is at stack[0]
		// expr:a is at stack[1]
		out << "STACKR 0\n";
		out << "POP\n";
		out << "STORE mathvar\n";
		out << "STACKR 0\n";
		out << "POP\n";
    //genChildASM(out, scope, varset, stackvars, labelctr);
		// ACC: a-b
    //setR0Call(out, scope, varset, stackvars, labelctr, endLabel);

    /*if (!tokens_[1].instance.compare("=<"))
    {
      out << "THIS IS A TEST FOR =< !!!!\n"; //debug
      out << "SUB mathvar\n";
			out << "BRNEG " << endLabel << "\n";
		}
    else if (!tokens_[1].instance.compare("=>"))
    {
      out << "THIS IS A TEST FOR => !!!!\n"; //debug
      out << "SUB mathvar\n";
			out << "BRPOS " << endLabel << "\n";
		}
    else if(!tokens_[1].instance.compare("=="))
    {
      out << "THIS IS A TEST FOR == !!!!\n"; //debug
      out << "SUB mathvar\n";
			out << "BRPOS " << endLabel << "\n";
      out << "BRNEG " << endLabel << "\n";
		}

    else if(!tokens_[1].instance.compare("%"))
    {
      out << "THIS IS A TEST FOR % !!!!\n"; //debug
      out << "MULT mathvar\n";
      out << "BRPOS " << endLabel << "\n";
    }

    else if(!tokens_[2].instance.compare("]"))
    {
      out << "THIS IS A TEST FOR [ == ] !!!!\n"; //debug
      out << "SUB mathvar\n";
      out << "BRZERO " << endLabel << "\n";
    }*/
		//out << endLabel << "\n";

		//gen asm for stat
		children_[children_.size()-1].genASM(out, scope, varset, stackvars, labelctr);

		/*out << "BR " << startLabel << "\n";
		out << endLabel << ": NOOP\n";*/

}
  else if (key_ == "<assign>")
  {         // assign idTK := expr
		genChildASM(out, scope, varset, stackvars, labelctr);
		out << "STACKR 0\n";
		out << "POP\n";
		out << "STORE " << tokens_[1].instance << "\n";
	}
  else if(key_ == "<label>") //void idTK
  {
    out << tokens_[1].instance << ": NOOP";
  }
  else if(key_ == "<goto>") //proc idTK
  {
    out << "BR " << tokens_[1].instance << "\n";
  }
}


template <class T>
void node<T>::genChildASM(std::ostream& out, int scope, std::set<std::string>& varset, stack<std::string, int>& stackvars, std::string& labelctr)
{
	for (int i = 0; i < children_.size(); i++)
  {
		children_[i].genASM(out, scope, varset, stackvars, labelctr);
	}
}

template <class T>
void node<T>::getNextLabelString(std::string& labelctr)
{
	// 65 - 122
	int lasti = labelctr.length()-1;

	if ((int)labelctr[lasti] < 122)
  {
		labelctr[lasti]++;
	}
  else
  {
		if ((int)labelctr[0] == 122)
    {
			labelctr.append("A");
		}

		for (int i = lasti; i >= 0; i--)
    {
			if ((int)labelctr[i] >= 122)
      {
				labelctr[i] = 'A';
				if (i > 0) labelctr[i-1]++;
			}
      else
      {
				break;
			}
		}
	}
}

//Read that R0 returns and decide which opTK to use based on that reading
/*template <class T>
void node<T>::setR0Call(std::ostream& out, int scope, std::set<std::string>& varset, stack<std::string, int>& stackvars, std::string& labelctr, std::string& endLabel)
{
  std::string tempString = tokens_[1].instance;
  out << "OUR TEMP IS SET TO " << tempString << "\n";
  out << "TESTING TO SEE IF WE COME IN!!!!\n"; //debug
  if (tempString == "=<")
  {
    out << "THIS IS A TEST FOR =< !!!!\n"; //debug
    out << "SUB mathvar\n";
    out << "BRNEG " << endLabel << "\n";
  }
  else if (tempString == "=>")
  {
    out << "THIS IS A TEST FOR => !!!!\n"; //debug
    out << "SUB mathvar\n";
    out << "BRPOS " << endLabel << "\n";
  }
  else if(tempString == "==")
  {
    out << "THIS IS A TEST FOR == !!!!\n"; //debug
    out << "SUB mathvar\n";
    out << "BRPOS " << endLabel << "\n";
    out << "BRNEG " << endLabel << "\n";
  }
  else if(tempString == "%")
  {
    out << "THIS IS A TEST FOR % !!!!\n"; //debug
    out << "MULT mathvar\n";
    out << "BRPOS " << endLabel << "\n";
  }
  else if(tempString == "[")
  {
    out << "THIS IS A TEST FOR [ == ] !!!!\n"; //debug
    out << "SUB mathvar\n";
    out << "BRZERO " << endLabel << "\n";
  }
}*/

#endif
