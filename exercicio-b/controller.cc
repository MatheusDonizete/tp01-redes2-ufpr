#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), windowSize(50), constant(3), lastRTT(100), packetCounter(0)
{}

/* Get current window size, in datagrams */
unsigned int Controller::window_size()
{
  /* Default: fixed window size of 100 outstanding datagrams */
  // unsigned int the_window_size = 50;

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
   << " window size is " << windowSize << endl;
  }

  return windowSize;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
            /* of the sent datagram */
            const uint64_t send_timestamp,
                                    /* in milliseconds */
            const bool after_timeout
            /* datagram was sent because of a timeout */ )
{
  /* Default: take no action */
  if(after_timeout) {
    windowSize = windowSize / constant; //reduz tamanho da janela a metade no caso 
  }

  if ( debug_ ) {
    cerr << "At time " << send_timestamp
   << " sent datagram " << sequence_number << " (timeout = " << after_timeout << ")\n";
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
             /* what sequence number was acknowledged */
             const uint64_t send_timestamp_acked,
             /* when the acknowledged datagram was sent (sender's clock) */
             const uint64_t recv_timestamp_acked,
             /* when the acknowledged datagram was received (receiver's clock)*/
             const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  /* Default: take no action */
  const uint64_t rtt = timestamp_ack_received - send_timestamp_acked;
  windowSize = updateSize(windowSize, 1/constant, rtt >= 75); 
  windowSize = windowSize <= 13 ? 13 : windowSize; 
  lastRTT = rtt;

  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
   << " received ack for datagram " << sequence_number_acked
   << " (send @ time " << send_timestamp_acked
   << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
   << ", last RTT @" << lastRTT
   << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms()
{
  return 1000; /* timeout of one second */
}


unsigned int Controller::updateSize(const uint64_t wTime, const uint64_t factor, const bool isConflict){
  if(packetCounter >= wTime) {    
    packetCounter = 0;
    if(isConflict){
      return wTime * factor;
    }

    return wTime + 1;
  } 
  

  packetCounter++;
  return wTime;
}
