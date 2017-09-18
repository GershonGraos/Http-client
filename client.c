#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>

//*********************************************************
//*********************************************************

#define IS_H 1
#define IS_D 2
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"

//*********************************************************
//*************  PRIVATE FUNCTIONS ************************

int check_H(char*);
void check_D(char*);
int check_Date(char*);
void check_URL(char*);
int connect_to_server(char* , int);
void write_and_read(int fd);

//*********************************************************
//************** GLOBAL VARIABLES *************************

char* website; //host
char* port;
char* files;
char* day;
char* hour;
char* minute;
char* new_url_if_port;
char* request;
int flag_port = 0;
int port_int = 0;
int day_int;
int hour_int;
int minute_int;
char timebuf[128];
time_t now;

//*********************************************************
//*****************  MAIN  ********************************
int main(int argc, char* argv[])
{   
    // Create malloc //
    website = (char*)malloc(512);
    port = (char*)malloc(512);
    files = (char*)malloc(512);
    day = (char*)malloc(512);
    hour = (char*)malloc(512);
    minute = (char*)malloc(512);
    new_url_if_port = (char*)malloc(512); 
    request = (char*)malloc(512);
    
    
    if(argc > 5 || argc < 2) // enter only url
    {
        perror("Wrong input - Please enter correct request\n"); 
        exit(1);
    }

    // ---Check the syntax of the input--- // 
    int i, c; 
    int counter_h = 0;
    int counter_d = 0;
    int counter_url = 0;
    int val_h = 0;
    int val_d = 0;
    int i_d = 0;
    int i_h = 0;
    int i_url = 0;
    int length = 0;
    char *check;
    char *command;
    char *url;

    for(i = 1; i < argc; i++)
    {     
        if( ( c = strcmp(argv[i],"-h") ) == 0 ) 
        { 
            counter_h++;
            i_h = i;
        }             
        else if( ( c = strcmp(argv[i],"-d") ) == 0 ) 
        { 
            counter_d++;
            i_d = i;
        }          
        else if( ( check = strstr(argv[i], "http://") ) != NULL  ) 
        {   
            counter_url++;
            i_url = i;  
        } 
    } 
    
    length = counter_d + counter_h + counter_url + 1;  
    if(counter_d == 1)
        length++;
    
    if(counter_h >1 || counter_d >1 || counter_url != 1 || length != argc) 
    {
        perror("Wrong input - Please enter correct request\n"); 
        exit(1);
    }
     
    // URL //
    url = argv[i_url];
    check_URL(url);


    // Check H //
    if(counter_h == 1)
        check_H( argv[i_h] );   

        
    // Check D //    
    if(counter_d == 1)
    {   
        check_D( argv[i_d] );
        val_d = check_Date(argv[i_d+1]);
    }  

    
    // Construct Request //
    request[0] = '\0';
    if(counter_h == 1)
        strcat(request,"HEAD ");
        
    else
        strcat(request,"GET ");    
    
    strcat(request,url);
    strcat(request," HTTP/1.0");
    
    if(counter_d == 1)
    {
        strcat(request, "\r\n");
        strcat(request, "If-Modified-Since: ");
        strcat(request, timebuf);
    }
    strcat(request, "\r\n\r\n");
    
    
    // PRINTING THE BUFFER //
    printf("\n%s\nLEN = %d\n",request,(int)strlen(request)); 

        
    // Connect to the server //
    if(flag_port == 1)
    {   port_int = atoi(port);
        if(port_int < 1024 || port_int > 65535){perror("\nWrong port\n"); exit(1);}   }
    
    int fd = connect_to_server(website, port_int);


    // Write and Read //
    write_and_read(fd);
    
    
    // FREE //
    free(website);
    free(port);
    free(day);
    free(hour);
    free(minute);
    free(new_url_if_port);
    free(request);
    return 0 ;
}


//***************************************************************
//********************* CHECK H *********************************
int check_H(char* h)
{
    if(h[2] != '\0') 
    {
        printf("\nerrorH\n");
        perror("Please enter correct request"); 
        exit(1);
    } 
    return IS_H;         
}     


//***************************************************************
//********************* CHECK D *********************************
void check_D(char* d)
{
    if(d[2] != '\0') 
    {
        printf("\nerrorD\n");
        perror("Please enter correct request"); 
        exit(1);
    }          
}
 
//***************************************************************
//******************  CHECK DATE ******************************** 
int check_Date(char* date)
{
    int counter = 0;
    int size_date = 0;
    int i = 0;
    int j = 0;
    int k;
    int i_day = 0;
    int i_hour = 0;
    int i_minute = 0;
    int c;
    
    // Save the place of day and minute //
    while(date[i] != '\0')
    {
        if(date[i] == ':') 
        {
            counter++; 
            if(counter == 1)
                i_day = i;
            else if(counter == 2)
                i_minute = i;    
        }    
        i++;
    }
    size_date = i;
    
    if(counter != 2)
    {
        perror("Wrong input - The date is not available-day:hour:minute\n");   
        exit(1);
    }  
    
    // Add data to: day,hour,minute //
    int s_d =0;
    int s_h =0;
    int s_m =0;
   
    for(k=0; k<i_day; k++)
    { 
        if( isdigit(date[k]) )
        {
            day[s_d] = date[k];  
            s_d++;     
        }
        else
        {
            perror("The date is composed of numbers!!\n");
            exit(1);
        } 
    }
    
    
    for(k=i_day+1; k<i_minute; k++)
    { 
        if( isdigit(date[k]) )
        {
            hour[s_h] = date[k];   
            s_h++;     
        }
        else
        {
            perror("The date is composed of numbers!!\n");
            exit(1);
        }    
    }
    
    
    for(k=i_minute+1; k<size_date; k++)
    { 
        if( isdigit(date[k]) )
        {
            minute[s_m] = date[k];  
            s_m++;     
        }
        else
        {
            perror("The date is composed of numbers!!\n");
            exit(1);
        } 
    }
    
    // Converter char to int //
    day_int = atoi(day);
    hour_int = atoi(hour);
    minute_int = atoi(minute);
    
    now = time(NULL);
    now = now - (day_int*24*3600+hour_int*3600+minute_int*60);
    strftime( timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now) ); 
    
    return IS_D;
}           
  
//**************************************************
//***************** Check URL *********************
void check_URL(char* url)
{
    char* temp;
    char* temp_f;
    int diff_port = 0;
    int there_file = 0;
    int i_port=0;
    int i_file=0;
    int i=0;
    int j=0;
    int k=0;
    int l=0;
    int m=0;
    int n=0;
    

    // CHECK - if previously of http there is strings  //
    char ch[] = "http://";
    for(; j<7; j++)
    {
        if(url[j] != ch[j])
        {   
            perror("Wrong input - Please enter correct request"); 
            exit(1);
        }   
    }
    
    if(url[7] != 'w')
    {  
        perror("\nwrong input\n");
        exit(1);
    }
    
    //  CHECK - if there is website //
    temp = strstr(url,"www");
    if (temp == NULL)
    {
        perror("Please enter address"); 
        exit(1);
    }


    // CHECK - if there is different port and files //
    while(temp[i] != '\0')  
    {
        if(temp[i] == ':')
        {
            diff_port++;
            i_port = i+1;
            n = i;
        }    
        
        else if(temp[i] == '/')
        {
            there_file++;
            i_file = i+1;
        } 
        
        i++;           
    } 
    
    // SAVE NETUNIM //
    // WEBSITE //
    while(temp[k] != '\0' && temp[k] != ':' && temp[k] != '/' ) 
    { 
        website[k] = temp[k];
        k++;   
    }

    // FILES //
    if(there_file > 0)   
        files = strstr(temp,"/"); 


    // PORT //
    if(diff_port > 1)
    {   
        if(i_file > 0)
        {
            if(i_port > i_file)
            {
                perror("The place of the port is not correct\n"); 
                exit(1); 
            }               
        }
        perror("You can add only one port\n"); 
        exit(1);      
    }
    
    // There is different port //
    else if(diff_port == 1)  
    {
        while(temp[i_port] != '\0' && temp[i_port] != '/')
        {
            if( isdigit(temp[i_port]) )
            {
                port[l] = temp[i_port];
                l++; 
                i_port++;        
            }
            else
            {
                perror("If you add port, please enter correct port");  
                exit(1);
            }  
        }
     flag_port = 1;   
    }
    
    // New URL if there is new port //
    int x=0;
    if(flag_port == 1)
    {
        while(url[m] != '\0')
        {            
            new_url_if_port[x] = url[m]; 
            x++;
            m++;
            if( m == n+7 )
                m = i_port+7;
        }
    }
}

//**************************************************************
//*****************  Connect to the Server *********************
int connect_to_server(char* website, int port) 
{
    // Port //
    if(flag_port != 1)
        port = 80;  //port of internet
         
    int fd; // socket descriptor    
    struct sockaddr_in server_addr;
    struct hostent* server_host;
	if((server_host = gethostbyname(website)) == NULL)
    {
        herror("gethostbyname()");
    	exit(1);
	}
	
    // Create the socket //
	if ( (fd = socket(PF_INET,SOCK_STREAM, 0)) < 0 )
    {
   	    perror("Socket");
        exit(errno);
    }
    
    // Connecting //
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(port);
   	server_addr.sin_addr = *((struct in_addr *)server_host->h_addr);
   	
    if ( connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0 )
   	{
        perror("Connect");
       	exit(1);
    } 
 
    return fd;
}

//**************************************************************
//******************** Read/Write ******************************
void write_and_read(int fd)
{
    int total = 0; // total bytes
    int wr_nbytes; // used by write() 
    int rd_nbytes; // used by read()
    char rd_buf[1024]; 
    
    memset(rd_buf, '\0', sizeof(rd_buf) ); //add '\0' for each place of rd_buf
    
    // Write // 
    if((wr_nbytes = write(fd, request, strlen(request) )) < 0) 
    {
        perror("write");
        exit(1);
    }
    printf("nbytes : %d\n\n",wr_nbytes);  

     
    // Read // 
    do
    {
        if((rd_nbytes = read(fd, rd_buf, sizeof(rd_buf))) < 0) 
        {
            perror("read");
            exit(1);
        }
         
        printf("%s\n" , rd_buf );   
        total += rd_nbytes ; 
        memset( rd_buf, '\0', sizeof(rd_buf) );
    }
    while (rd_nbytes > 0);  // while there is something to read
    
    printf("Total received response bytes: %d\n\n", total);
}
