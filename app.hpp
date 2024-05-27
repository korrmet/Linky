#ifndef APP_HPP
#define APP_HPP

#include "independency.hpp"

#define iv independency::value
#define IM independency::message
#define IC independency::callback
#define IP independency::path
#define ROOT IP()

#include <cstdio>

#define PRINT(...) std::printf(__VA_ARGS__); std::fflush(stdout);

#ifdef DEBUG
#define DBG PRINT
#else
#define DBG
#endif // DEBUG

// common bus for all
extern independency::bus bus;

// here the current circuit is stores
extern independency::storage circuit;

// this one for the current temporary data
extern independency::storage context;

// and this one for the stuff from the .linkyrc config file
extern independency::storage params;

#endif // APP_HPP
