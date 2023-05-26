#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <fmt/format.h>

inline std::string printError(const char* file, int line, const std::string& error)
{
   throw std::string(fmt::format("error from {} ({}):\n {} \n", file, line, error));
}

#define error(err) throwError_(__FILE__, __LINE__, err)

inline std::string throwError_(const char* file, int line, const std::string& error){
   throw printError(file, line, error);
}


#endif