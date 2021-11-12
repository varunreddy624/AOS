Create a new txt file called **tracker_info.txt** which contains IP and port of tracker and must be in same directory as client and tracker

to use SHA1 library in c  (for ubuntu)



* Install OpenSSL library -** sudo apt-get install libssl-dev**
* If other distro, use corresponding library for the distro in step 1

**How to run tracker**

g++ tracker.cpp -o tracker -lpthread -lcrypto

./tracker 1 tracker_info.txt

**How to run client**

g++ client.cpp -o client -lpthread -lcrypto

./client 127.0.0.2 tracker_info.txt

**COMMAND FORMATS**



* Tracker:
    * Run Tracker: .**/tracker tracker_no tracker_info.txt**
        * tracker_info.txt - Contains ip, port details of all the trackers
    * Close Tracker: **quit**
* Client:
    *  Run Client: **./client &lt;IP>:&lt;PORT> tracker_info.txt**
        * tracker_info.txt - Contains ip, port details of all the trackers
    * Create User Account: **create_user &lt;user_id> &lt;passwd>**
    * Login: **login &lt;user_id> &lt;passwd>**
    * Create Group: **create_group &lt;group_id>**
    * Join Group:** join_group &lt;group_id>**
    * Leave Group: **leave_group &lt;group_id>**
    * List pending join requests: **list_requests &lt;group_id>**
    * Accept Group Joining Request: **accept_request &lt;group_id> &lt;user_id>**
    * List All Group In Network: **list_groups**
    * List All sharable Files In Group: **list_files &lt;group_id>**
    * Upload File:** upload_file &lt;file_path> &lt;group_id >**
        * Whole file path must not exceed 95 characters, if found more than that, rename the file and make sure it is less than 95 characters
    * Download File: **download_file &lt;group_id> &lt;file_name> &lt;destination_path>**
    * Logout: **logout**
    * Stop sharing:** stop_share &lt;group_id> &lt;file_name>**
* **If any command doesn’t work, take a deep breath and check the original cpp file to view the exact command**  

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