#ifndef STACK_H
#define STACK_H

#include <vector>

template <class K, class I>
class stack
{
private:
  std::vector<K> keysK;
  std::vector<I> scopesI;

public:
  void push(K key, I scope);
  void pop();
  void popScope(I scope);

  bool containsKey(K key);
  bool containsKey(K key, I scope);

  void printStack();
};

template <class K, class I>
void stack<K, I>::push(K key, I scope)
{
  //std::cout << key << "\n"; //debug
  keysK.push_back(key);
  scopesI.push_back(scope);
}

template <class K, class I>
void stack<K, I>::pop()
{
  keysK.pop_back();
  scopesI.pop_back();
}

template <class K, class I>
void stack<K, I>::popScope(I scope)
{
  if(scopesI.size() > 0)
  {
      while(scopesI.back() == scope)
      {
        pop();
      }
  }
}

template <class K, class I>
bool stack<K, I>::containsKey(K key)
{
  if(keysK.size() > 0)
  {
    for(int i = 0; i < keysK.size(); i++)
    {
      if(keysK[i] == key)
      {
        return true;
      }
    }
  }

  return false;
}

template <class K, class I>
bool stack<K, I>::containsKey(K key, I scope)
{
  if(keysK.size() > 0)
  {
    for(int i = 0; i < keysK.size(); i++)
    {
      if(keysK[i] == key && scopesI[i] == scope)
      {
        return true;
      }
    }
  }
  return false;
}

template <class K, class I>
void stack<K, I>::printStack()
{
  if(keysK.size() > 0)
  {
    std::cout << "Stack Contains: \n";

    for(int i = keysK.size() - 1; i >= 0; i--)
    {
      std::cout << keysK[i] << " : " << scopesI[i] << "\n";
    }

    std::cout << "\n:";
  }
}

#endif
