#include <SPI.h>
#include <Ethernet.h>

#include <Twitter.h>

void setup(){
    pinMode(7, INPUT);
}

void loop(){

    if(digitalRead(7)){
        tweetMessage();
        delay(1000);
    }
}

void tweetMessage(){
    Twitter twitter("122389426-X1UImGRsnRyjsUISCLM9HVd1OyHaytcxx7jJQElK");
    //Our message (in lolcat, of course)
    String stringMsg = "All ur lightz be ";
    stringMsg += analogRead(0);
    stringMsg += " out of 1023. #tweet #from #edison #board #IoT";

    //Convert our message to a character array
    char msg[140];
    stringMsg.toCharArray(msg, 140);

    //Tweet that sucker!
    twitter.post(msg);
}
