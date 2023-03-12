#ifndef BASEEXCEPTION_H
#define BASEEXCEPTION_H

#include <exception>
#include <string>

class BaseException : public std::exception
{
public:
    BaseException(int line, const char* file);
    const char* what() const override;
    virtual const char* GetType() const;
    int GetLine() const;
    const std::string& GetFile() const;
    std::string GetOriginString() const;
private:
    int line;
    std::string file;
protected:
    mutable std::string whatBuffer;
};

#endif