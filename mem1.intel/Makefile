###############################################################
#   Students' Makefile for Operating Systems Project          #
#                     Use As Is                               #
###############################################################


# The assignment directory 
ASG_DIR = /home/osp/mem1.intel/

# The default C compiler 
C_COMP = cc 


#make OSP:  

OSP : $(ASG_DIR)osp.o dialog.o memory.o 
	$(C_COMP) $(ASG_DIR)osp.o dialog.o memory.o  -lm -g -o OSP

dialog.o : dialog.c
	$(C_COMP) -c -g dialog.c

memory.o : memory.c 
	$(C_COMP) -c -g memory.c 

