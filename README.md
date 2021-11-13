**13-11-21: Rishabh’s session**



* This is same as I node preparation gone through GATE
    * Single and double pointers etc.,
* Working Demo is in recording session
* For read, write and append, we have to first open the file first	
    * If it is not opened, the file descriptor is lost and it must throw an error
* In write mode, we need to clear the file first (write from the start)
* Cannot delete the file when opened, we need to first close it
* When we unmount a disk, all files should be closed
* Spaces in file name are not mandatory
* Consider disk as a file
    * Know how to create a file of given larger size (500mb) and store it as disk
* Use priority queues?
    * FD can be any number, not least available integer