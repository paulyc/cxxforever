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

#ifndef _IO_GITHUB_PAULYC_TWILIO___LIBTWILIO___HPP_
#define _IO_GITHUB_PAULYC_TWILIO___LIBTWILIO___HPP_

#include <map>
#include <list>
#include <array>
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include <functional>
#include <algorithm>

#include <cassert>
#include <cerrno>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "client.hpp"

namespace io { namespace github { namespace paulyc { namespace twilioplusplus {
    
    class NetworkEngineConfig {
    public:
        NetworkEngineConfig() : NetworkEngineConfig(8888) {}
        NetworkEngineConfig(int httpListeningPort) : httpListeningPort(httpListeningPort) {}
        virtual ~NetworkEngineConfig() {}
        
        int httpListeningPort;
    };
    
    const static int MAX_SOCKETS = 1024;
    const static int MAX_EVENTS = 1024;
    
    class errno_exception : public std::exception {
    public:
        errno_exception() : _errno(errno), _str(strerror(errno)) {}
        int _errno;
        std::string _str;
    };
    class sockinit_exception : public errno_exception {};
    
    template <typename Client_T>
    class NetworkEngine : public NetworkEngineConfig {
    public:
        NetworkEngine() : _kq(-1), _listenSock4(-1) {}
        virtual ~NetworkEngine() {}
        
        void run() {
            _runThread = std::unique_ptr<std::thread>(new std::thread([this]() {
                doServerListen();
                serverMainLoop();
                //close kq
            }));
            
       }
        
        void cleanup() {
            close(_listenSock4);
            _listenSock4 = -1;
        }
        
        void initServerSock() throw(sockinit_exception) {
            // initialize server socket
            _listenSock4 = socket(AF_INET, SOCK_STREAM, PF_INET);
            if (_listenSock4 == -1) {
                cleanup();
                throw sockinit_exception();
            }
            
            const sockaddr_in addr = {
                AF_INET,
                AF_INET,
                htons(8888),
                { inet_addr("0.0.0.0") }
            };
            int res = bind(_listenSock4, (const sockaddr *)&addr, sizeof(sockaddr_in));
            if (res != 0) {
                cleanup();
                throw sockinit_exception();
            }
            
            res = listen(_listenSock4, 100);
            if (res != 0) {
                
            }
        }
        
        void doServerListen() {
            std::unique_lock<std::mutex>(_mutex);
            
            initServerSock();
            
            _kq = kqueue();
            if (_kq == -1) {
                // error
            }
            
            _eventsRegistered[0] = {(uintptr_t)_listenSock4, EVFILT_READ, EV_ADD | EV_ERROR | EV_CLEAR, NULL, 0, NULL};
            
            /*
             The kevent() system call returns the number of events placed in the
             eventlist,    up to the value    given by nevents.  If an error occurs while
             processing    an element of the changelist and there is enough room in the
             eventlist,    then the event will be placed in the eventlist with EV_ERROR
             set in flags and the system error in data.     Otherwise, -1 will be
             returned, and errno will be set to    indicate the error condition.  If the
             time limit    expires, then kevent() returns 0.*/
            
            std::array<struct kevent, MAX_EVENTS> eventsPending;
            int res = kevent(_kq, _eventsRegistered.data(), 1, eventsPending.data(), eventsPending.size(), NULL);
            if (res > 0) {
                assert(res == 1);
                if (eventsPending[0].flags & EV_ERROR) {
                    //errx(EXIT_FAILURE,    "Event error: %s", strerror(event.data));
                }
                //err(EXIT_FAILURE, "kevent register");
            } else if (res == 0) {
                // timeout
            } else if (res == -1) {
                // error
            } else {
                //???
            }
        }
        
        void serverMainLoop() {
            // main loop
            const struct timespec timeout = { 1, 0 }; //1.000000000 seconds
            std::array<struct kevent, MAX_EVENTS> eventsPending;
            for (;;) {
                int res = kevent(_kq, NULL, 0, eventsPending.data(), eventsPending.size(), &timeout);
                if (res > 0) {
                    // just q events
                    for (struct kevent *ev = eventsPending.data(); res > 0; --res, ++ev) {
                        // process each event
                        if (ev->ident == _listenSock4) {
                            if (ev->flags & EV_ERROR) {
                                // server socket error shut down the whole shebang
                            }
                            acceptClient();
                        } else {
                            handleClientEvent(ev, _clientSockMap[ev->ident]);
                        }
                    }
                } else if (res == 0) {
                    // timeout
                } else if (res == -1) {
                    // error
                } else {
                    //???
                }
            }
        }
        
        void acceptClient() {
            std::unique_lock<std::mutex>(_mutex);
            
            struct sockaddr_in addr;
            socklen_t addrLen;
            int clientSock = accept(_listenSock4, (struct sockaddr*)&addr, &addrLen);
            if (clientSock == -1) {
                /// error
            }
            assert(addrLen == sizeof(struct sockaddr_in));
            std::shared_ptr<Client_T> p = std::make_shared(new Client_T(_listenSock4, clientSock));
            _clientSockMap[p] = p;
            //struct *kevent pkv = &eventsRegistered[_clientSockMap.size()];
            std::array<struct kevent, 1> eventsPending;
            struct kevent *newEv = &_eventsRegistered[_clientSockMap.size()];
            *newEv = {(uintptr_t)clientSock, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ERROR | EV_CLEAR | EV_EOF, NULL, 0, this};
            int res = kevent(_kq, newEv, 1, eventsPending.data(), eventsPending.size(), NULL);
            if (res > 0) {
                assert(res == 1);
            } else if (res == 0) {
                //timeout
            } else if (res == -1) {
                // error
            } else {
                // ?????
            }
            for (auto &l : _clientListeners) {
                l(p);
            }
        }
        
        void handleClientEvent(struct kevent *ev, std::shared_ptr<Client_T> client) {
            //std::unique_lock<std::mutex>(_mutex);
        }
        
        typedef std::function<void(std::shared_ptr<Client_T>)> ClientListener;
        void registerClientListener(ClientListener listener) {
            std::unique_lock<std::mutex>(_mutex);
            
            _clientListeners.add(listener);
        }
        
    private:
        int _kq;
        int _listenSock4;
        std::unique_ptr<std::thread> _runThread;
        std::mutex _mutex;
        std::map<int, std::shared_ptr<Client_T>> _clientSockMap;
        std::list<ClientListener> _clientListeners;
        std::array<struct kevent, MAX_SOCKETS> _eventsRegistered;
    };
    
}}}}

#endif
