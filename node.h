
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

  public:
      // AST functionality
      void insert(node<T> child);
      void insert(token childToken);
      node<T> (T key);

      // traversal functionality
      void preOrder(std::ostream& out);
      void preOrder();
      std::string toString();

      //Enables Static Semantics
      void statChecker();
};

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

#endif
