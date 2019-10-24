#ifndef __ERROR_H__
#define __ERROR_H__

namespace error{

class semantic_error : public std::exception {
    std::string info;
    int pos;
    // Add (if needed) file, function

    public:
    	  semantic_error(std::string msg, int pos) : info(msg), pos(pos) {}

        std::string getMessage() { return info+std::to_string(pos); }

    };
};

#endif
