// api.h
class hello // a <hello> XML message is simply a hello class with members (sub-elements)
{
public:
  std::string name;               // a <hello> message has a <name> element
  hello(const std::string &text); // construct a <hello><name>text</name></hello> message
};

class greeting // a <greeting> XML message is simply a greeting class with members (sub-elements)
{
public:
  std::string message;               // a <greeting> has a <message> element
  greeting(const std::string &text); // construct a <greeting><message>text</message></greeting>
};