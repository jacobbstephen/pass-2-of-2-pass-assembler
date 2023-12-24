#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define text_len 61
#define len 100
#define arr_len 7

char line[len], label[arr_len], opcode[arr_len],operand[arr_len],opvalue[arr_len], hex[arr_len];
char addr[arr_len]; int textrec_len;
char text_rec[len]; char opvalue[arr_len];
FILE *symtab, *intermediate, *object, *optab;

void readLine(){
    label[0] = opcode[0] = operand[0] = hex[0] = '\0';
    fgets(line,len,intermediate);
    int cnt = sscanf(line,"%s %s %s %s ",hex,label,opcode,operand) ;
    if(cnt == 3){
        sscanf(line,"%s %s %s",hex,opcode,operand);
        label[0] = '\0';
    }else if(cnt == 2){
        sscanf(line,"%s %s",hex,opcode);
        label[0] = operand[0] = '\0';
    }
    else sscanf(line,"%s %s %s %s\n", hex,label,opcode,operand);
}

bool search(FILE* ptr, char arr[]){
    rewind(ptr);
    char symbol[arr_len];
    while(fscanf(ptr, "%s %s", symbol, addr) != EOF){
        if(!strcmp(arr,symbol)) return true;
    }
    return false;
}

void pass2(){
    fscanf(intermediate, "%s %s %s\n",label,opcode,operand);
    hex[0] = '\0';
    text_rec[0] = '\0';
    char locctr[arr_len];
    bool found, set_error_flag;
    int length = 16;
    char start_addr[arr_len];
    if(!strcmp(opcode,"START")){
        strcpy(start_addr,operand);
        fprintf(object,"H^%-6s^%06s^%06x\n",label,start_addr,length);
    }
    readLine();
    strcpy(text_rec,"T^00");
    strcat(text_rec,start_addr);
    strcat(text_rec,"^00^");
    textrec_len = 0;
    while(strcmp(opcode,"END") != 0){
        found = search(optab,opcode);
        if (found) {
            if(operand[0]){
                strcpy(opvalue,addr);
               found =  search(symtab,operand);
                if(!found) set_error_flag = false;
            } 
            else strcpy(addr,"0000"); 
            strcat(text_rec,opvalue);
            strcat(text_rec,addr);
            strcat(text_rec,"^");
            textrec_len += 6;
        }
        else if (!strcmp(opcode,"WORD")){
            strcat(text_rec,"0000");
            strcat(text_rec,operand);
            strcat(text_rec,"^");
            textrec_len += 6;
        }
        else if(!strcmp(opcode,"BYTE")){

            if(operand[0] == 'X'){
                char c[2];
                printf("I am in B %s %d\n", operand, (strlen(operand) - 3)/2);
                for(int i = 2; i < strlen(operand) - 1; i++){
                    sprintf(c,"%c",operand[i]);
                    strcat(text_rec,c);
                }
                textrec_len += strlen(operand) - 3;
            } 
            else if(operand[0] == 'C'){
                char c[2];
                for(int i = 2; i < strlen(operand) - 1; i++){
                    sprintf(c,"%X",operand[i]);
                    strcat(text_rec,c);
                }
                strcat(text_rec,"^");
                textrec_len += 6;
            } 
        }
        else printf("INVALID\n");
        readLine();
        if (textrec_len == 60 || strlen(text_rec) >= 75){
            char str[7];
            fprintf(object,"%s\n",text_rec);
            textrec_len = 0;
            text_rec[0] = '\0';
            strcpy(text_rec,"T^00");
            sprintf(str,"%06X",hex);
            strcat(text_rec,str);
            strcat(text_rec,"^00^");
        }
    }
    textrec_len = textrec_len / 2;
    char str[3];
    sprintf(str,"%d",textrec_len);
    strncpy(text_rec+9,str,2);
    fprintf(object,"%s\n",text_rec);
    fprintf(object,"E^%06s\n",start_addr);

    int program_length = (int) strtol(hex,NULL,16) - (int) strtol(start_addr,NULL,16);
    fseek(object,16,SEEK_SET);
    fprintf(object,"%06X\n",program_length);
}
int main(){
    optab = fopen("optab.txt","r+");
    object = fopen("object.txt","w+");
    symtab = fopen("symtab.txt","r");
    intermediate = fopen("intermediate.txt","r");
    pass2();
    printf("Object code generated\n");
    return(0);
}