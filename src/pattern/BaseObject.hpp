#include <map>
#include <string>

struct BaseObject {};

struct ObjectInt : public BaseObject
{
  int a;
};

struct ObjectString : public BaseObject
{
  std::string a;
};