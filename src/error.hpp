// Copyright (c) 2018 Paul Ciarlo <paul.ciarlo@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef _IO_GITHUB_PAULYC_TWILIO___ERROR___HPP_
#define _IO_GITHUB_PAULYC_TWILIO___ERROR___HPP_

#include <string>
#include <exception>
#include <cerrno>

#define STRING(X) X
#define STRING_SEP ":"
#define STRINGIFY(X) #X
#define STRINGIFY_FILELINE() STRINGIFY(__FILE__) ":" STRINGIFY(__LINE__)

namespace io { namespace github { namespace paulyc {
    
    class fileline_exception : public std::exception {
    public:
        fileline_exception(const char *fileline_str) : _fileline_str(fileline_str) {}
        const std::string &getFilelineStr() const { return _fileline_str; }
    private:
        std::string _fileline_str;
    };
#define FILELINE_EXCEPTION() (io::github::paulyc::fileline_exception(STRINGIFY_FILELINE()))
    
    class classmember_exception : public std::exception {
    public:
        classmember_exception(const char *classmember_str) : _classmember_str(classmember_str) {}
        const std::string &getClassmemberStr() const { return _classmember_str; }
    private:
        std::string _classmember_str;
    };
#define CLASSMEMBER_EXCEPTION(CLASSMEMBER) (io::github::paulyc::classmember_exception(STRINGIFY(CLASSMEMBER)))

    class errno_exception : public std::exception {
    public:
        errno_exception() : _errno(errno), _strerror(strerror(errno)) {}
        int getErrno() const { return _errno; }
        const std::string &getStrError() const { return _strerror; }
    private:
        int _errno;
        std::string _strerror;
    };
#define ERRNO_EXCEPTION() (io::github::paulyc::errno_exception())
    
    class general_exception : public fileline_exception, public errno_exception {
    public:
        general_exception(const char *msg_str="") :
            fileline_exception(msg_str),
            errno_exception(),
            _msg(msg_str) {}
        general_exception(const char *fileline_str, const char *classmember_str, const char *msg_str="") :
            fileline_exception(fileline_str),
            errno_exception(),
            _msg(msg_str) {}
        const std::string &getMessage() const { return _msg; }
    private:
        std::string _msg;
    };
#define GENERAL_EXCEPTION(CLASSMEMBER) (io::github::paulyc::general_exception((STRINGIFY_FILELINE()), #CLASSMEMBER))
#define GENERAL_EXCEPTION_MSG(CLASSMEMBER, MSG) (io::github::paulyc::general_exception((STRINGIFY_FILELINE()), (STRINGIFY(CLASSMEMBER)), MSG)
    
}}}

#endif
