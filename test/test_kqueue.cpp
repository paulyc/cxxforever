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

#include <gtest/gtest.h>

#include "../src/net.hpp"

#include <cerrno>

namespace {
    
    using namespace io::github::paulyc::twilioplusplus;
    
    int sock;
    int kq;
    
    TEST(SocketTest, socketSuccess) {
        sock = socket(PF_INET, SOCK_STREAM, 0);
        EXPECT_NE(sock, -1);
    }
    
    TEST(SocketTest, socketFail) {
        sock = socket(99999, SOCK_STREAM, PF_INET);
        
        EXPECT_EQ(sock, -1);
        EXPECT_EQ(errno, EAFNOSUPPORT);
    }
    
    TEST(SocketTest, bindSuccess) {
        const sockaddr_in addr = {
            AF_INET,
            AF_INET,
            htons(8888),
            { inet_addr("0.0.0.0") }
        };
        sock = socket(PF_INET, SOCK_STREAM, 0);
        int res = bind(sock, (const sockaddr *)&addr, sizeof(sockaddr_in));
        EXPECT_EQ(res, 0);
    }
    
    TEST(KqueueTest, kqueue) {
        
    }
}
