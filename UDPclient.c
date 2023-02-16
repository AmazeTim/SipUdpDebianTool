#include "headerFiles.h"
# define BUFSIZE 1024
char* get_ip(int*s);
char* uuid(char out[UUID_STR_LEN]);
char* gen_num(int len);
char* get_message(char* servName,int servPort,char*clntName,int clntPort);
int findstr(char * str);
void packet_handler(u_char *user, const struct pcap_pkthdr *header, const u_char *packet);
void* child(void* arg);
char* createFolder(void);
void packet_handler(u_char *user, const struct pcap_pkthdr *header, const u_char *packet);
typedef struct threadData{
  pcap_t *handle;
  int num_packets;
}threadData;
int main(int argc, char *argv[]) {//-ip -port -max_retry
  int s;
  int slen,rlen;
  char* servName;
  int servPort;
  char buffer[BUFSIZE+1];
  //check the parameters
  if(argc !=5)
  {
    printf("Error:four arguments are needed!\n");
    exit(1);
  }
  servName = argv[1];
  servPort = atoi(argv[2]);
  //build server socket address
  struct sockaddr_in servAddr;
  memset(&servAddr,0,sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  inet_pton(AF_INET,servName,&servAddr.sin_addr);
  servAddr.sin_port = htons(servPort);
  //setting and starting sniffing tools
  pcap_t *handle;
  char errbuf[PCAP_ERRBUF_SIZE];  /* Error string */
  struct bpf_program fp;    /* The compiled filter */
  char filter_exp[]="udp";
  // char filter_exp[]="port ";
  // strcat(filter_exp,argv[3]);/* The filter expression */
  bpf_u_int32 mask;   /* Our netmask */
  bpf_u_int32 net;    /* Our IP */
  struct pcap_pkthdr header;  /* The header that pcap gives us */
  const u_char *packet;   /* The actual packet */
  handle = pcap_open_live("eth0", BUFSIZ, 1, 1000, errbuf);
  if (handle == NULL) {
      fprintf(stderr, "Couldn't open device %s: %s\n", "eth0", errbuf);
      return(1);
  }
  if (pcap_lookupnet("eth0", &net, &mask, errbuf) == -1) {
      fprintf(stderr, "Couldn't get netmask for device %s: %s\n", "eth0", errbuf);
      net = 0;
      mask = 0;
  }
  if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
      fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
      return(1);
  }
  if (pcap_setfilter(handle, &fp) == -1) {
      fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
      return(1);
  }
  //construct threading for sniffing
  pthread_t t;
  threadData p;
  p.handle=handle;
  p.num_packets=-1;//default sniffing until error occuring
  pthread_create(&t, NULL, child,(void*)&p); // constuct sub-thread
  //create socket
  if((s = socket(PF_INET,SOCK_DGRAM,0)) <0 ){
    perror("Error: socket failed!");
    exit(1);
  }
  else{
    printf("Create socket successfully\n");
  }
  //construct SIP message
  int localPort = atoi(argv[3]);
  char* localName=get_ip(&s);
  char *string= get_message(servName,servPort,localName,localPort);
  printf("String Content:\n%s",string);
  //setting timeout
  struct timeval timeout;      
  timeout.tv_sec = 0;
  timeout.tv_usec = 500000;
  setsockopt (s, SOL_SOCKET, SO_SNDTIMEO, &timeout,sizeof timeout);
  setsockopt (s, SOL_SOCKET, SO_RCVTIMEO, &timeout,sizeof timeout);
  long interval= 500000;//base 500ms
  int max_retry = atoi(argv[4]);
  //sending and receiving
  for(int i=0;i<max_retry;i++){
    printf("Sending...\n");
    slen=sendto(s,string,strlen(string),0,(struct sockaddr*)&servAddr,sizeof(servAddr));
    if ( slen < 0){
        if(i==(max_retry-1)){
          perror("Sending failed and reach the max_retry times\n");
          close(s);
          pcap_breakloop(handle);
          pcap_close(handle);
          pthread_join(t, NULL);
          exit(1);
        }
        perror("Sending falied");
        // printf("Try %d times\n",i+1);
        // printf("Sleep %06ld\n",interval);
        usleep(interval);
        if(interval < 8000000){
          interval *= 2;// wait_interval = base * 2^n 
          }
        if(interval >8000000){
          interval=8000000;
        }   
        continue;
      }
    else{
      printf("Receiving...\n");
      rlen=recvfrom(s,buffer,BUFSIZE,0,NULL,NULL);
      if (rlen < 0){
        if(i==(max_retry-1)){
          perror("Receiving failed and reach the max_retry times!");
          close(s);
          pcap_breakloop(handle);
          pcap_close(handle);
          pthread_join(t, NULL);
          exit(1);
        }
        perror("Receiving falied");
        // printf("Try %d times\n",i+1);
        // printf("Sleep %06ld\n",interval);
        usleep(interval);
        if(interval < 8000000){
          interval *= 2;// wait_interval = base * 2^n 
          }
        if(interval >8000000){
          interval=8000000;
        }   
        continue;
      }
      else{
        if(rlen > BUFSIZE){
          perror("Msg size is bigger than receiving buffer size\n");
          close(s);
          pcap_breakloop(handle);
          pcap_close(handle);
          pthread_join(t, NULL);
          exit(1);
        }
        else{
          break;
        }
      }
    }
  }
  //print and verify relpy string
  buffer[rlen]='\0';
  if(findstr(buffer)<0){
    perror("Server Error!");
    fputs(buffer,stdout);
    close(s);
    pcap_breakloop(handle);
    pcap_close(handle);
    pthread_join(t, NULL);
    exit(1);
  }
  else{
    printf("Server Reply:\n");
    fputs(buffer,stdout);
    close(s);
    pcap_breakloop(handle);
    pcap_close(handle);
    pthread_join(t, NULL);
    exit(0);
  }
}
char* get_ip(int*s){ /* IP address attached to "eth0" */
    struct ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    ioctl(*s, SIOCGIFADDR, &ifr);
    return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}
char* uuid(char out[UUID_STR_LEN]){
  uuid_t b;
  uuid_generate(b);
  uuid_unparse_lower(b, out);
  return out;
}
char* gen_num(int len){
  char *buf=malloc (sizeof (char) * (len+1));
  for(int i=0;i<len;i++){
    buf[i]=rand()%10 +'0';
  }
  buf[len]= '\0';
  return buf;
}
char* get_message(char* servName,int servPort,char*clntName,int clntPort){
  char *string=malloc (sizeof (char) * (1024));
  char *branch = gen_num(19);
  char out[UUID_STR_LEN]={0};
  char *tag = uuid(out);
  snprintf(string, 1024, "OPTIONS sip:ping@%s:%d SIP/2.0\r\nVia: SIP/2.0/UDP %s:%d;branch=z9hG4bK__%s\r\nFrom: <sip:ping@%s>;tag=%s\r\nTo: <sip:ping@%s>\r\nMax-Forwards: 70\r\nContact: <sip:ping@%s:%d;transport=UDP>\r\nCall-ID: %s\r\nCSeq: 1 OPTIONS\r\nSupported: outbound\r\nUser-Agent: sipping\r\nContent-Length: 0\r\n\r\n", servName,servPort,clntName,clntPort,branch,clntName,tag,servName,clntName,clntPort,tag);
  return string;
}
int findstr(char * str){
    char *word = "SIP/2.0 200 OK";
    char *pch = strstr(str, word);
    if(pch){
        return 0;
    }  
    else{
        return -1;
    }
}
void* child(void* arg) {
    threadData *data=(threadData *)arg;
    pcap_t *handle=data->handle;
    int num_packets=data->num_packets;
    pcap_loop(handle, num_packets, packet_handler, NULL);
    pthread_exit(NULL);
}
void packet_handler(u_char *user, const struct pcap_pkthdr *header, const u_char *packet)
{
    static int count = 1;
    char filename[256];
    pcap_dumper_t *dumper;
    // create pcap data folder
    char* folder=createFolder();
    // open a new PCAP file for writing
    snprintf(filename, sizeof(filename), "%s/packet-%03d.pcap",folder,count++);
    dumper = pcap_dump_open(pcap_open_dead(DLT_EN10MB, BUFSIZ), filename);
    // write the packet to the file
    pcap_dump((u_char *) dumper, header, packet);
    // close the file
    pcap_dump_close(dumper);
    printf("Packet captured and saved to %s\n", filename);
}
char* createFolder(void){
    char *out=malloc(sizeof(char)*64);
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char tmp[]="/data";
        strcat(cwd,tmp);
    }
    else {
        perror("getcwd() error");
        exit(1);
    }
    struct stat st = {0};
    if (stat(cwd, &st) == -1) {
        mkdir(cwd, 0700);
    }
    strcpy(out,cwd);
    return out;
}
