#include <chrono>
#include <iostream>
#include <thread>

#include <Lla/Lla.h>

namespace lla = o2::lla;

int main()
{
  lla::SessionParameters params = lla::SessionParameters::makeParameters("example sess", "3b:00.0");
  lla::Session session = lla::Session(params);

  if (session.start()) {
    std::this_thread::sleep_for(std::chrono::seconds(4)); // critical section
    session.stop();
  } else {
    std::cerr << "Could not start session" << std::endl;
  }

  auto params2 = lla::SessionParameters::makeParameters()
                   .setSessionName("example sess")
                   .setCardId("#3");
  auto session2 = lla::Session(params2);
  if (session2.timedStart(1000)) {
    std::this_thread::sleep_for(std::chrono::seconds(4)); // critical section
    session2.stop();
  } else {
    std::cerr << "Could not start session after 1000 ms" << std::endl;
  }

  return 0;
}
