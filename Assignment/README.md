README
=========

### REQUIREMENTS

- Linux environment with GCC (GNU Compiler Collection)  
- Make utility  

#### Check if the requirements are satisfied by using these commands.

    gcc –version  
    make –version  

#### If in any case you cannot find any of these requirements, Install them by using these commands.

    sudo apt install GCC  
    sudo apt install make -y  


### HOW TO COMPILE

#### To compile the program use the following command in the terminal.

    make

#### If you want a clean compilation, use the following two commands respectively.

    make clean
    make  

### HOW TO RUN

#### To run the program use the following command.  
*Note: You need to replace m, tc, tw, td, ti values with positive integers.*  

    ./cq m tc tw td ti  

> m -  length of customer queue.  
> tc - periodic time for the customer to arrive in the queue.  
> tw - time duration to serve withdrawal.  
> td - time duration to serve deposit.  
> ti - time duration to serve information.  
