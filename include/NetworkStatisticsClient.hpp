//  NetworkStatisticsClient.hpp
//  Copyright © 2017 Alex Malone. All rights reserved.

#ifndef NetworkStatisticsClient_hpp
#define NetworkStatisticsClient_hpp

extern "C" {
  #include "net_stats_client.h"
}

/*
 * NetworkStatisticsListener
 *
 * Applications need to implement this interface to receive callbacks
 * from the NetworkStatisticsClient.
 *
 * Note: Darwin reports bidirectional UDP conversations as two separate streams
 *
 */
class NetworkStatisticsListener
{
public:
  virtual void onStreamAdded(const NTStatStream *stream)=0;

  virtual void onStreamRemoved(const NTStatStream *stream)=0;

  virtual void onStreamStatsUpdate(const NTStatStream *stream)=0;
};

// this is for testing... you can ignore
class NTStatClientEmulation
{
public:
  // saves all messages (requests and responses) to "./ntstat-xnu-<version>.bin"
  virtual void enableRecording() = 0;

  // in the place of run(), this will process messages from filename
  virtual void runRecording(char *filename, unsigned int xnuVersion) = 0;
};

const int NTSTAT_LOGF_ERROR    = (1 << 1);
const int NTSTAT_LOGF_SENDRECV = (1 << 2);
const int NTSTAT_LOGF_DEBUG    = (1 << 3);
const int NTSTAT_LOGF_TRACE    = (1 << 4);
const int NTSTAT_LOGF_DROPS    = (1 << 5);

/*
 * NetworkStatisticsClient
 *
 * This is the interface to interact with the com.apple.network.statistics data.
 * Behind the scenes, the implementation creates a system socket, subscribes
 * to TCP and UDP sources, and passes along the data to the listener.
 */
class NetworkStatisticsClient : public NTStatClientEmulation
{
public:
  /*
   * This should be done before call to run().
   */
  virtual bool connectToKernel() = 0;

  /*
   * returns true if connectToKernel() was called and was successful.
   */
  virtual bool isConnected() = 0;

  /*
   * Blocking: run from dedicated thread.  Subscribes to TCP/UDP and continuously reads messages.
   * The connection to the kernel will be disconnected when run() exits.
   */
  virtual void run() = 0;

  /*
   * configure() - needs to be called prior to run()
   *
   * @param wantTcp If true, include TCP streams. Default: true.
   * @param wantUdp If true, include UDP streams. Default: false.
   * @param updateIntervalSeconds  Interval in seconds to receive stat updates on persistent
   *                               connections.  If zero, this feature is turned off. Default: 30.
   */
  virtual void configure(bool wantTcp, bool wantUdp, uint32_t updateIntervalSeconds) = 0;

  /*
   * Will set the stop flag, so run() will exit.
   */
  virtual void stop() = 0;

  /*
   * configure logging. Default flags == 0, no logging.
   */
  virtual void setLogging(uint8_t flags) = 0;

  /*
   * returns the number of ENOBUFS errors received from kernel, indicating
   * the inability to send some requested information due to full buffer.
   * See docs/protocol.md for more details.
   */
  virtual uint32_t getNumDrops() = 0;

};

// Instantiate (singleton) the NetworkStatisticsClient

NetworkStatisticsClient* NetworkStatisticsClientNew(NetworkStatisticsListener* l);


struct NTStatStreamKeyCpp : NTStatStreamKey
{
  bool operator<(const NTStatStreamKey& b) const; // needed to be a key type for std::map
};


/*
#include <string>
struct NTStatInterface
{
  std::string                     name;
  uint32_t                        ifindex;
  uint64_t                        threshold;
  unsigned int                    type;
  std::string                     description;
};
*/

// convenience macros

#define IS_LISTEN_PORT(pstream) (0 == (pstream)->key.rport)

#endif /* NetworkStatisticsClient_hpp */
