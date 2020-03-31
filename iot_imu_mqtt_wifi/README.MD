#Test case ID : pressed_toggle_1

#Description:
Objective
To change the label when collect the data.

Test 
When press the user button on the board, the label change.
The led status on board change.

#Procedure
Press the user_button switch

#Test data
The label should change 
0 to 1  
led_blue -> led_yellow
or 
1 to 0
led_yellow -> led_blue

#Expected result
status : walk
//device_id, status, seq, pGyroDataXYZ[0], ...
1,1,1,<value>,...
<led_yellow>

<toggle>
status : idle
//device_id, status, seq, pGyroDataXYZ[0], ...
1,0,1,<value>,...
<led_blue>
