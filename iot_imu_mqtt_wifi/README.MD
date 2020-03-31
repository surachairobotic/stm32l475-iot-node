# Test case ID : pressed_toggle_1

# Description:
To change the label when collect the data.  
When press the user button on the board, the label change.  
The led status on board change.  

# Procedure
Press the user_button switch.  

# Test data
The label should change.  
0 to 1  
led_blue -> led_yellow  

or  

1 to 0  
led_yellow -> led_blue  

# Expected result
```
status : walk  
1,1,1,<value>,...  
```
//device_id, status, seq, pGyroDataXYZ[0], ...  

<led_yellow>  

<toggle>  

```
status : idle  
1,0,1,<value>,...  
```
//device_id, status, seq, pGyroDataXYZ[0], ...  

<led_blue>  

# Test case : speed of MQTT protocol.
## Description
Test to find max speed and accuracy of systems
## Procedure
Use the iot node to send data to NETPIE and subscribe with python and observe the data loss and speed by varying the amount of data sent between 1-1024 characters.
## Test Data :
> S100,D512,F10  
> 1DDDDD … DDD <--  num of charactor for each line is 512  
> 2DDDDD … DDD  
> 3DDDDD … DDD  
> .  
> 100DDDDD … DDD
## Expected results
When the python side receives the header information, it will immediately know what the data will be sent with the pattern, making it possible to check the accuracy of the data.

