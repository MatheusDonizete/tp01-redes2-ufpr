#ifndef CONTROLLER_HH
#define CONTROLLER_HH

#include <cstdint>

/* Congestion controller interface */

class Controller
{
private:
  bool debug_; /* Enables debugging output */

  /* Add member variables here */
  uint64_t windowSize;
  double packetsPerSecond;
  double poissonValue;
  uint64_t packetCounter;
  uint64_t currentTime;
  double bValue;

public:
  /* Public interface for the congestion controller */
  /* You can change these if you prefer, but will need to change
     the call site as well (in sender.cc) */

  /* Default constructor */
  Controller( const bool debug );

  /* Get current window size, in datagrams */
  unsigned int window_size();

  /* A datagram was sent */
  void datagram_was_sent( const uint64_t sequence_number,
			  const uint64_t send_timestamp,
			  const bool after_timeout );

  /* An ack was received */
  void ack_received( const uint64_t sequence_number_acked,
		     const uint64_t send_timestamp_acked,
		     const uint64_t recv_timestamp_acked,
		     const uint64_t timestamp_ack_received );

  /* How long to wait (in milliseconds) if there are no acks
     before sending one more datagram */
  unsigned int timeout_ms();

  /* função que calcula a atualização bayesiana
  * @param integer packetsPerSecond: número de pacotes por segundo (0 - 1000)
  * @return 
  */
  double bayesianUpdate(const double oldValue, const uint64_t tick);

  double poissonCalc();

  uint64_t avg(const uint64_t value);
  
  int scheduler(const int times, const double alpha);  
};

#endif
