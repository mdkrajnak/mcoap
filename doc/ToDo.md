mc_endpt_udp_recv(): Should it automatically ack confirmable messages? 

  * If it does, we won't be able to piggy back responses. 
  * If it does not then the source address of the message to confirm may not be available!
    * Consider adding source address field to message structure (only fill in for received messages). ***
  
How do we do piggybacked responses with acks?

Need to complete thread logic for mc_endpt_udp_start() to manage retransmits and receipts.

  * One looping thread that does both or two so we can choose one or the other or both?
  * How do we manage sharing the socket for send/recv operations? Locking.
  * How do we manage sharing the socket for sending first time messages and retransmits?
  
mc_endpt_read_fn_t: should the first argument be the endpoint?
  
Start a high level programmers manual.

Need top level test applications, testing method using standard tools like Californium?

