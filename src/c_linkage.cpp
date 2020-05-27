#include "../include/NetworkStatisticsClient.hpp"


class CListenerWrapper : public NetworkStatisticsListener
{
public:
  CListenerWrapper(listener_func f, void *user_context) {
    this->listener = f;
    this->user_context = user_context;
  }

  virtual void onStreamAdded(const NTStatStream *stream)
  {
    this->listener(ACTION_ADD, stream, user_context);
  }
  virtual void onStreamRemoved(const NTStatStream *stream)
  {
    this->listener(ACTION_DEL, stream, user_context);
  }
  virtual void onStreamStatsUpdate(const NTStatStream *stream)
  {
    this->listener(ACTION_UPDATE, stream, user_context);
  }
private:
  listener_func listener;
  void *user_context;
};

extern "C" {
  int run_ntstats(listener_func f, void *user_context, int32_t interval_sec, int32_t want_tcp, int32_t want_udp) {
    CListenerWrapper listener = CListenerWrapper(f, user_context);
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