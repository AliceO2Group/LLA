
// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file TestLock.cxx
/// \brief Tests for the Locks of the LLA library.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#define BOOST_TEST_MODULE LLA_TestLocks
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <unordered_map>

#include <Lla/Exception.h>
#include <InterprocessLockFactory.h>

using namespace o2::lla;

BOOST_AUTO_TEST_SUITE(LowLevelArbitration)

BOOST_AUTO_TEST_CASE(LockFactoryFails)
{
  auto params = LockParameters::makeParameters(LockType::Type::FileLock);
  BOOST_CHECK_THROW(InterprocessLockFactory::getInterprocessLock(params), LlaException);
}

BOOST_AUTO_TEST_CASE(NamedMutexRawLock)
{
  auto params = LockParameters::makeParameters(LockType::Type::NamedMutex);
  auto namedMutex = InterprocessLockFactory::getInterprocessLock(params);
  namedMutex->lock();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  namedMutex->unlock();
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(SocketRawLock)
{
  auto params = LockParameters::makeParameters(LockType::Type::SocketLock);
  auto socketLock = InterprocessLockFactory::getInterprocessLock(params);
  socketLock->lock();
  std::this_thread::sleep_for(std::chrono::milliseconds(50)); // critical section
  socketLock->unlock();
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(SocketTryLock)
{
  auto params = LockParameters::makeParameters(LockType::Type::SocketLock);
  auto socketLock = InterprocessLockFactory::getInterprocessLock(params);
  if (socketLock->tryLock()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // critical section
    socketLock->unlock();
  }
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(SocketTimedLock)
{
  auto params = LockParameters::makeParameters(LockType::Type::SocketLock);
  auto socketLock = InterprocessLockFactory::getInterprocessLock(params);
  if (socketLock->timedLock(50)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // critical section
    socketLock->unlock();
  }
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(MultiSocketLock)
{
  std::vector<std::thread> workers;
  int failed = 0;
  for (int i = 0; i < 2; i++) {
    workers.push_back(std::thread([&]() {
      auto params = LockParameters::makeParameters(LockType::Type::SocketLock);
      auto alock = InterprocessLockFactory::getInterprocessLock(params);
      if (alock->timedLock(10)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(12)); // critical section
        alock->unlock();
      } else {
        failed++;
      }
    }));
  }

  std::for_each(workers.begin(), workers.end(), [](std::thread& t) {
    t.join();
  });

  BOOST_CHECK(failed == 1);
}

BOOST_AUTO_TEST_CASE(TimeSocketLock)
{
  std::vector<std::thread> workers;
  auto params = LockParameters::makeParameters(LockType::Type::SocketLock);
  auto alock = InterprocessLockFactory::getInterprocessLock(params);
  int avg = 0, i;
  std::chrono::high_resolution_clock::time_point start, stop;
  for (i = 0; i < 1000; i++) {
    start = std::chrono::high_resolution_clock::now();
    alock->lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    alock->unlock();
    stop = std::chrono::high_resolution_clock::now();
    avg += std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start - std::chrono::milliseconds(10)).count();
  }

  //std::cout << avg / i << std::endl;

  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(UltraSocketLock)
{
  std::vector<std::thread> workers;
  std::unordered_map<int, int> umap;
  for (int i = 0; i < 6; i++) {
    umap[i] = 0;
    workers.push_back(std::thread([&](int x) {
      auto params = LockParameters::makeParameters(LockType::Type::SocketLock);
      auto alock = InterprocessLockFactory::getInterprocessLock(params);

      const auto start = std::chrono::steady_clock::now();
      auto timeExceeded = [&]() { return ((std::chrono::steady_clock::now() - start) > std::chrono::seconds(1)); };
      while (!timeExceeded()) {
        if (alock->timedLock(100)) {
          umap[x]++;
          std::this_thread::sleep_for(std::chrono::milliseconds(10 * x + 2)); // critical section
          alock->unlock();
        }
      }
    },
                                  i));
  }

  std::for_each(workers.begin(), workers.end(), [](std::thread& t) {
    t.join();
  });

  //std::cout << "umap state" << std::endl;

  for (auto& el : umap) {
    //std::cout << el.first << " " << el.second << std::endl;
    if (el.second < 1) {
      BOOST_CHECK(false);
    }
  }

  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(NamedMutexTryLock)
{
  auto params = LockParameters::makeParameters(LockType::Type::NamedMutex);
  auto socketLock = InterprocessLockFactory::getInterprocessLock(params);
  if (socketLock->tryLock()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // critical section
    socketLock->unlock();
  }
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(NamedMutexTimedLock)
{
  auto params = LockParameters::makeParameters(LockType::Type::NamedMutex);
  auto socketLock = InterprocessLockFactory::getInterprocessLock(params);
  if (socketLock->timedLock(50)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // critical section
    socketLock->unlock();
  }
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(MultiNamedMutexLock)
{
  std::vector<std::thread> workers;
  int failed = 0;
  for (int i = 0; i < 2; i++) {
    workers.push_back(std::thread([&]() {
      auto params = LockParameters::makeParameters(LockType::Type::NamedMutex);
      auto alock = InterprocessLockFactory::getInterprocessLock(params);
      if (alock->timedLock(10)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(12)); // critical section
        alock->unlock();
      } else {
        failed++;
      }
    }));
  }

  std::for_each(workers.begin(), workers.end(), [](std::thread& t) {
    t.join();
  });

  BOOST_CHECK(failed == 1);
}

BOOST_AUTO_TEST_CASE(TimeNamedMutexLock)
{
  std::vector<std::thread> workers;
  auto params = LockParameters::makeParameters(LockType::Type::NamedMutex);
  auto alock = InterprocessLockFactory::getInterprocessLock(params);
  int avg = 0, i;
  std::chrono::high_resolution_clock::time_point start, stop;
  for (i = 0; i < 1000; i++) {
    start = std::chrono::high_resolution_clock::now();
    alock->lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    alock->unlock();
    stop = std::chrono::high_resolution_clock::now();
    avg += std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start - std::chrono::milliseconds(10)).count();
  }

  //std::cout << avg / i << std::endl;

  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(UltraNamedMutexLock)
{
  std::vector<std::thread> workers;
  std::unordered_map<int, int> umap;
  for (int i = 0; i < 6; i++) {
    umap[i] = 0;
    workers.push_back(std::thread([&](int x) {
      auto params = LockParameters::makeParameters(LockType::Type::NamedMutex);
      auto alock = InterprocessLockFactory::getInterprocessLock(params);

      const auto start = std::chrono::steady_clock::now();
      auto timeExceeded = [&]() { return ((std::chrono::steady_clock::now() - start) > std::chrono::seconds(2)); };
      while (!timeExceeded()) {
        if (alock->timedLock(300)) {
          umap[x]++;
          std::this_thread::sleep_for(std::chrono::milliseconds(10 * x + 2)); // critical section
          alock->unlock();
        }
      }
    },
                                  i));
  }

  std::for_each(workers.begin(), workers.end(), [](std::thread& t) {
    t.join();
  });

  //std::cout << "umap state" << std::endl;

  for (auto& el : umap) {
    //std::cout << el.first << " " << el.second << std::endl;
    if (el.second < 1) {
      BOOST_CHECK(false);
    }
  }

  BOOST_CHECK(true);
}
BOOST_AUTO_TEST_SUITE_END()
