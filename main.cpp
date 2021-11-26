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



void create_disk(){

    S new_disk_sb; // super block
    INODE* inode_array;
    DATA_BLOCK* disk_blocks_array;

    bool *inode_bitmap,*disk_block_bitmap;

    int i;


    new_disk_sb.number_of_inodes=number_of_inodes;
    new_disk_sb.number_of_data_blocks = number_of_data_blocks;

    // initialize inode and disk_block bitmaps
    inode_bitmap = (bool*)malloc(sizeof(bool)*number_of_inodes);
    disk_block_bitmap = (bool*)malloc(sizeof(bool)*number_of_data_blocks);

    // initialize inode array
    inode_array = (INODE*)malloc(sizeof(INODE)*new_disk_sb.number_of_inodes);
    for(i=0;i<new_disk_sb.number_of_inodes;i++){
        strcpy(inode_array[i].name,"");
        inode_array[i].starting_disk_block=i;
        inode_array[i].ending_disk_block=-1;
        inode_array[i].mode_opened='r';
        inode_bitmap[i]=false;
    }

    // initialize disk block array
    disk_blocks_array = (DATA_BLOCK*)malloc(sizeof(DATA_BLOCK)*new_disk_sb.number_of_data_blocks);
    for(i=0;i<new_disk_sb.number_of_data_blocks;i++){
        strcpy(disk_blocks_array[i].data,"empty");
        disk_blocks_array[i].next_data_block_num=i;
        disk_block_bitmap[i]=false;
    }

    FILE* new_disk_data_fp = fopen("new_disk_data","w");

    // write super block
    fwrite(&new_disk_sb,sizeof(S),1,new_disk_data_fp);


    // write inode bitmap
    fwrite(inode_bitmap,sizeof(bool),new_disk_sb.number_of_inodes,new_disk_data_fp);


    // write disk_block_arr bitmap
     fwrite(disk_block_bitmap,sizeof(bool),new_disk_sb.number_of_data_blocks,new_disk_data_fp);
        

    // write inode metada
    fwrite(inode_array,sizeof(INODE),new_disk_sb.number_of_inodes,new_disk_data_fp);


    // // write disk block array metadata
    // fwrite(disk_blocks_array,sizeof(DATA_BLOCK),new_disk_sb.number_of_data_blocks,new_disk_data_fp);
    

    fclose(new_disk_data_fp);

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


void mount_disk(){

    

    S new_disk_sb; // super block
    INODE* inode_array;
    DATA_BLOCK* disk_blocks_array; // arrays pointing to the respective entries

    bool *inode_bitmap,*disk_block_bitmap;


    FILE* new_disk_data_fp = fopen("new_disk_data","r");
    // read super block
    fread(&new_disk_sb,sizeof(S),1,new_disk_data_fp);

    int numberofinodes = new_disk_sb.number_of_inodes;
    int numberofdatablocks = new_disk_sb.number_of_data_blocks;

    inode_bitmap = (bool*)malloc(sizeof(bool)*numberofinodes);
    disk_block_bitmap = (bool*)malloc(sizeof(bool)*numberofdatablocks);
    
    int i;


    for(int i=0;i<new_disk_sb.number_of_inodes;i++)
        fread(&inode_bitmap[i],sizeof(bool),1,new_disk_data_fp);

    // write disk_block_arr bitmap
    for(int i=0;i<new_disk_sb.number_of_data_blocks;i++)
        fread(&disk_block_bitmap[i],sizeof(bool),1,new_disk_data_fp);

    // read inode metada
    inode_array = (INODE*)malloc(sizeof(INODE)*new_disk_sb.number_of_inodes);
    for(i=0;i<new_disk_sb.number_of_inodes;i++)
        fread(&inode_array[i],sizeof(INODE),1,new_disk_data_fp);


    // read disk block array metadata
    disk_blocks_array = (DATA_BLOCK*)malloc(sizeof(DATA_BLOCK)*new_disk_sb.number_of_data_blocks);
    for(i=0;i<new_disk_sb.number_of_data_blocks;i++)
        fread(&disk_blocks_array[i],sizeof(DATA_BLOCK),1,new_disk_data_fp);

    fclose(new_disk_data_fp);

    print_fs_info(new_disk_sb,inode_bitmap,disk_block_bitmap,inode_array,disk_blocks_array);
}

void close_disk();


int main(){
    cout << sizeof(S) << " " << sizeof(INODE) << " " << sizeof(DATA_BLOCK) << endl;
    create_disk();
    mount_disk();
}