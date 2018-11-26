#include <iostream>
#include <random>
#include <cmath>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), windowSize(14), packetsPerSecond(0), poissonValue(0), packetCounter(0), currentTime(0), bValue(1)
{}

/* Get current window size, in datagrams */
unsigned int Controller::window_size()
{

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
    windowSize = windowSize / 2; 
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
  
  packetCounter++;
  const uint64_t rtt = timestamp_ack_received - send_timestamp_acked;
  currentTime += rtt;
  packetsPerSecond = (packetCounter * 1000)/currentTime;

  poissonValue = poissonCalc();
  
  if(rtt > 90 && poissonValue > 0.8) {
    const double tick = currentTime/1000.0;
    const double constant = bayesianUpdate(bValue, tick);
    bValue = constant < 0.09 ? constant * 10.0 : constant;
    bValue = bValue > 0.9 ? constant/10.0 : bValue;

    windowSize = scheduler(tick, bValue);
    const uint64_t limitUp = 20;
    const uint64_t limitDown = 10;
    windowSize = windowSize % limitUp;
    windowSize = windowSize < limitDown ? 14 : windowSize;
  
  }
  
  if(packetCounter == 100) { 
    packetCounter = 0;
    currentTime = 0;
  }

   cerr << "pvalue " << poissonValue
	 << " window size is " << windowSize
   << " Bvalue " << bValue 
   << " tick " << currentTime/1000
   << " ps: " << packetsPerSecond << endl;
   cerr << " rtt: " << rtt << endl;
  
  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << endl;
  }
}

double Controller::bayesianUpdate(const double oldValue, const uint64_t tick){
  cerr << "ov " << oldValue << endl;
  cerr << "pc " << packetsPerSecond << endl;
  cerr << "tick " << tick << endl;

  
  if(oldValue <= 1) {
    return (packetsPerSecond * poissonValue);
  }
  return (oldValue * poissonValue);
}

double Controller::poissonCalc(){  
  std::default_random_engine generator;
  std::poisson_distribution<int> distribution(packetCounter);
  
  return distribution(generator);
}

int Controller::scheduler(const int times, const double alpha){
  double newAlpha = alpha > 1 ? alpha / 10.0 : alpha;
  double average =  avg(times);
  double count = ((1 - newAlpha) * average)/times;

  cerr << "alpha " << alpha << endl;
  cerr << "newAlpha " << newAlpha << endl;
  cerr << "times " << times << endl;
  cerr << "average " << average << endl;
  cerr << "count " << count << endl;

  while(count < 5) {
    count = count * 10.0 + 5;
  }

  return abs(count);
}

uint64_t Controller::avg(const uint64_t value) {
  if(value == 1) {
    return 1;
  }

  return avg(value - 1) + (value - avg(value - 1))/value;
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms()
{
  return 1000; /* timeout of one second */
}
