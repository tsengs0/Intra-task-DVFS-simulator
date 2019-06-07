#!/bin/bash

#in_alpha=$1
temp=3

if [ $# -eq 0 ]
  then
    echo "No arguments supplied, please give the Trial#"
    exit 1
fi

mkdir ../result/csv/Experiment_$1 &&
mkdir ../result/identifier/Experiment_$1

#   Created by: Ian Brown (ijbrown@hotmail.com)
#   Please share with me your modifications
# Note: From http://stackoverflow.com/questions/11592583/bash-progress-bar
# Functions
PUT(){ echo -en "\033[${1};${2}H";}  
DRAW(){ echo -en "\033%";echo -en "\033(0";}         
WRITE(){ echo -en "\033(B";}  
HIDECURSOR(){ echo -en "\033[?25l";} 
NORM(){ echo -en "\033[?12l\033[?25h";}
function showBar {
        percDone=$(echo 'scale=2;'$1/$2*100 | bc)
        halfDone=$(echo $percDone/2 | bc) #I prefer a half sized bar graph
        barLen=$(echo ${percDone%'.00'})
        halfDone=`expr $halfDone + 6`
        tput bold
        PUT $(( $3 + 2  )) 28; printf "%4.4s  " $barLen%     #Print the percentage
        PUT $3 $halfDone;  echo -e "\033[7m \033[0m" #Draw the bar
        tput sgr0
        }
# Start Script
clear
for in_alpha in {40..100..5};
do

	#alpha_tsk0=45
	alpha_tsk0=$in_alpha
	alpha_tsk1=$in_alpha
	#alpha_tsk1=45
	#alpha_tsk2=$(( 100 - alpha_tsk1 + 40 ))
	alpha_tsk2=$in_alpha
	#alpha_tsk2=45
	#alpha_tsk3=45
	alpha_tsk3=$in_alpha
	#alpha_tsk4=45
	alpha_tsk4=$in_alpha

#HIDECURSOR
echo -e ""                                           
echo -e ""                                          
DRAW    #magic starts here - must use caps in draw mode    
echo "Task 0 Alapha: " $alpha_tsk0 ", Task 1 Alpha: " $alpha_tsk1 ", Task 2 Alpha: " $alpha_tsk2 ", TaSK 3 Alpha: " $alpha_tsk3 ", Task 4 Alpha: " $alpha_tsk4
echo -e "          PLEASE WAIT WHILE SIMULATION IS IN PROGRESS"
echo -e "    lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk"  
#echo -e "    |---------------------------------------------------|"  
echo -e "    x                                                   x" 
#echo -e "    |                                                   |" 
echo -e "    mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj"
#echo -e "    |---------------------------------------------------|"  
WRITE
#
# Insert your script here 
 for i in {0..2..1}
   do
	 
 	../bin/mul $alpha_tsk0 $alpha_tsk1 $alpha_tsk2  $alpha_tsk3 $alpha_tsk4 1000 > log.txt 
	showBar $i 10 $((temp)) #Call bar drawing function "showBar"
   done
   mkdir ../result/identifier/Experiment_$1/Alpha_$in_alpha &&
   mkdir ../result/csv/Experiment_$1/Alpha_$in_alpha &&
   mv *.txt ../result/identifier/Experiment_$1/Alpha_$in_alpha &&
   mv *.csv ../result/csv/Experiment_$1/Alpha_$in_alpha
# End of your script
# Clean up at end of script
PUT 10 12
echo -e ""                                        
NORM

clear
#temp=$(( temp+10 ))
done

