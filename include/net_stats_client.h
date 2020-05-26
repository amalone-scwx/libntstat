#ifndef _NET_STATS_CLIENT_H_
#define _NET_STATS_CLIENT_H_

#include <stdint.h>
#include <netinet/in.h>

// Data types for reporting

typedef union {
  struct in_addr addr4;
  struct in6_addr addr6;
} addr_t;

typedef struct NTStatCounters
{
  uint64_t rxpackets;       // failed if TCP && rxpackets == 0 && txpackets > 0
  uint64_t txpackets;
  uint64_t rxbytes;
  uint64_t txbytes;

  uint64_t cell_rxbytes;
  uint64_t cell_txbytes;
  uint64_t wifi_rxbytes;
  uint64_t wifi_txbytes;
  uint64_t wired_rxbytes;  // wnot present for XNU v2422 and earlier
  uint64_t wired_txbytes;
}NTStatCounters;

typedef struct NTStatStreamState
{
  uint32_t state;      // TCPS_LISTEN, TCPS_SYN_SENT, etc.
  uint32_t txcwindow;  // TCP only
  uint32_t txwindow;   // TCP only
}NTStatStreamState;

typedef struct NTStatStreamKey
{
  uint8_t     isV6;
  uint8_t     ipproto; // IPPROTO_TCP or IPPROTO_UDP
  uint16_t    pad;
  uint32_t    ifindex;
  uint16_t    lport;   // local port (network-endian)
  uint16_t    rport;   // remove port (network-endian)
  addr_t      local;
  addr_t      remote;
}NTStatStreamKey;

typedef struct NTStatProcess
{
  uint32_t     pid;
  char         name[64];
}NTStatProcess;

typedef struct NTStatStream
{
  // these are constant once we see the stream
  uint64_t           id;     // Stream ID provided by kernel. Unique for app runtime. Can rollover at uint32_t, so id can be reused.
  NTStatStreamKey    key;
  NTStatProcess      process;

  // these get updated
  NTStatCounters     stats;
  NTStatStreamState  states;
} NTStatStream;


#define ACTION_ADD 1
#define ACTION_DEL 2
#define ACTION_UPDATE 3

typedef void (*listener_func)(int action, const NTStatStream*stream);

extern int run_ntstats(listener_func f, int32_t interval_sec, int32_t want_tcp, int32_t want_udp);

#endif // _NET_STATS_CLIENT_H_
