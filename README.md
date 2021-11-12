- Gave RWX permissions to User,Group and Others for newly created file by default  
- If anything strange happens, clear the memory using bzero(it is the culprit)  
- Since client and server are same, I have not used generic int32_t when transfering data

# **don't forget to close FD for server file after copying is done**  

# tr -dc "A-Za-z 0-9" < /dev/urandom | fold -w1023 |head -n 1024 > bigfile.txt ==> to generate a file of size (1023+1)*(1024) ==> 1MB

# Redundant code in server and tracker classes, fix it

# Tokenizing logic is same, extract it from P2P and tracker?

# If any segfault, deallocate memory whereever needed

# didn't handled read write mode (when user needs to share his chunks while downloading as well)  



# ASSUMPTIONS
- assumed that username and password have no spaces
- assumed that total cumulative sizes of group names is less than 1024 
- assumed total path of filename will not exceed 95 chars
- Assumed whatever requests made by peer will be received in 1 shot from the tracker 
  - above should work since the maximum file size to be tested is 100mb ==> 200 chunks ==> 200*40 ==> 8000 bytes which should be transfered in one shot typically


# TODO  
- Handle spaces in file name at client side
- Flush everywhere cout is present
- increase all array size to CHUNK size to be on safe side and declare it using malloc so that we can deallocate easily
- Try to incorporate port numbers taken from runtime arguments

# TODO LATER
- Use STD threads to pass arguments in C++ instead of using pthreads for better handling  
- Implement piece selection algorithm