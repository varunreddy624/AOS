#include<stdio.h>
#include<stdlib.h>
#include<bits/stdc++.h>

using namespace std;


// SUPER BLOCK STRUCT
typedef struct S{
    int number_of_inodes;      // 4 BYTES
    int number_of_data_blocks;  // 4 BYTES
}S; 


//INODE STRUCT
typedef struct INODE{
    int starting_disk_block;            // 4 BYTES
    int ending_disk_block;              // 4 BYTES
    int size_in_bytes;                  // 4 BYTES
    char mode_opened;                   // 1 BYTE
    char name[40];                      // 40 BYTES
}INODE;


//DISK BLOCK STRUCT
typedef struct DATA_BLOCK{
    int next_data_block_num;     // 4 BYTES
    char data[512];             // 512 BYTES
}DATA_BLOCK;

int number_of_inodes = 56;
int number_of_data_blocks = 56;


class DISK{
    public:

    string filename;

    S new_disk_sb; // super block
    INODE* inode_array;
    DATA_BLOCK* disk_blocks_array; // arrays pointing to the respective entries
    bool *inode_bitmap,*disk_block_bitmap;

    unordered_map<string,int> fileNameToInodeNum;

    unordered_set<string> openedFiles;



    DISK(string diskname){
        filename = diskname;
    }

    void create_disk(){
        int i;


        new_disk_sb.number_of_inodes=number_of_inodes;
        new_disk_sb.number_of_data_blocks = number_of_data_blocks;

        // initialize inode and disk_block bitmaps
        inode_bitmap = (bool*)malloc(sizeof(bool)*new_disk_sb.number_of_inodes);
        disk_block_bitmap = (bool*)malloc(sizeof(bool)*new_disk_sb.number_of_data_blocks);

        // initialize inode array
        inode_array = (INODE*)malloc(sizeof(INODE)*new_disk_sb.number_of_inodes);
        for(i=0;i<new_disk_sb.number_of_inodes;i++){
            strcpy(inode_array[i].name,"");
            inode_array[i].starting_disk_block=-1;
            inode_array[i].ending_disk_block=-1;
            inode_array[i].mode_opened=' ';
            inode_array[i].size_in_bytes=0;
            inode_bitmap[i]=true;
        }

        // initialize disk block array
        disk_blocks_array = (DATA_BLOCK*)malloc(sizeof(DATA_BLOCK)*new_disk_sb.number_of_data_blocks);
        for(i=0;i<new_disk_sb.number_of_data_blocks;i++){
            strcpy(disk_blocks_array[i].data,"empty");
            disk_blocks_array[i].next_data_block_num=-1;
            disk_block_bitmap[i]=true;
        }

        unmount_disk();
    }

    void unmount_disk(){
        FILE* new_disk_data_fp = fopen(filename.c_str(),"w+");

        // write super block
        fwrite(&new_disk_sb,sizeof(S),1,new_disk_data_fp);


        // write inode bitmap
        fwrite(inode_bitmap,sizeof(bool),new_disk_sb.number_of_inodes,new_disk_data_fp);


        // write disk_block_arr bitmap
        fwrite(disk_block_bitmap,sizeof(bool),new_disk_sb.number_of_data_blocks,new_disk_data_fp);
            

        // write inode metada
        fwrite(inode_array,sizeof(INODE),new_disk_sb.number_of_inodes,new_disk_data_fp);


        // write disk block array metadata
        fwrite(disk_blocks_array,sizeof(DATA_BLOCK),new_disk_sb.number_of_data_blocks,new_disk_data_fp);
        

        fclose(new_disk_data_fp);

        free(inode_array);
        free(disk_blocks_array);
        free(inode_bitmap);
        free(disk_block_bitmap);
    }

    void mount_disk(){
        FILE* new_disk_data_fp = fopen(filename.c_str(),"r");
        // read super block
        fread(&new_disk_sb,sizeof(S),1,new_disk_data_fp);

        int numberofinodes = new_disk_sb.number_of_inodes;
        int numberofdatablocks = new_disk_sb.number_of_data_blocks;

        inode_bitmap = (bool*)malloc(sizeof(bool)*numberofinodes);
        disk_block_bitmap = (bool*)malloc(sizeof(bool)*numberofdatablocks);
        
        int i;

        // write inode bitmap
        fread(inode_bitmap,sizeof(bool),new_disk_sb.number_of_inodes,new_disk_data_fp);


        // write disk_block_arr bitmap
        fread(disk_block_bitmap,sizeof(bool),new_disk_sb.number_of_data_blocks,new_disk_data_fp);
            

        // write inode metada
        inode_array = (INODE*)malloc(sizeof(INODE)*new_disk_sb.number_of_inodes);
        fread(inode_array,sizeof(INODE),new_disk_sb.number_of_inodes,new_disk_data_fp);


        // write disk block array metadata
        disk_blocks_array = (DATA_BLOCK*)malloc(sizeof(DATA_BLOCK)*new_disk_sb.number_of_data_blocks);
        fread(disk_blocks_array,sizeof(DATA_BLOCK),new_disk_sb.number_of_data_blocks,new_disk_data_fp);


        fclose(new_disk_data_fp);

        // print_fs_info(new_disk_sb,inode_bitmap,disk_block_bitmap,inode_array,disk_blocks_array);
    }

    void print_fs_info(S s,bool* inode_bitmap, bool* disk_block_bitmap,INODE* inode_array,DATA_BLOCK* disk_block_arr){

        printf("number of inodes - %d\n",s.number_of_inodes);
        printf("number of data blocks - %d\n",s.number_of_data_blocks);
        
        for(int i=0;i<s.number_of_inodes;i++)
            cout << inode_bitmap[i]<< " " << flush;
        cout << endl << flush;

        for(int i=0;i<s.number_of_data_blocks;i++)
            cout << disk_block_bitmap[i] << " " << flush;
        cout << endl << flush;

        for(int i=0;i<s.number_of_inodes;i++)
            cout << inode_array[i].starting_disk_block << "," << inode_array[i].mode_opened << " " << flush;
        cout << endl << flush;

        for(int i=0;i<s.number_of_data_blocks;i++)
            cout << disk_block_arr[i].next_data_block_num << " " << flush;
        cout << endl << flush;

    }

    int getFirstFreeInode(){
        for(int i=0;i<new_disk_sb.number_of_inodes;i++){
            if(inode_bitmap[i]){
                inode_bitmap[i]=false;
                return i;
            }
        }
        return -1;
    }
    
    int getFirstFreeDiskBlock(){
        for(int i=0;i<new_disk_sb.number_of_data_blocks;i++){
            if(disk_block_bitmap[i]){
                disk_block_bitmap[i]=false;
                return i;
            }
        }
        return -1;
    }

    void create_file(string filename){
        if(fileNameToInodeNum.find(filename)!=fileNameToInodeNum.end()){
            printf("file name already exists; enter proper file name\n");
            return ;
        }
        else{
            int firstFreeInode = getFirstFreeInode();
            fileNameToInodeNum.insert(make_pair(filename,firstFreeInode));
            strcpy(inode_array[firstFreeInode].name,filename.c_str());
            int firstFreeBlock = getFirstFreeDiskBlock();
            cout << firstFreeInode << " " << firstFreeBlock << endl;
            inode_array[firstFreeInode].starting_disk_block = firstFreeBlock;
            inode_array[firstFreeInode].ending_disk_block = firstFreeBlock;
            printf("file creation successfull\n");
        }
    }

    void open_file(string filename,char mode){
        if(fileNameToInodeNum.find(filename)==fileNameToInodeNum.end()){
            printf("file name doesn't exists; enter proper file name\n");
            return ;
        }
        else{
            if(openedFiles.find(filename)!=openedFiles.end()){
                printf("file is already opened; close it first\n");
                return ;
            }   
            else{
                int inodeOfFile = fileNameToInodeNum[filename];
                inode_array[inodeOfFile].mode_opened=mode;
                openedFiles.insert(filename);
                printf("file opened successfull\n");
            }
        }
    }

    void list_files(){
        for(auto it=fileNameToInodeNum.begin();it!=fileNameToInodeNum.end();it++){
            cout << it->first << endl;
        }
    }

    void list_opened_files(){
        for(auto it=openedFiles.begin();it!=openedFiles.end();it++){
            cout << (*it) << " opened in " << inode_array[fileNameToInodeNum[*it]].mode_opened << endl;
        }
    }

    void handleMenu(){
        int choice;
        string filename;
        char mode;
        while(1){
            printf("file handling menu\n");
            printf("1. create file\n");
            printf("2. open file\n");
            printf("3. read file\n");
            printf("4. write file\n");
            printf("5. append file\n");
            printf("6. close file\n");
            printf("7. delete file\n");
            printf("8. list of files\n");
            printf("9. list of opened files\n");
            printf("10. unmount disk\n");
            cin >> choice;
            if(choice==1){
                printf("enter file name\n");
                cin >> filename;
                create_file(filename);
            }
            else if(choice==2){
                printf("enter file name\n");
                cin >> filename;
                printf("enter mode r-read, w-write, a-append\n");
                cin >> mode;
                if(mode!='r' && mode!='w' && mode!='a')
                    printf("wrong option entered for mode; enter valid value\n");
                
                else
                    open_file(filename,mode);
            }
            else if(choice==8){
                list_files();
            }
            else if(choice==9){
                list_opened_files();
            }
            else if(choice==10){
                unmount_disk();
                printf("disk unmounted successfully\n");
                return ;
            }
            else
                printf("ivalid option entered; enter valid option\n");
        }
    }

};

unordered_map<string,DISK> diskNameToDiskObj;


void close_disk(); // pending


int main(){
    int choice;
    string diskname;
    while(1){
        printf("disk creation menu\n");
        printf("1. create disk\n");
        printf("2. mount disk\n");
        printf("3. exit\n");
        cin >> choice;
        if(choice==1){
            printf("enter disk name\n");
            cin >> diskname;
            if(diskNameToDiskObj.find(diskname)!=diskNameToDiskObj.end()){
                printf("disk name already exists; give new disk name\n");
                continue;
            }
            
            else{
                DISK d = DISK(diskname);
                d.create_disk();
                diskNameToDiskObj.insert(make_pair(diskname,d));
                printf("disk creation successfull\n");
            }
            
        }
        else if(choice==2){
            printf("enter disk name\n");
            cin >> diskname;
            if(diskNameToDiskObj.find(diskname)==diskNameToDiskObj.end()){
                printf("disk name doesn't exist; give correct disk name\n");
                continue;
            }
            
            else{
                DISK d = (diskNameToDiskObj.find(diskname))->second;
                d.mount_disk();
                printf("disk mount successfull\n");
                d.handleMenu();
            }
        }
        else if(choice==3)
            return 0;
        else
            printf("ivalid option entered; enter valid option\n");
        
    }
}