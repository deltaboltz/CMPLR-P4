#ifndef TOKEN_H
#define TOKEN_H
#include <map>

enum tokenID {
	null=-1,
    error=0,
    identifier=1001,
    integer=1002,
    opordel=1003,
    keyword=1004,
    eoftk=1005
};

struct token {
    enum tokenID id;
    std::string instance;
    std::pair<int, int> pos;
    std::pair<int, int> size;
    std::map<int, std::string> idmap = {
        {null,        "\nNULL TOKEN\n"},
        { error,      "\nSCANNER ERROR:\n"},
        { identifier, "idTK:" },
        { integer,    "numTK:" },
        { opordel,    "opTK:" },
        { keyword,    "kwTK:" },
        { eoftk,      "eofTK" }
    };
    token(enum tokenID tid, std::string tinst, std::pair<int, int> tpos,
        std::pair<int, int> tsize) : id(tid), instance(tinst), pos(tpos), size(tsize) {}

    token()
    {
      id = null;
      instance = "";
      pos = std::make_pair(0,0);
      size = std::make_pair(0,0);
    }

    std::string posString()
    {
      std::string p("");

      p+= std::to_string(std::get<0>(pos)) + ":" + std::to_string(std::get<1>(pos));

      return p;
    }

    std::string toString()
    {
        std::string out("");
        if (id == error)
        {
            out += idmap[id] + posString() + ": " + instance;
        }
        else if(id == null)
        {
          out += idmap[id];
        }
        else
        {
          out += idmap[id] + "'" + instance + "'";
        }
        return out;
    }
};

#endif
