#!/bin/bash

#zypper in sshpass
#sshpass -p "4Wr/WrCB" scp -r ../Intra-task_DVFS s1610105@tlab21:~/Intra-task_DVFS_$1 &&
#sshpass -p "4Wr/WrCB" scp -r ../Intra-task_DVFS s1610105@lin01.jaist.ac.jp:~/Intra-task_DVFS_$1

#scp -r -W bak_password.txt ../Intra-task_DVFS s1610105@tlab21:~/Intra-task_DVFS_$1 &&
#scp -r -W bak_password.txt ../Intra-task_DVFS s1610105@lin01.jaist.ac.jp:~/Intra-task_DVFS_$1

scp -r /home/s1610105/Documents/Intra-task_DVFS s1610105@tlab21:~/Documents/research/Master/Practices
