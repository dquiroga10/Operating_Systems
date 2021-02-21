 #!/bin/bash

shell=sshell
shell_source=$1

sudo bash ./script-a.sh $shell_source
sudo bash ./script-b.sh $shell_source
sudo bash ./script-cd.sh $shell_source
