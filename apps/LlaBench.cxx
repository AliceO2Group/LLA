// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file LlaBench.cxx
/// \brief Benchmarking program for the LLA library.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>
#include <unordered_map>

#include "Common/Program.h"
#include "ReadoutCard/CardDescriptor.h"
#include "ReadoutCard/ChannelFactory.h"
#include "ReadoutCard/Cru.h"
#include "Lla/Lla.h"

namespace roc = AliceO2::roc;
namespace po = boost::program_options;

namespace o2
{
namespace lla
{

class LlaBench : public AliceO2::Common::Program
{
 public:
  LlaBench()
  {
  }

  virtual Description getDescription() override
  {
    return { "LLA-BENCH", "Low-Level Arbitration Benchmarking program", "o2-lla-bench" };
  }

  virtual void addOptions(po::options_description& options) override
  {
    options.add_options()("session-name",
                          po::value<std::string>(&mOptions.sessionName)->default_value("##UNSET##"),
                          "The session name");
    options.add_options()("card-id",
                          po::value<int>(&mOptions.cardId)->default_value(-1),
                          "The card id");
    options.add_options()("runtime",
                          po::value<int>(&mOptions.runtime)->default_value(-1),
                          "Runtime in ms");
    options.add_options()("no-locking",
                          po::bool_switch(&mOptions.noLocking)->default_value(false),
                          "To lock or not to lock");
    options.add_options()("threads",
                          po::value<int>(&mOptions.threads)->default_value(1),
                          "Number of threds to spawn");
    options.add_options()("lock-type",
                          po::value<std::string>(&mOptions.lockTypeString)->default_value("socket-lock"),
                          "Type of lock to use ['socket-lock','named-mutex']");
  }

  virtual void run(const po::variables_map&) override
  {
    std::ios_base::sync_with_stdio(false);

    std::cin.tie(nullptr);
    std::cerr.tie(nullptr);

    if (mOptions.runtime == -1) {
      runForever = true;
    }

    std::cout << "Running Low-Level Arbitration Benchmarking program" << std::endl;

    simpleCritical = false;
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "================== SOCKET LOCK ==================" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    lockType = LockType::Type::SocketLock;
    runBenchmark(std::bind(&LlaBench::lockingOverhead, this, 100));
    std::cout << std::endl;
    runBenchmark(std::bind(&LlaBench::criticalSectionTimes, this));
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "================== NAMED MUTEX ==================" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    lockType = LockType::Type::NamedMutex;
    runBenchmark(std::bind(&LlaBench::lockingOverhead, this, 100));
    std::cout << std::endl;
    runBenchmark(std::bind(&LlaBench::criticalSectionTimes, this));
    return;
  }

  void runBenchmark(std::function<void()> fun)
  {
    fun();
  }

  // Locking Overhead w/ creations
  // Runs runSession and runRaw once for each loop iteration
  void lockingOverhead(int times)
  {
    std::cout << "Running locking overhead benchmark" << std::endl;
    std::cout << "Fine: Around the critical section (session included)" << std::endl;
    std::cout << "Coarse: Including object creation (Parameters, BAR interface, Session)" << std::endl;
    oneShot = true;
    std::unordered_map<int, double> umap;
    std::chrono::high_resolution_clock::time_point start, stop;
    long long averageSession = 0, averageRaw = 0, averageFineSession = 0, averageFineRaw = 0;

    for (int i = 0; i < times && !isSigInt(); i++) {
      startCounting = std::chrono::steady_clock::now();
      start = std::chrono::high_resolution_clock::now();
      averageFineSession = runSession(42, umap);
      stop = std::chrono::high_resolution_clock::now();
      averageSession += std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();

      startCounting = std::chrono::steady_clock::now();
      start = std::chrono::high_resolution_clock::now();
      averageFineRaw = runRaw(umap);
      stop = std::chrono::high_resolution_clock::now();
      averageRaw += std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    }

    oneShot = false;
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "Raw Fine Total: " << averageFineRaw / times << "ns" << std::endl; //TODO: Is this total or average??
    std::cout << "Session Fine Total: " << averageFineSession / times << "ns" << std::endl;
    std::cout << "Diff Fine Total: " << averageFineSession / times - averageFineRaw / times << "ns" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "Raw Coarse Average: " << averageRaw / times << "ms" << std::endl;
    std::cout << "Session Coarse Average: " << averageSession / times << "ms" << std::endl;
    std::cout << "Diff Coarse Average: " << averageSession / times - averageRaw / times << "ms" << std::endl;
    /*    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "RAW: # " << umap[-1] << std::endl;
    std::cout << "SESSION: # " << umap[42] << std::endl;
    std::cout << "DIFF: # " << umap[-1] - umap[42] << std::endl;
    std::cout << "DIFF: " << (double)(umap[-1] - umap[42])/umap[-1] * 100 << " %" << std::endl;*/
    std::cout << "-------------------------------------------------" << std::endl;
  }

  void criticalSectionTimes()
  {
    std::cout << "Running critical section times benchmark" << std::endl;

    std::vector<std::thread> workers;
    std::unordered_map<int, double> umap;
    std::unordered_map<int, double> rawumap;

    startCounting = std::chrono::steady_clock::now();
    runRaw(std::ref(rawumap));

    startCounting = std::chrono::steady_clock::now();
    for (int i = 0; i < mOptions.threads; i++) {
      workers.push_back(std::thread(&LlaBench::runSession, this, i, std::ref(umap)));
    }

    for (auto& t : workers) {
      t.join();
    }

    //TODO: Figure out a metric for thread distribution ->> stdev
    std::vector<int> times;

    int min = INT_MAX;
    int max = INT_MIN;
    for (const auto& el : umap) {
      if (el.first == -1) {
        continue;
      };
      min = std::min(min, (int)el.second);
      max = std::max(max, (int)el.second);
      times.push_back(el.second);
    }
    std::cout << "-------------------------------------------------" << std::endl;

    // Calculate standard distribution
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    double mean = sum / times.size();

    std::vector<double> diff(times.size());
    std::transform(times.begin(), times.end(), diff.begin(),
                   std::bind2nd(std::minus<double>(), mean));

    double squareSum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    double stdev = std::sqrt(squareSum / times.size());
    // End calculate standard distribution

    std::cout << "RAW: " << rawumap[-1] << std::endl;
    std::cout << "SESSION: " << sum << std::endl;
    std::cout << "DIFF: " << (double)rawumap[-1] - sum << std::endl;
    std::cout << "DIFF: " << (double)(rawumap[-1] - sum) / rawumap[-1] * 100 << " %" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "MIN: " << min << std::endl;
    std::cout << "MAX: " << max << std::endl;
    std::cout << "DIFF: " << max - min << std::endl;
    std::cout << "DIFF: " << (double)(max - min) / max * 100 << " %" << std::endl;
    std::cout << "MEAN: " << mean << std::endl;
    std::cout << "STDEV: " << stdev << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    /*    std::cout << "tid | #times in critical section" << std::endl;
    for (const auto& el : umap) {
      std::cout << el.first << " " << el.second << std::endl;
    }
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << std::endl;*/
  }

  long long runSession(int tid, std::unordered_map<int, double>& umap)
  {
    long long ret = 0;
    std::chrono::high_resolution_clock::time_point start, stop;

    SessionParameters params = SessionParameters::makeParameters()
                                 .setSessionName(mOptions.sessionName)
                                 .setCardId(mOptions.cardId);
    std::unique_ptr<Session> session = std::make_unique<Session>(params, lockType); // Class constructor only availabe when O2_LLA_BENCH_ENABLED is defined

    std::string cardSequence("#" + std::to_string(mOptions.cardId)); // TODO: This needs to be polished
    std::shared_ptr<roc::BarInterface> bar0, bar2;
    if (simpleCritical) {
      bar0 = roc::ChannelFactory().getBar(roc::PciSequenceNumber(cardSequence), 0);
    } else {
      bar2 = roc::ChannelFactory().getBar(roc::PciSequenceNumber(cardSequence), 2);
    }

    while ((!timeExceeded() || runForever) && !isSigInt()) {
      start = std::chrono::high_resolution_clock::now();
      if (session->timedStart(2000)) {
        umap[tid]++;

        if (simpleCritical) {
          criticalSimple(bar0, tid);
        } else {
          criticalSwt(bar2);
        }

        session->stop();
        stop = std::chrono::high_resolution_clock::now();
        ret += std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
      } else {
        std::cerr << tid << "Couldn't grab session lock" << std::endl;
        continue;
      }
      if (oneShot) {
        return ret;
      }
    }

    return ret;
  }

  long long runRaw(std::unordered_map<int, double>& umap)
  {
    long long ret = 0;
    std::chrono::high_resolution_clock::time_point start, stop;

    /*SessionParameters params = SessionParameters::makeParameters()
      .setSessionName(mOptions.sessionName)
      .setCardId(mOptions.cardId);
    Session session = Session(params);*/

    std::string cardSequence("#" + std::to_string(mOptions.cardId));
    std::shared_ptr<roc::BarInterface> bar0, bar2;
    if (simpleCritical) {
      bar0 = roc::ChannelFactory().getBar(roc::PciSequenceNumber(cardSequence), 0);
    } else {
      bar2 = roc::ChannelFactory().getBar(roc::PciSequenceNumber(cardSequence), 2);
    }

    while ((!timeExceeded() || runForever) && !isSigInt()) {
      umap[-1]++;
      start = std::chrono::high_resolution_clock::now();

      if (simpleCritical) {
        criticalSimple(bar0, 0x0badf00d);
      } else {
        criticalSwt(bar2);
      }

      stop = std::chrono::high_resolution_clock::now();
      ret += std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
      if (oneShot) {
        return ret;
      }
    }

    return ret;
  }

  void criticalSimple(std::shared_ptr<roc::BarInterface> bar, uint32_t wr)
  {
    uint32_t regIndex = 0xc00 / 4;
    bar->writeRegister(regIndex, wr);
    uint32_t rd = bar->readRegister(regIndex);
    if (rd != wr) {
      std::cerr << rd << " != " << wr << std::endl;
      BOOST_THROW_EXCEPTION(LlaException() << ErrorInfo::Message("oh boy"));
    }
  }

  void criticalSwt(std::shared_ptr<roc::BarInterface> bar)
  {
    namespace sc_regs = roc::Cru::ScRegisters;
    uint32_t wrLow = 0xcafecafe;
    uint32_t wrMed = 0x0badf00d;
    //    uint32_t wrHigh = 0xdeadbeef;

    bar->writeRegister(sc_regs::SC_LINK.index, 0);

    bar->writeRegister(sc_regs::SC_RESET.index, 0x1);
    bar->writeRegister(sc_regs::SC_RESET.index, 0x0);

    //    bar->writeRegister(sc_regs::SWT_WR_WORD_H.index, wrHigh);
    bar->writeRegister(sc_regs::SWT_WR_WORD_M.index, wrMed);
    bar->writeRegister(sc_regs::SWT_WR_WORD_L.index, wrLow);

    bar->readRegister(sc_regs::SWT_MON.index);

    // ---

    auto timeOut = std::chrono::steady_clock::now() + std::chrono::milliseconds(10);

    uint32_t numWords = 0x0;
    while ((std::chrono::steady_clock::now() < timeOut) && (numWords < 1)) {
      numWords = (bar->readRegister(sc_regs::SWT_MON.index) >> 16);
    }

    if (numWords < 1) {
      std::cout << "oops" << std::endl;
    }

    for (int i = 0; i < (int)numWords; i++) {
      bar->writeRegister(sc_regs::SWT_CMD.index, 0x2);
      bar->writeRegister(sc_regs::SWT_CMD.index, 0x0);

      uint32_t rdLow = bar->readRegister(sc_regs::SWT_RD_WORD_L.index);
      uint32_t rdMed = bar->readRegister(sc_regs::SWT_RD_WORD_M.index);
      //uint32_t rdHigh = bar->readRegister(sc_regs::SWT_RD_WORD_H.index);

      if (rdLow != wrLow || rdMed != wrMed /*|| rdHigh != wrHigh*/) {
        std::cout << std::hex << rdLow << " " << wrLow << std::endl;
        std::cout << std::hex << rdMed << " " << wrMed << std::endl;
        //std::cout << std::hex << rdHigh << " " << wrHigh << std::endl;
        BOOST_THROW_EXCEPTION(LlaException() << ErrorInfo::Message("oh girl"));
      }
    }
  }

 private:
  bool timeExceeded() { return ((std::chrono::steady_clock::now() - startCounting) > std::chrono::milliseconds(mOptions.runtime)); };

  struct OptionsStruct {
    std::string sessionName = "";
    int cardId = -1;
    int runtime = 1000;
    bool noLocking = false;
    int threads = 1;
    std::string lockTypeString = "socket-lock";
  } mOptions;

  bool runForever = false;
  bool oneShot = false;
  std::chrono::steady_clock::time_point startCounting;
  bool simpleCritical = true;

  LockType::Type lockType = LockType::Type::SocketLock;
};

} // namespace lla
} // namespace o2

int main(int argc, char** argv)
{
  return o2::lla::LlaBench().execute(argc, argv);
}
