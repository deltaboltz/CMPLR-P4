#include <iostream>
#include <string>
#include <fstream>
#include "scanner.h"
#include "token.h"
#include "node.h"

using namespace std;

static token t;
static std::istream in(NULL);
static void parseErr(std::string expected);

static node<std::string> Program();
static node<std::string> Block();
static node<std::string> Vars();
static node<std::string> Expr();
static node<std::string> N();
static node<std::string> A();
static node<std::string> M();
static node<std::string> R();
static node<std::string> Stats();
static node<std::string> mStat();
static node<std::string> Stat();
static node<std::string> In();
static node<std::string> Out();
static node<std::string> If();
static node<std::string> Loop();
static node<std::string> Assign();
static node<std::string> R0();
static node<std::string> Label();
static node<std::string> Goto();

node<std::string> parser(std::istream& stream)
{
  in.rdbuf(stream.rdbuf());
  t = scan(in);

  node<std::string> root = Program();

  if(t.id != eoftk)
  {
    parseErr("EOFTK");
  }

  return root;
}

//<program> -> <vars> kwTK(main) <block>
static node<std::string> Program()
{
  node<std::string> root("<program>"); //set the root of the node to <program>

  root.insert(Vars()); //call the <vars> definition

  //check to see if the tokenID is a keword and it's comparable to "main"
  if(t.id == keyword && !t.instance.compare("main"))
  {
    root.insert(t); //insert token into node
    t = scan(in); //scan for next token
    root.insert(Block()); //call the <block> definition
  }

  return root; //return the root vector
}

//<block> -> kwTK(begin) <vars> <stats> kwTK(end)
static node<std::string> Block()
{
  node<std::string> root("<block>"); //set the root of the node to <block>

  //check to see if the tokenID is a kwTK(begin)
  if(t.id == keyword && !t.instance.compare("begin"))
  {
    root.insert(t); //insert token into node
    t = scan(in); //scan for the next token

    root.insert(Vars()); //call the <vars> definition
    root.insert(Stats()); //call the <stats> definition

    //check now to see if the <block> definition is at the last terminal
    if(t.id == keyword && !t.instance.compare("end"))
    {
      root.insert(t); //insert token into node
      t = scan(in); //scan for next token
      return root; //return root vector
    }
    parseErr("kwTK: 'end'"); //parse error when different token other than 'end' is given
  }
  parseErr("kwTK: 'begin'"); //parse error when different token other than 'begin' is given
  return root; //return root vector
}

//<vars> -> Ɛ | kwTK(data) idTK(identifier) opTK(:) opTK(=) numTK(integer) opTK(;) <vars>
static node<std::string> Vars()
{
  node<std::string> root("<vars>"); //set the root of the node to <vars>

  //check to see if tokenID is a kwTK("data")
  if(t.id == keyword && !t.instance.compare("data"))
  {
    root.insert(t); //insert root into node
    t = scan(in); //scan for next token

    //if tokenID is any tkID
    if(t.id == identifier)
    {
      root.insert(t); //insert root into node
      t = scan(in); //scan for next token

      //if tokenID is opTK(":")
      if(t.id == opordel && !t.instance.compare(":"))
      {
        root.insert(t); //insert root into node
        t = scan(in); //scan for next token

        //if tokenID is opTK("=") making our complete ":="
        if(t.id == opordel && !t.instance.compare("="))
        {
          root.insert(t); //insert root into node
          t = scan(in); //scan for next token

          //if tokenID is any numTK
          if(t.id == integer)
          {
            root.insert(t); //insert root into node
            t = scan(in); //scan for next token

            //if tokenID is opTK(";")
            if(t.id == opordel && !t.instance.compare(";"))
            {
              root.insert(t); //insert root into note
              t = scan(in); //scan for next token
              root.insert(Vars());
              return root;
            }
            return root;
          }
          parseErr("numTK");
        }
        parseErr("opTK: '='");
      }
      parseErr("opTK: ':'");
    }
    parseErr("idTK");
  }
  return root;
}

//<expr> -> <N> - <expr> | <N>
static node<std::string> Expr()
{
  node<std::string> root("<expr>");

  root.insert(N());

  if(t.id == opordel && !t.instance.compare("-"))
  {
    root.insert(t);
    t = scan(in);
    root.insert(Expr());
    return root;
  }

  return root;
}

//<N> -> <A> / <N> | <A> * <N> | <A>
static node<std::string> N()
{
  node<std::string> root("<N>");

  root.insert(A());

  if(t.id == opordel && !t.instance.compare("/"))
  {
    root.insert(t);
    t = scan(in);
    root.insert(N());
    return root;
  }
  else if(t.id == opordel && !t.instance.compare("*"))
  {
    root.insert(t);
    t = scan(in);
    root.insert(N());
    return root;
  }
  return root;
}

//<A> -> <M> + <A> | <M>
static node<std::string> A()
{
  node<std::string> root("<A>");

  root.insert(M());

  if(t.id == opordel && !t.instance.compare("+"))
  {
    root.insert(t);
    t = scan(in);
    root.insert(A());
    return root;
  }

  return root;
}

// <M> -> * <M> | <R>
static node<std::string> M()
{
  node<std::string> root("<M>");

  if(t.id == opordel && !t.instance.compare("*"))
  {
    root.insert(t);
    t = scan(in);
    root.insert(M());
    return root;
  }
  else
  {
    root.insert(R());
    return root;
  }

  return root;
}

//<R> -> opTK(() <expr> opTK()) | idTK | numTK
static node<std::string> R()
{
  node<std::string> root("<R>");

  if(t.id == opordel && !t.instance.compare("("))
  {
    root.insert(t);
    t = scan(in);
    root.insert(Expr());

    if(t.id == opordel && !t.instance.compare(")"))
    {
      root.insert(t);
      t = scan(in);
      return root;
    }
    parseErr("opTK: ')'");
  }
  else if(t.id == identifier)
  {
    root.insert(t);
    t = scan(in);
    return root;
  }
  else if(t.id == integer)
  {
    root.insert(t);
    t = scan(in);
    return root;
  }
  parseErr("idTK or numTK");
  return root;
}

//<stats> -> <stat> <mStat>
static node<std::string> Stats()
{
  node<std::string> root("<stats>");

  root.insert(Stat());
  root.insert(mStat());

  return root;
}

//<mStat> -> Ɛ | <stat> <mStat>
static node<std::string> mStat()
{
  node<std::string> root("<mStat>");

  if(t.id == keyword && (!t.instance.compare("stat") || !t.instance.compare("outter") || !t.instance.compare("getter") || !t.instance.compare("assign")))
  {
    root.insert(Stat());
    root.insert(mStat());

    return root;
  }

  return root;
}

/* <stat> -> <in> opTK(;) | <out> opTK(;) | <block> |
 * <if> opTK(;) | <loop> opTK(;) | <assign> opTK(;) |
 * <goto> opTK(;) | <label> opTK(;) |
 */
static node<std::string> Stat()
{
  node<std::string> root("<stat>");

  if(t.id == keyword && !t.instance.compare("getter"))
  {
    root.insert(In());
    if(t.id == opordel && !t.instance.compare(";"))
    {
      root.insert(t);
      t = scan(in);
      return root;
    }
    return root;
  }
  else if(t.id == keyword && !t.instance.compare("outter"))
  {
    root.insert(Out());
    if(t.id == opordel && !t.instance.compare(";"))
    {
      root.insert(t);
      t = scan(in);
      return root;
    }
    return root;
  }
  else if(t.id == keyword && !t.instance.compare("begin")) //<block> begin with kwTK("begin")
  {
    root.insert(Block());
    return root;
  }
  else if(t.id == keyword && !t.instance.compare("if"))
  {
    root.insert(If());
    if(t.id == opordel && !t.instance.compare(";"))
    {
      root.insert(t);
      t = scan(in);
      return root;
    }
    return root;
  }
  else if(t.id == keyword && !t.instance.compare("loop"))
  {
    root.insert(Loop());
    if(t.id == opordel && !t.instance.compare(";"))
    {
      root.insert(t);
      t = scan(in);
      return root;
    }
    return root;
  }
  else if(t.id == keyword && !t.instance.compare("assign"))
  {
    root.insert(Assign());
    if(t.id == opordel && !t.instance.compare(";"))
    {
      root.insert(t);
      t = scan(in);
      return root;
    }
    return root;
  }
  else if(t.id == keyword && !t.instance.compare("proc")) //<goto> begins with kwTK("proc")
  {
    root.insert(Goto());
    if(t.id == opordel && !t.instance.compare(";"))
    {
      root.insert(t);
      t = scan(in);
      return root;
    }
    return root;
  }
  else if(t.id == keyword && !t.instance.compare("void")) //<label> begins with kwTK("void")
  {
    root.insert(Label());
    if(t.id == opordel && !t.instance.compare(";"))
    {
      root.insert(t);
      t = scan(in);
      return root;
    }
    return root;
  }

  parseErr("kwTK : {in, out, begin, if, loop, assign, proc, or label}");
}

// <in> -> kwTK("getter") idTK
static node<std::string> In()
{
  node<std::string> root("<in>");

  if(t.id == keyword && !t.instance.compare("getter"))
  {
    root.insert(t);
    t = scan(in);

    if(t.id == identifier)
    {
      root.insert(t);
      t = scan(in);
      return root;
    }
    parseErr("idTK or opTK");
  }
  parseErr("kwTK: 'getter'");
  return root;
}

// <out> -> kwTK("outter") <expr>
static node<std::string> Out()
{
  node<std::string> root("<out>");

  if(t.id == keyword && !t.instance.compare("outter"))
  {
    root.insert(t);
    t = scan(in);
    root.insert(Expr());
    return root;
  }
  parseErr("kwTK: 'outter'");
  return root;
}

// <if> -> kwTK("if") opTK([) <expr> <R0> <expr> opTK(]) kwTK("then") <stat>
static node<std::string> If()
{
  node<std::string> root("<if>");

  if(t.id == keyword && !t.instance.compare("if"))
  {
    root.insert(t);
    t = scan(in);

    if(t.id == opordel && !t.instance.compare("["))
    {
      root.insert(t);
      t = scan(in);

      root.insert(Expr());
      root.insert(R0());
      root.insert(Expr());

      if(t.id == opordel && !t.instance.compare("]"))
      {
        root.insert(t);
        t = scan(in);
      }

      if(t.id == keyword && !t.instance.compare("then"))
      {
        root.insert(t);
        t = scan(in);
      }
      root.insert(Stat());
      return root;
      parseErr("opTK: ']'");
    }
    parseErr("opTK: '['");
  }
  parseErr("kwTK: 'if'");

  return root;
}

// <loop> -> kwTK("loop") opTK([) <expr> <R0> <expr> opTK(]) <stat>
static node<std::string> Loop()
{
  node<std::string> root("<loop>");

  if(t.id == keyword && !t.instance.compare("loop"))
  {
    root.insert(t);
    t = scan(in);

    if(t.id == opordel && !t.instance.compare("["))
    {
      root.insert(t);
      t = scan(in);

      root.insert(Expr());
      root.insert(R0());
      root.insert(Expr());

      if(t.id == opordel && !t.instance.compare("]"))
      {
        root.insert(t);
        t = scan(in);
      }
      root.insert(Stat());
      return root;
      parseErr("opTK: ']'");
    }
    parseErr("opTK: '['");
  }
  parseErr("kwTK: 'loop'");
  return root;
}

//<assign> -> kwTK("assign") idTK optk(:) opTK(=) <expr>
static node<std::string> Assign()
{
  node<std::string> root("<assign>");
  if(t.id == keyword && !t.instance.compare("assign"))
  {
    root.insert(t);
    t = scan(in);
    if(t.id == identifier)
    {
      root.insert(t);
      t = scan(in);

      if(t.id == opordel && !t.instance.compare(":"))
      {
        root.insert(t);
        t = scan(in);

        if(t.id == opordel && !t.instance.compare("="))
        {
          root.insert(t);
          t = scan(in);

          root.insert(Expr());
          return root;
        }
      }
      parseErr("opTK: ':='");
    }
    parseErr("idTK");
    return root;
  }
}

// <R0> -> opTK(=>) | opTK(=<) | opTK(==) | opTK([) opTK(==) opTK(]) | opTK(%)
static node<std::string> R0()
{
  node<std::string> root("<R0>");

  if(t.id == opordel && !t.instance.compare("=>"))
  {
    root.insert(t);
    t = scan(in);
    return root;
  }

  if(t.id == opordel && !t.instance.compare("=<"))
  {
    root.insert(t);
    t = scan(in);
    return root;
  }
  if(t.id == opordel && !t.instance.compare("=="))
  {
    root.insert(t);
    t = scan(in);
    return root;
  }
  if(t.id == opordel && !t.instance.compare("["))
  {
    root.insert(t);
    t = scan(in);

    if(t.id == opordel && !t.instance.compare("=="))
    {
      root.insert(t);
      t = scan(in);
    }

    if(t.id == opordel && !t.instance.compare("]"))
    {
      root.insert(t);
      t = scan(in);
      return root;
    }
    parseErr("opTK: ']'");
  }
  else if(t.id == opordel && !t.instance.compare("%"))
  {
    root.insert(t);
    t = scan(in);
    return root;
  }
  parseErr("opTK: '=', '<', '>', '%'");
  return root;
}

// <label> -> kwTK(void) idTK
static node<std::string> Label()
{
  node<std::string> root("<label>");

  if(t.id == keyword && !t.instance.compare("void"))
  {
    root.insert(t);
    t = scan(in);

    if(t.id == identifier)
    {
      root.insert(t);
      t = scan(in);

      return root;
    }
    parseErr("idTK");
  }
  parseErr("kwTK: 'void'");
  return root;
}

// <goto> -> kwTK("proc") idTK
static node<std::string> Goto()
{
  node<std::string> root("<goto>");

  if(t.id == keyword && !t.instance.compare("proc"))
  {
    root.insert(t);

    t = scan(in);

    if(t.id == identifier)
    {
      root.insert(t);
      t = scan(in);
      return root;
    }
    parseErr("idTK");
  }
  parseErr("kwTK: 'proc'");
  return root;
}

//parse error function to throw expected tokenID
static void parseErr(std::string expected)
{
  cout << "\n PARSE ERR \n";

  cout << "Line" << t.posString() << "  \"" << t.instance;

  cout << "\" does not match the expected token : " << expected << ".\n";

  exit(-1);
}
