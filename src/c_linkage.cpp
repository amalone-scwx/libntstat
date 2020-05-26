#include "../include/NetworkStatisticsClient.hpp"


class CListenerWrapper : public NetworkStatisticsListener
{
public:
  CListenerWrapper(listener_func f) {
    this->listener = f;
  }

  virtual void onStreamAdded(const NTStatStream *stream)
  {
    this->listener(ACTION_ADD, stream);
  }
  virtual void onStreamRemoved(const NTStatStream *stream)
  {
    this->listener(ACTION_DEL, stream);
  }
  virtual void onStreamStatsUpdate(const NTStatStream *stream)
  {
    this->listener(ACTION_UPDATE, stream);
  }
private:
  listener_func listener;
};

extern "C" {
  int run_ntstats(listener_func f, int32_t interval_sec, int32_t want_tcp, int32_t want_udp) {
    CListenerWrapper listener = CListenerWrapper(f);
    NetworkStatisticsClient* netstatClient =  NetworkStatisticsClientNew(&listener);

    // connect to ntstat via kernel control module socket
    
    if (false == netstatClient->connectToKernel()) {
      return 2;
    }

    // configure
    netstatClient->configure(want_tcp, want_udp, interval_sec);

    // in a real app, we would want to run this in a dedicated thread
    netstatClient->run();
  }
}